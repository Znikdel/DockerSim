#include "StorageController.h"

Define_Module (StorageController);


StorageController::StorageController(){

}


StorageController::~StorageController(){

}


void StorageController::initialize(){

    HWEnergyInterface::initialize();

    numDevices = par("numStorageSystems").longValue();
    complementaryDevices.clear();

    for (int i = 0; i < numDevices; i ++)
        complementaryDevices.push_back(NULL);

    setNumberOfComponents (numDevices);
}


void StorageController::finish(){
    HWEnergyInterface::finish();
}

cGate* StorageController::getOutGate(cMessage *msg){
    return NULL;
}

void StorageController::registerDevice (IStorageDevice* device, int index){

    (*(complementaryDevices.begin()+index)) = device;

}

void StorageController::e_changeState (string state, unsigned componentIndex){

    string actual_state;


      actual_state = (*(complementaryDevices.begin()+componentIndex))->e_getActualState();

          HWEnergyInterface::e_changeState(state,componentIndex);
}


string StorageController::e_getActualState (unsigned componentIndex){
    return HWEnergyInterface::e_getActualState(componentIndex);
}


simtime_t StorageController::e_getStateTime (string state, unsigned componentIndex){
    return HWEnergyInterface::e_getStateTime(state, componentIndex);
}

string StorageController::e_getStateName (int statePosition, unsigned componentIndex){
    return HWEnergyInterface::e_getStateName(statePosition, componentIndex);
}

void StorageController::changeDeviceState (string state, unsigned componentIndex){
    return (*(complementaryDevices.begin()+componentIndex))->changeDeviceState(state);
}


void StorageController::changeState (string energyState, unsigned componentIndex){
    return (*(complementaryDevices.begin()+componentIndex))->changeState(energyState);
}
