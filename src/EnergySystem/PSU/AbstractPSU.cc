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

#include "AbstractPSU.h"

AbstractPSU::~AbstractPSU() {

    state = "";
    wattage = 0.0;
}

void AbstractPSU::initialize(){

	state = "";
    wattage = par("wattage").longValue();
    scale = par("scale").longValue();

    alarm = NULL;

    memory = cpu = network = storage = NULL;

    nodeEnergyConsumed = 0.0;
    psuConsumptionLoss = 0.0;

    switchOff();

    alarm = new cMessage ("measure_energy");
    scheduleAt (simTime()+scale, alarm);
}

void AbstractPSU::finish(){
    state = "";
    wattage = 0.0;
}

void AbstractPSU::handleMessage(cMessage *msg){

    double subsystemsConsumption;

        // If msg is a Self Message...
        if (msg->isSelfMessage()){
            if (wattage == 0.0) cancelAndDelete(msg);
            else{
                subsystemsConsumption = getNodeSubsystemsConsumption();
                psuConsumptionLoss += calculateConsumptionLoss(subsystemsConsumption) * scale;
                scheduleAt (simTime()+scale, alarm);
            }

        // Not a self message...
        }else{
            throw cRuntimeError("This module can not receive messages [%s]", msg->getName());
        }


}

void AbstractPSU::resetNodeEnergyConsumed (){

    if (memory != NULL){
       memory->resetConsumptionHistory();
    }

    if (cpu != NULL) {
        cpu->resetConsumptionHistory();
    }

    if (network != NULL) {
        network->resetConsumptionHistory();
    }

    if (storage != NULL) {
        storage->resetConsumptionHistory();
    }
    nodeEnergyConsumed = 0.0;
}

double AbstractPSU::getNodeSubsystemsConsumption(){

    double instantConsumption = 0.0;

    string memorystate = "";
    string networkstate = "";
    vector<string> cpustate;
    vector<string> storagestate;

    cpustate.clear();
    storagestate.clear();

    if (storage != NULL) {
        for (int i = 0; i < storage->getNumberOfComponents();i++)
            storagestate.push_back(storage->getActualState(i));
    }


    if (cpu != NULL) {
        for (int i = 0; i < cpu->getNumberOfComponents();i++)
            cpustate.push_back(cpu->getActualState(i));
    }

    if (memory != NULL){
        memorystate = memory->getActualState();
    }

    if (network != NULL) {
        networkstate = network->getActualState();
    }

    instantConsumption = getInstantNodeConsumption (memorystate, cpustate, networkstate, storagestate);

    return instantConsumption;
}


void AbstractPSU::registerComponent (AbstractMeterUnit* element){

    string element_type;

    // Create the memory consumption calculator
        element_type =  element->getType();

        if (strcmp(element_type.c_str(), "cpu") == 0){
            cpu = element;
        }
        else if (strcmp(element_type.c_str(), "memory") == 0){
            memory = element;
        }
        else if (strcmp(element_type.c_str(), "storage") == 0){
            storage = element;
        }
        else if (strcmp(element_type.c_str(), "network") == 0){
            network = element;
        }
        else {
            throw cRuntimeError("AbstractPSU::registerComponent: cannot register component. Unknown %s component type", element->getType().c_str());
        }
}

void AbstractPSU::switchOn (){

    Enter_Method_Silent();

    if ((scale > 0.0) && (state != PSU_ON))
       state = PSU_ON;

}

void AbstractPSU::switchOff (){
   state = PSU_OFF;
}

double AbstractPSU::getIntervalEnergyConsumed(){

    double energyConsumed = 0.0;

    string memorystate = "";
    string networkstate = "";
    vector<string> cpustate;
    vector<string> storagestate;

    cpustate.clear();
    storagestate.clear();

    if (storage != NULL) {
        for (int i = 0; i < storage->getNumberOfComponents();i++)
            storagestate.push_back(storage->getActualState(i));
    }

    if (cpu != NULL) {
        for (int i = 0; i < cpu->getNumberOfComponents();i++)
            cpustate.push_back(cpu->getActualState(i));
    }

    if (memory != NULL){
        memorystate = memory->getActualState();
    }

    if (network != NULL) {
        networkstate = network->getActualState();
    }

    energyConsumed = getIntervalEnergyCalculus(memorystate, cpustate, networkstate, storagestate);

   return energyConsumed;
}

double AbstractPSU::getIntervalEnergyCalculus(string memoryState, vector<string> cpuState, string networkState, vector<string> diskState){

    double energyConsumed = 0.0;
    double energy;

    if (memory != NULL){
            energy = memory->getEnergyConsumed();
            energyConsumed += energy;
        }

    if (cpu != NULL) {
            energy = cpu->getEnergyConsumed();
            energyConsumed += energy;
    }

    if (network != NULL) {
        energy = network->getEnergyConsumed();
        energyConsumed += energy;
    }

    if (storage != NULL) {
            energy = storage->getEnergyConsumed();
            energyConsumed += energy;
    }

    if (DEBUG_ENERGY){
        std::ostringstream debug;

       debug << getParentModule()->getFullName() << " AbstractPSU getIntervalEnergyCalculus [ m@ " << memoryState.c_str() << " cpu@ ";
        for (int i=0; i <  (int) cpuState.size();i++){
            debug << (*(cpuState.begin() +i)) << " ";
        }

        debug << "net@ " << networkState << " disk@ ";

        for (int i=0; i <  (int) diskState.size();i++){
            debug << (*(diskState.begin() +i)) << " ";
        }

        debug << "] " << energyConsumed;

        printf("%s\n", debug.str().c_str());
    }

    return energyConsumed;
}

double AbstractPSU::getInstantNodeConsumption(string memoryState, vector<string> cpuState, string networkState, vector<string> diskState){

    double instantConsumption = 0.0;
    double consumption;

    if (memory != NULL){
       consumption = memory->getInstantConsumption(memoryState);
       instantConsumption += consumption;
   }

   if (cpu != NULL) {
       for (int i=0; i < (int) cpuState.size();i++){
           consumption = cpu->getInstantConsumption((*(cpuState.begin() + i)),i);
           instantConsumption += consumption;
       }
       if (instantConsumption != 0) instantConsumption += cpu->getConsumptionBase();
   }

   if (network != NULL) {
       consumption = network->getInstantConsumption(networkState);
       instantConsumption += consumption;
   }

   if (storage != NULL) {
       for (int i=0; i < (int) diskState.size();i++){
           consumption = storage->getInstantConsumption((*(diskState.begin() + i)),i);
           instantConsumption += consumption;
       }
   }

   if (DEBUG_ENERGY){
       std::ostringstream debug;

       debug << getParentModule()->getFullName() << " AbstractPSU getInstantNodeConsumption [ m@ " << memoryState.c_str() << " cpu@ ";
       for (int i=0; i < (int) cpuState.size();i++){
           debug << (*(cpuState.begin() +i)) << " ";
       }

       debug << "net@ " << networkState << " disk@ ";

       for (int i=0; i < (int) diskState.size();i++){
           debug << (*(diskState.begin() +i)) << " ";
       }

       debug << "] " << instantConsumption;

       printf("%s\n", debug.str().c_str());

   }
   return instantConsumption;
}
