#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    test.py
# @author  Pablo Alvarez Lopez
# @date    2016-11-25
# @version $Id$

# import common functions for netedit tests
import os
import sys

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(
    os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit  # noqa

# Open netedit
neteditProcess, match = netedit.setupAndStart(neteditTestRoot)

# go to additional mode
netedit.additionalMode()

# select containerStop
netedit.changeAdditional("containerStop")

# change reference to center
netedit.modifyAdditionalDefaultValue(8, "reference center")

# create containerStop 1 in mode "reference center"
netedit.leftClick(match, 250, 150)

# create containerStop 2 in mode "reference center"
netedit.leftClick(match, 450, 150)

# go to inspect mode
netedit.inspectMode()

# inspect first containerStop
netedit.leftClick(match, 250, 170)

# Change parameter 0 with a non valid value (Duplicated ID)
netedit.modifyAttribute(0, "containerStop_gneE2_1_1")

# Change parameter 0 with a non valid value (Invalid ID)
netedit.modifyAttribute(0, "Id with spaces")

# Change parameter 0 with a valid value
netedit.modifyAttribute(0, "correctID")

# Change parameter 1 with a non valid value (dummy lane)
netedit.modifyAttribute(1, "dummy lane")

# Change parameter 1 with a valid value (different edge)
netedit.modifyAttribute(1, "gneE0_0")

# Change parameter 1 with a valid value (original edge, same lane)
netedit.modifyAttribute(1, "gneE2_1")

# Change parameter 1 with a valid value (original edge, different lane)
netedit.modifyAttribute(1, "gneE2_0")

# Change parameter 2 with a valid value (< 0)
netedit.modifyAttribute(2, "-5")

# Change parameter 2 with a non valid value (> endPos)
netedit.modifyAttribute(2, "400")

# Change parameter 2 with a valid value
netedit.modifyAttribute(2, "20")

# Change parameter 3 with a valid value (out of range)
netedit.modifyAttribute(3, "3000")

# Change parameter 3 with a non valid value (<startPos)
netedit.modifyAttribute(3, "10")

# Change parameter 3 with a valid value
netedit.modifyAttribute(3, "30")

# Change parameter 4 with a valid value
netedit.modifyAttribute(4, "busStop")

# Change parameter 4 with a different value
netedit.modifyBoolAttribute(5)

# Change parameter 6 with a non valid value (throw warning)
netedit.modifyAttribute(6, "line1, line2")

# Change parameter 6 with a valid value
netedit.modifyAttribute(6, "line1 line2")

# click over an empty area
netedit.leftClick(match, 0, 0)

# Check undos and redos
netedit.undo(match, 14)
netedit.redo(match, 14)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
