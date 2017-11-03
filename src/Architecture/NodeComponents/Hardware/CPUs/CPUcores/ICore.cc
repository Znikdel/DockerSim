#include "ICore.h"

ICore::~ICore(){

}

void ICore::initialize(int stage){

    if (stage == 0){
        // Init the superClass
           HWEnergyInterface::initialize();
    }
    else {
        cModule* mod;
        mod = getParentModule()->getSubmodule ("eController");

        cpuController = check_and_cast<CPUController*> (mod);
        cpuController->registerCore(this, getIndex());
    }

}

void ICore::e_changeState (string energyState){
    cpuController->e_changeState(energyState.c_str(), getIndex());
}

int ICore::e_getStatesSize (){
    return cpuController->e_getStatesSize(getIndex());
}
