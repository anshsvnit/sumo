#ifndef GUIEdgeControlBuilder_h
#define GUIEdgeControlBuilder_h
//---------------------------------------------------------------------------//
//                        GUIEdgeControlBuilder.h -
//  A builder for edges during the loading derived from the
//      NLEdgeControlBuilder
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
// $Log$
// Revision 1.2  2003/02/07 10:38:19  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <vector>
#include <netload/NLEdgeControlBuilder.h>
#include <guisim/GUIEdge.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSJunction;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * This class is reponsible for building GUIEdge-objects
 */
class GUIEdgeControlBuilder : public NLEdgeControlBuilder {
public:
    /** standard constructor; the parameter is a hint for the maximal number
        of lanes inside an edge */
    GUIEdgeControlBuilder(unsigned int storageSize=10);

    /// standard destructor
    ~GUIEdgeControlBuilder();

    /** adds an edge with the given id to the list of edges; this method
        throws an XMLIdAlreadyUsedException when the id was already used for
        another edge */
    void addEdge(const std::string &id);

    /** adds information about the source and the destination edge
        (gui-version only) */
    void addSrcDestInfo(const std::string &id, MSJunction *from,
        MSJunction *to);

    MSLane *addLane(MSNet &net, const std::string &id,
        double maxSpeed, double length, bool isDepart);

private:
    /** invalid copy constructor */
    GUIEdgeControlBuilder(const GUIEdgeControlBuilder &s);

    /** invalid assignment operator */
    GUIEdgeControlBuilder &operator=(const GUIEdgeControlBuilder &s);
};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "GUIEdgeControlBuilder.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

