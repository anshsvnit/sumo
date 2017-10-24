/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2017 German Aerospace Center (DLR) and others.
/****************************************************************************/
//
//   This program and the accompanying materials
//   are made available under the terms of the Eclipse Public License v2.0
//   which accompanies this distribution, and is available at
//   http://www.eclipse.org/legal/epl-v20.html
//
/****************************************************************************/
/// @file    GNEChange_Attribute.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 1017
/// @version $Id$
///
// A change in the values of Calibrators in netedit
/****************************************************************************/
#ifndef GNEChange_CalibratorItem_h
#define GNEChange_CalibratorItem_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include <utils/foxtools/fxexdefs.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include "GNEChange.h"
#include "GNECalibratorRoute.h"
#include "GNECalibratorVehicleType.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNECalibrator;
class GNECalibratorFlow;
class GNECalibratorRoute;
class GNECalibratorVehicleType;

// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class GNEChange_CalibratorItem
* A change to the network selection
*/
class GNEChange_CalibratorItem : public GNEChange {
    FXDECLARE_ABSTRACT(GNEChange_CalibratorItem)

public:
    /**@brief Constructor for modifying selection
     * @param[in] newCalibratorRoutes vector with the new calibratorRoutes of calibrator
     */
    GNEChange_CalibratorItem(GNECalibrator *calibrator);

    /// @brief Destructor
    ~GNEChange_CalibratorItem();

    /// @name inherited from GNEChange
    /// @{
    /// @brief get undo Name
    FXString undoName() const;

    /// @brief get Redo name
    FXString redoName() const;

    /// @brief undo action
    void undo();

    /// @brief redo action
    void redo();
    /// @}


private:
    /// @brief calibrator in which apply changes
    GNECalibrator *myCalibrator;
};

#endif
/****************************************************************************/
