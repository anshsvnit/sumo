/***************************************************************************
                          NBJunctionLogicCont.cpp
			  Class for the io-ing between junctions (nodes) and the computers
        mass storage system
                             -------------------
    project              : SUMO
    subproject           : netbuilder / netconverter
    begin                : Tue, 20 Nov 2001
    copyright            : (C) 2001 by DLR http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.2  2003/02/07 10:43:44  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 15:48:13  dkrajzew
// initial commit for net building classes
//
// Revision 1.4  2002/06/11 16:00:41  dkrajzew
// windows eol removed; template class definition inclusion depends now on the EXTERNAL_TEMPLATE_DEFINITION-definition
//
// Revision 1.3  2002/05/14 04:42:55  dkrajzew
// new computation flow
//
// Revision 1.2  2002/04/26 10:07:11  dkrajzew
// Windows eol removed; minor double to int conversions removed;
//
// Revision 1.1.1.1  2002/04/09 14:18:27  dkrajzew
// new version-free project name (try2)
//
// Revision 1.1.1.1  2002/04/09 13:22:00  dkrajzew
// new version-free project name
//
// Revision 1.2  2002/04/09 12:21:25  dkrajzew
// Windows-Memoryleak detection changed
//
// Revision 1.2  2002/03/22 10:50:03  dkrajzew
// Memory leaks debugging added (MSVC++)
//
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
//
// Revision 1.1  2001/12/06 13:38:01  traffic
// files for the netbuilder
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include "NBLogicKeyBuilder.h"
#include "NBJunctionLogicCont.h"
#include <utils/common/FileHelpers.h>
#include <utils/common/UtilExceptions.h>


/* =========================================================================
 * debugging definitions (MSVC++ only)
 * ======================================================================= */
#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection
   #define _INC_MALLOC	     // exclude standard memory alloc procedures
#endif


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static member definitions
 * ======================================================================= */
NBJunctionLogicCont::LogicMap NBJunctionLogicCont::_map;


/* =========================================================================
 * method definitions
 * ======================================================================= */
bool
NBJunctionLogicCont::exists(const string &key)
{
    return _map.find(key)!=_map.end();
}


int
NBJunctionLogicCont::try2convert(const string &key)
{
    int rot = 0;
    if(exists(string("b.") + key)) return rot;
    string run = key;
    run = NBLogicKeyBuilder::rotateKey(run);
    while(run!=key) {
        rot++;
        if(exists(string("b.") + run)) return rot;
        run = NBLogicKeyBuilder::rotateKey(run);
    }
    return -1;
}


void
NBJunctionLogicCont::add(const std::string &key,
                         const std::string &xmlDescription)
{
    string nkey = string("b.")+key;
//
    LogicMap::iterator i=_map.find(nkey);
    if(i!=_map.end()) {
        string tmp = (*i).second;
        if(tmp!=xmlDescription) {
            cout << key << endl;
            cout << tmp << endl;
            cout << xmlDescription << endl;
            int bla = 0;
        } else {
            cout << "Equal." << endl;
        }
    }
//
    _map.insert(LogicMap::value_type(nkey, xmlDescription));
}


void
NBJunctionLogicCont::writeXML(ostream &into)
{
    for(LogicMap::iterator i=_map.begin(); i!=_map.end(); i++) {
        into << (*i).second << endl;
    }
    into << endl;
}


void
NBJunctionLogicCont::clear() {
    _map.clear();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NBJunctionLogicCont.icc"
//#endif

// Local Variables:
// mode:C++
// End:

