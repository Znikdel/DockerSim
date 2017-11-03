//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef ABSTRACTMETERUNIT_H
#define ABSTRACTMETERUNIT_H

#include <omnetpp.h>

#include "HWEnergyInterface.h"
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <map>
using std::string;
using std::pair;
using std::vector;
#include "AbstractPSU.h"
#include "Memoization_uthash.h"

#define NULL_STATE "null_state"

class AbstractPSU;

class AbstractMeterUnit : public cSimpleModule{

protected:

    string type;                                    // The type of component to measure the energy
    string componentName;                           // The name of the component (model)
    double consumptionBase;                         // To elements like the CPU, that has a base and peaks for each core
    simtime_t lastEnergyRead;                       // It is needed to know last time when energy has been read.

    HWEnergyInterface* e_internal;                  // The abstract energy layer for all hardware components

    vector <EnergyState*> states;                   // The set of states

    // for memorization option..
    bool memorization;

    int numEnergyStates;

    MemoSupport* memo;

protected:

    /*
     * Destructor
     */
	virtual ~AbstractMeterUnit();

	/*
	*  Module initialization.
	*/
	virtual void initialize();

    void handleMessage(cMessage *msg){};

public:

    /**
     * Module ending.
     */
    virtual void finish();

     /*
      * This method load the memo support
      */
    void loadMemo(MemoSupport* memoFile){memo = memoFile;};

    // This method reset the amount of time accumulated in the energy states vector
    void resetConsumptionHistory();

	// This method returns the value of the instant power consumed by component
    virtual double getInstantConsumption(string state = NULL_STATE, int partIndex = -1) = 0;

    // This method returns the quantity of jules that the component has consumed
	virtual double getEnergyConsumed(int partIndex = -1);

	// This method is invoked by concrete energy meters when the component has several components and common energy.
	double postEnergyConsumedCalculus(int partIndex, vector<double> consumption);

    // This method returns reset the accumulated energy that component has consumed
    void resetAccumulatedEnergy(int componentIndex = -1);

    // This method returns returns the quantity of jules that the component has consumed and reset those values
    double getEnergyAccumulated_and_reset(int componentIndex = -1);


	string getType(){return type.c_str();};

	// Activate memorization
	void activeMemorization (bool active){memorization = active;};

    // Activate meter
    void activateMeter(unsigned componentIndex = 0, bool heterogeneousComponents = true);

    // This method returns the actual state:
    string getActualState(int partIndex = -1);

    // This method returns the number of components that consists of the entire component
    int getNumberOfComponents(){return e_internal->getNumberOfComponents();};

    // This method returns the consumption base.
    double getConsumptionBase(){return e_internal->getConsumptionBase();};

    void printStateVector(string operation, int size,float * statesVectors, float energy);

    string getComponentName(){return componentName;};
};

#endif
