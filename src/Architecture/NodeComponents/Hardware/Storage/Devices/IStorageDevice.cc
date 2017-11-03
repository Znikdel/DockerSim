#include "IStorageDevice.h"

void IStorageDevice::initialize(int stage){

    if (stage == 0){
        // Init the superClass
           HWEnergyInterface::initialize();
    }
    else {

        storageMod = check_and_cast<StorageController*> (getParentModule()->getParentModule()->getSubmodule("eController"));
        storageMod->registerDevice(this, getIndex());

    }

}

void IStorageDevice::e_changeState (string energyState){
    storageMod->e_changeState(energyState, getIndex());
}
