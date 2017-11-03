#include "CPUController.h"

Define_Module (CPUController);

cGate* CPUController::getOutGate (cMessage *msg){
    return NULL;
}

CPUController::~CPUController(){
}


void CPUController::initialize(){

    HWEnergyInterface::initialize();

    complementaryCores.clear();

    energyIndepentCores = par("indepentCores").boolValue();
    numCPUs = par("numCPUs").longValue();

    for (int i = 0; i < numCPUs; i ++)
       complementaryCores.push_back(NULL);

    setNumberOfComponents (numCPUs);
}


void CPUController::finish(){

}

void CPUController::registerCore (ICore* core, int coreIndex){

   (*(complementaryCores.begin()+coreIndex)) = core;

}


void CPUController::e_changeState (string state, unsigned int componentIndex){

    string actual_state;

    if (energyIndepentCores){
        actual_state = e_getActualState(componentIndex);
//        if (strcmp(actual_state.c_str(), state.c_str()) != 0)
            HWEnergyInterface::e_changeState(state,componentIndex);
    }else {
        unsigned int numcpus = (unsigned int) numCPUs;
        unsigned int i;

        for (i = 0; i < numcpus; i ++){
                HWEnergyInterface::e_changeState(state,componentIndex);
        }
    }
}


string CPUController::e_getActualState (unsigned int componentIndex){
    return HWEnergyInterface::e_getActualState(componentIndex);
}


simtime_t CPUController::e_getStateTime (string state, unsigned int componentIndex){
    return HWEnergyInterface::e_getStateTime(state, componentIndex);
}

string CPUController::e_getStateName (int statePosition, unsigned int componentIndex){
    return HWEnergyInterface::e_getStateName(statePosition, componentIndex);
}

int CPUController::e_getStatesSize(unsigned int componentIndex){
    return HWEnergyInterface::e_getStatesSize(componentIndex);
}

void CPUController::changeDeviceState (string state, unsigned int componentIndex){
    showErrorMessage("CPUController::changeDeviceState does not has to be invoked ...");
}


void CPUController::changeState (string energyState, unsigned int componentIndex){
    showErrorMessage("CPUController::changeState does not has to be invoked ...");
}
