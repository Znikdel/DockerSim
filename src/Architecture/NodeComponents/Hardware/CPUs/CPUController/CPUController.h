//
// This module is the software controller for the different cores at cpu module
//
// @author Gabriel González Castañé
// @date 2013-12-12



// All the CPUs inherit from this class to cast the states from the Node!

#ifndef CPUMODULE_H_
#define CPUMODULE_H_

#include <omnetpp.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <string>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <stdexcept>
#include <math.h>
#include "stdio.h"
#include "stdlib.h"
#include <sstream>
#include <vector>
#include <string>
#include "ICore.h"
#include "HWEnergyInterface.h"

using std::string;
using std::pair;
using std::vector;

class ICore;

class CPUController  :public HWEnergyInterface{

  protected:

    /** If the cores has independant energy mananagement */
    bool energyIndepentCores;

    /** The other cores (cModules) which complete the cpuModule */
    vector<ICore*> complementaryCores;

    // The number of cpu cores
    int numCPUs;

    /**
     * Get the outGate ID to the module that sent <b>msg</b>
     * @param msg Arrived message.
     * @return. Gate Id (out) to module that sent <b>msg</b> or NOT_FOUND if gate not found.
     */
     virtual cGate* getOutGate (cMessage *msg);

    /**
     * Process a self message.
     * @param msg Self message.
     */
     virtual void processSelfMessage (cMessage *msg){};

    /**
     * Process a request message.
     * @param sm Request message.
     */
     virtual void processRequestMessage (icancloud_Message *sm){};

    /**
     * Process a response message.
     * @param sm Request message.
     */
     virtual void processResponseMessage (icancloud_Message *sm){};

    /*
     * Destructor
     */
    virtual ~CPUController();

    /**
     *  Module initialization.
     */
     void initialize();

    /**
     * Module ending.
     */
     void finish();

  public:

     /*
      * Register cores
      */
     void registerCore (ICore* core, int coreIndex);


	/*************************************
	 * Operations from HWManagerInterface
	 *************************************
	 */

    /**
     * Change the actual state to the new given state in the instant actualSimtime
     */
    void e_changeState (string state, unsigned int componentIndex);

    /*
     * Return the actual state of the component. The parameter gives if there are several components
     */
    string e_getActualState (unsigned int componentIndex);

    /*
     * Return the simtime associated to a given as parameter state
     */
    simtime_t e_getStateTime (string state, unsigned int componentIndex);

    /*
     * This method returns the name of the state at position "statePosition" given as parameter
     */
    string e_getStateName (int statePosition, unsigned int componentIndex);

    /*
     * This method returns the quantity of states of component index element
     */
    int e_getStatesSize(unsigned int componentIndex);

    /*
     * This method returns the states vector as string.
     */
    string e_statesToString () {return "";};

    /*
     * Change the energy state of the memory given by node state
     */
    void changeDeviceState (string state, unsigned int componentIndex);

    /*
     * Change the energy state of the disk
     */
    void changeState (string energyState, unsigned int componentIndex);

};

#endif /* CPUMODULEINTERFACE_H_ */
