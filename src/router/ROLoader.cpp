//---------------------------------------------------------------------------//
//                        ROLoader.cpp -
//  Loader for networks and route imports
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.14  2004/01/26 09:54:29  dkrajzew
// the loader now stops on errors as described within the manual
//
// Revision 1.13  2004/01/26 08:01:10  dkrajzew
// loaders and route-def types are now renamed in an senseful way;
//  further changes in order to make both new routers work;
//  documentation added
//
// Revision 1.12  2003/08/18 12:44:54  dkrajzew
// xerces 2.2 and later compatibility patched
//
// Revision 1.11  2003/07/07 08:36:58  dkrajzew
// Warnings are now reported to the MsgHandler
//
// Revision 1.10  2003/06/18 11:20:54  dkrajzew
// new message and error processing: output to user may be a message, warning
//  or an error now; it is reported to a Singleton (MsgHandler);
// this handler puts it further to output instances.
// changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.9  2003/05/20 09:48:35  dkrajzew
// debugging
//
// Revision 1.8  2003/04/09 15:39:11  dkrajzew
// router debugging & extension: no routing over sources, random routes added
//
// Revision 1.7  2003/04/01 15:19:51  dkrajzew
// behaviour on broken nets patched
//
// Revision 1.6  2003/03/20 16:39:16  dkrajzew
// periodical car emission implemented; windows eol removed
//
// Revision 1.5  2003/03/12 16:39:19  dkrajzew
// artemis route support added
//
// Revision 1.4  2003/03/03 15:08:21  dkrajzew
// debugging
//
// Revision 1.3  2003/02/07 10:45:04  dkrajzew
// updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <parsers/SAXParser.hpp>
#include <util/PlatformUtils.hpp>
#include <util/TransService.hpp>
#include <sax2/SAX2XMLReader.hpp>
#include <utils/options/OptionsCont.h>
#include <utils/convert/ToString.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/XMLHelpers.h>
#include <utils/importio/LineReader.h>
#include "RONet.h"
#include "RONetHandler.h"
#include "ROLoader.h"
#include "RORDLoader_TripDefs.h"
#include "ROWeightsHandler.h"
#include "RORDLoader_SUMORoutes.h"
#include "RORDLoader_Cell.h"
#include "RORDLoader_SUMOAlt.h"
#include "RORDLoader_Artemis.h"
#include "RORDGenerator_Random.h"
#include "RORDGenerator_ODAmounts.h"
#include "ROAbstractRouteDefLoader.h"


/* =========================================================================
 * xerces 2.2 compatibility
 * ======================================================================= */
#if defined(XERCES_HAS_CPP_NAMESPACE)
using namespace XERCES_CPP_NAMESPACE;
#endif


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
ROLoader::ROLoader(OptionsCont &oc,
                   bool emptyDestinationsAllowed)
    : _options(oc), myEmptyDestinationsAllowed(emptyDestinationsAllowed)
{
}


ROLoader::~ROLoader()
{
    for(RouteLoaderCont::iterator i=_handler.begin(); i!=_handler.end(); i++) {
        delete (*i);
    }
}


RONet *
ROLoader::loadNet(ROAbstractEdgeBuilder &eb)
{
    RONet *net = new RONet(_options.isSet("sumo-input"));
    std::string file = _options.getString("n");
    if(_options.getBool("v")) {
        cout << "Loading net... ";
    }
    RONetHandler handler(_options, *net, eb);
	handler.setFileName(file);
	XMLHelpers::runParser(handler, file);
    if(MsgHandler::getErrorInstance()->wasInformed()) {
		MsgHandler::getErrorInstance()->inform("failed.");
        delete net;
        return 0;
    } else {
		MsgHandler::getMessageInstance()->inform("done.");
	}
    // build and prepare the parser
    return net;
}


void
ROLoader::openRoutes(RONet &net, float gBeta, float gA)
{
	// build loader
	    // load additional precomputed sumo-routes when wished
	openTypedRoutes(new RORDLoader_SUMORoutes(net), "sumo-input");
		// load the XML-trip definitions when wished
	openTypedRoutes(
        new RORDLoader_TripDefs(net, myEmptyDestinationsAllowed),
        "trip-defs");
		// load the cell-routes when wished
	openTypedRoutes(new RORDLoader_Cell(net, gBeta, gA), "cell-input");
		// load artemis routes when wished
	openTypedRoutes(new RORDLoader_Artemis(net), "artemis-input");
		// load the sumo-alternative file when wished
	openTypedRoutes(new RORDLoader_SUMOAlt(net, gBeta, gA), "alternatives");
	// build generators
		// load the amount definitions if wished
	openTypedRoutes(
        new RORDGenerator_ODAmounts(net, myEmptyDestinationsAllowed),
        "flows");
		// check whether random routes shall be build, too
    if(_options.isSet("R")) {
        RORDGenerator_Random *randGen = new RORDGenerator_Random(net);
        randGen->init(_options);
        _handler.push_back(randGen);
    }
	skipUntilBegin();
}

void
ROLoader::skipUntilBegin()
{
    MsgHandler::getMessageInstance()->inform("Skipping");
    for(RouteLoaderCont::iterator i=_handler.begin(); i!=_handler.end(); i++) {
        (*i)->skipUntilBegin();
    }
    MsgHandler::getMessageInstance()->inform(
        string("Skipped until: ") + toString<long>(getMinTimeStep()));
}


