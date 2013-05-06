/****************************************************************************/
/// @file    MSPhasedTrafficLightLogic.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: MSPhasedTrafficLightLogic.cpp 7711 2010-02-13 17:00:00Z gslager $
///
// The base class for traffic light logic with phases
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <cassert>
#include <utility>
#include <vector>
#include <bitset>
#include <sstream>
#include <microsim/MSEventControl.h>
#include "MSTrafficLightLogic.h"
#include "MSPhasedTrafficLightLogic.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS



// ===========================================================================
// member method definitions
// ===========================================================================
MSPhasedTrafficLightLogic::MSPhasedTrafficLightLogic(MSTLLogicControl &tlcontrol,
        const std::string &id, const std::string &subid, const Phases &phases,
        unsigned int step, SUMOTime delay,
		const ParameterMap& parameters
		) 
        : MSTrafficLightLogic(tlcontrol, id, subid, delay, parameters), myPhases(phases),
        myStep(step) {
    for (size_t i=0; i<myPhases.size(); i++) {
        myDefaultCycleTime += myPhases[i]->duration;
    }
}


MSPhasedTrafficLightLogic::~MSPhasedTrafficLightLogic() {
    // MSPhasedTrafficLightLogic:deletePhases();
    /*for (size_t i=0; i<myPhases.size(); i++) {
        delete myPhases[i];
    }*/
}


// ------------ Switching and setting current rows
/// MEMBER FACTORIZED TO PARENT CLASS (MSTrafficLightLogic)
/*SUMOTime
MSPhasedTrafficLightLogic::trySwitch(bool) {
    // check whether the current duration shall be increased
    if (myCurrentDurationIncrement>0) {
        SUMOTime delay = myCurrentDurationIncrement;
        myCurrentDurationIncrement = 0;
        return delay;
    }

    // increment the index
    myStep++;
    // if the last phase was reached ...
    if (myStep==myPhases.size()) {
        // ... set the index to the first phase
        myStep = 0;
    }
    assert(myPhases.size()>myStep);
    //stores the time the phase started
    myPhases[myStep]->myLastSwitch = MSNet::getInstance()->getCurrentTimeStep();
    // check whether the next duration was overridden
    if (myOverridingTimes.size()>0) {
        SUMOTime nextDuration = myOverridingTimes[0];
        myOverridingTimes.erase(myOverridingTimes.begin());
        return nextDuration;
    }
    // return offset to the next switch
    return myPhases[myStep]->duration;
}
*/



void MSPhasedTrafficLightLogic::proceedToNextStep() {
    setStep(myStep + 1);

}

void MSPhasedTrafficLightLogic::setStep(unsigned int step) {
	assert(myPhases.size() > step);
	if(myStep != step % myPhases.size()) {
		myStep = step % myPhases.size();
    	myPhases[myStep]->myLastSwitch = MSNet::getInstance()->getCurrentTimeStep();
	}
}

// ------------ Static Information Retrieval
unsigned int
MSPhasedTrafficLightLogic::getPhaseNumber() const {
    return (unsigned int) myPhases.size();
}


const MSPhasedTrafficLightLogic::Phases &
MSPhasedTrafficLightLogic::getPhases() const {
    return myPhases;
}

const MSPhaseDefinition &
MSPhasedTrafficLightLogic::getPhase(unsigned int givenStep) const {
    assert(myPhases.size()>givenStep);
    return *myPhases[givenStep];
}


// ------------ Dynamic Information Retrieval
unsigned int
MSPhasedTrafficLightLogic::getCurrentPhaseIndex() const {
    return myStep;
}


const MSPhaseDefinition &
MSPhasedTrafficLightLogic::getCurrentPhaseDef() const {
    return *myPhases[myStep];
}


// ------------ Conversion between time and phase
SUMOTime
MSPhasedTrafficLightLogic::getPhaseIndexAtTime(SUMOTime simStep) const {
    unsigned int position = 0;
    if (myStep > 0)	{
        for (unsigned int i=0; i < myStep; i++) {
            position = position + getPhase(i).duration;
        }
    }
    position = position + simStep - getPhase(myStep).myLastSwitch;
    position = position % myDefaultCycleTime;
    assert(position <= myDefaultCycleTime);
    return position;
}


SUMOTime
MSPhasedTrafficLightLogic::getOffsetFromIndex(unsigned int index) const {
    assert(index < myPhases.size());
    if (index == 0) {
        return 0;
    }
    unsigned int pos = 0;
    for (unsigned int i=0; i < index; i++)	{
        pos += getPhase(i).duration;
    }
    return pos;
}


unsigned int
	MSPhasedTrafficLightLogic::getIndexFromOffset(SUMOTime offset) const {
    assert(offset <= myDefaultCycleTime);
    if (offset == myDefaultCycleTime) {
        return 0;
    }
    unsigned int pos = offset;
    unsigned int testPos = 0;
    for (unsigned int i=0; i < myPhases.size(); i++)	{
        testPos = testPos + getPhase(i).duration;
        if (testPos > pos) {
            return i;
        }
        if (testPos == pos) {
            assert(myPhases.size() > (i+1));
            return (i+1);
        }
    }
    return 0;
}


// ------------ Changing phases and phase durations
void
MSPhasedTrafficLightLogic::changeStepAndDuration(MSTLLogicControl &tlcontrol,
        SUMOTime simStep, unsigned int step, SUMOTime stepDuration) {
    mySwitchCommand->deschedule(this);
	//delete mySwitchCommand;Consider this operation!!!
    mySwitchCommand = new SwitchCommand(tlcontrol, this, stepDuration+simStep);
    myStep = step;
    MSNet::getInstance()->getBeginOfTimestepEvents().addEvent(
        mySwitchCommand, stepDuration+simStep,
        MSEventControl::ADAPT_AFTER_EXECUTION);
}


/****************************************************************************/
void
MSPhasedTrafficLightLogic::setPhases(const Phases& phases, unsigned int step) {
    assert(step < phases.size());
    deletePhases();
    myPhases = phases;
    myStep = step;
}


void
MSPhasedTrafficLightLogic::deletePhases() {
    for (size_t i = 0; i < myPhases.size(); i++) {
        delete myPhases[i];
    }
}