void
ROLoader::processRoutesStepWise(long start, long end,
                                RONet &net, ROAbstractRouter &router)
{
	long absNo = end - start;
    // skip routes that begin before the simulation's begin
    // loop till the end
    bool endReached = false;
    bool errorOccured = false;
    long time = getMinTimeStep();
    long firstStep = time;
    long lastStep = time;
    for(; (!endReached||net.furtherStored())&&time<end&&!errorOccured; time++) {
        if(_options.getBool("v")) {
			double perc =
				(double) (time-start) / (double) absNo;
			cout.setf ( ios::fixed , ios::floatfield ) ; // use decimal format
			cout.setf ( ios::showpoint ) ; // print decimal point
            cout << "Reading time step: " << time
				<< "  (" << (time-start) << "/" <<  absNo
				<< " = " << setprecision( 2 ) << perc * 100 << "% done)       " << (char) 13;
        }
        endReached = true;
        RouteLoaderCont::iterator i;
        // go through all handlers
        for(i=_handler.begin(); i!=_handler.end(); i++) {
            // load routes until the time point is reached
            (*i)->readRoutesAtLeastUntil(time+1);
            // save the routes
            net.saveAndRemoveRoutesUntil(_options, router, time);
        }
        // check whether further data exist
        endReached = true;
        lastStep = time;
        for(i=_handler.begin(); endReached&&i!=_handler.end(); i++) {
            if(!(*i)->ended()) {
                endReached = false;
            }
        }
        errorOccured = MsgHandler::getErrorInstance()->wasInformed();
    }
    time = end;
    double perc =
        (double) (time-start) / (double) absNo;
    cout.setf ( ios::fixed , ios::floatfield ) ; // use decimal format
    cout.setf ( ios::showpoint ) ; // print decimal point
    cout << "Reading time step: " << time
        << "  (" << (time-start) << "/" <<  absNo
        << " = " << setprecision( 2 ) << perc * 100 << "% done)       " << endl;
    MsgHandler::getMessageInstance()->inform(
        string("Routes found between time steps ") + toString<int>(firstStep)
        + string(" and ") + toString<int>(lastStep) + string("."));
}


unsigned int
ROLoader::getMinTimeStep() const
{
    unsigned int ret = LONG_MAX;
    for(RouteLoaderCont::const_iterator i=_handler.begin(); i!=_handler.end(); i++) {
        unsigned int akt = (*i)->getCurrentTimeStep();
        if(akt<ret) {
            ret = akt;
        }
    }
    return ret;
}


void
ROLoader::processAllRoutes(unsigned int start, unsigned int end,
                           RONet &net,
                           ROAbstractRouter &router)
{
    bool ok = true;
    for(RouteLoaderCont::iterator i=_handler.begin(); ok&&i!=_handler.end(); i++) {
        (*i)->readRoutesAtLeastUntil(end);
    }
    // save the routes
    net.saveAndRemoveRoutesUntil(_options, router, end);
}


void
ROLoader::closeReading()
{
    // close the reading
    for(RouteLoaderCont::iterator i=_handler.begin(); i!=_handler.end(); i++) {
        (*i)->closeReading();
    }
}


void
ROLoader::openTypedRoutes(ROAbstractRouteDefLoader *handler,
                          const std::string &optionName)
{
	// check whether the current loader is wished
	if(!_options.isSet(optionName)) {
		delete handler;
		return;
	}
    // check the given files
    if(!FileHelpers::checkFileList(_options.getString(optionName))) {
        MsgHandler::getErrorInstance()->inform(
            string("The list of ") + handler->getDataName() +
            string("' is empty!"));
        throw ProcessError();
    }
    // allocate a reader and add it to the list
    addToHandlerList(handler,
		_options.getString(optionName));
}


void
ROLoader::addToHandlerList(ROAbstractRouteDefLoader *handler,
                           const std::string &fileList)
{
    StringTokenizer st(fileList, ";");
    while(st.hasNext()) {
        // get the file name
        string file = st.next();
        // check whether the file can be used
        //  necessary due to the extensions within cell-import
        if(!handler->checkFile(file)) {
            MsgHandler::getErrorInstance()->inform(
                string("Problems with ")
                + handler->getDataName() + string("-typed input '")
                + file + string("'."));
            throw ProcessError();
        }
        // build the instance when everything's all right
        ROAbstractRouteDefLoader *instance =
			handler->getAssignedDuplicate(file);
        if(!instance->init(_options)) {
            delete instance;
            MsgHandler::getErrorInstance()->inform(string("The loader for ") + handler->getDataName()
                + string(" from file '") + file + string("' could not be initialised."));
            throw ProcessError();
        }
        _handler.push_back(instance);
    }
    // delete the archetype
    delete handler;
}


bool
ROLoader::loadWeights(RONet &net)
{
    string weightsFileName = _options.getString("w");
    // check whether the file exists
    if(!FileHelpers::exists(weightsFileName)) {
        MsgHandler::getErrorInstance()->inform(
            string("The weights file '") + weightsFileName
            + string("' does not exist!"));
        return false;
    }
    // build and prepare the weights handler
    ROWeightsHandler handler(_options, net, weightsFileName);
    MsgHandler::getMessageInstance()->inform(
        "Loading precomputed net weights.");
    // build and prepare the parser
	XMLHelpers::runParser(handler, weightsFileName);
    bool ok = !MsgHandler::getErrorInstance()->wasInformed();
    // report whe wished
    if(ok) {
        MsgHandler::getMessageInstance()->inform("done.");
    } else {
        MsgHandler::getMessageInstance()->inform("failed.");
    }
    return ok;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


