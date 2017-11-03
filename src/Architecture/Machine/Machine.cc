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

#include "Machine.h"

Machine::~Machine() {
}

void Machine::initialize(){
  //  if (DEBUG_CLOUD_SCHED) printf("\n Method[Machine]: ------->initialize\n");

    cModule* osMod;

 // create the gates for the new application
    osMod = this->getSubmodule("osModule")->getSubmodule("syscallManager");
    os = dynamic_cast<AbstractSyscallManager*>(osMod);

    type = new elementType();
    type->setDiskSize( par("storageSize_GB").longValue() * 1024 * 1024);
    type->setMemorySize(par("memorySize_MB").longValue() * 1024);

    os->setFreeMemory (type->getMemorySize());
    os->setFreeStorage (type->getStorageSize());
    if (DEBUG_CLOUD_SCHED) cout <<"type->getMemorySize()---->" <<type->getMemorySize() << endl;
    if (DEBUG_CLOUD_SCHED) cout << "type->getStorageSize()"<<type->getStorageSize() << endl;

    type->setNumCores(par("numCores").longValue());
    type->setNumStorageDevices(par("numStorageSystems").longValue());
    type->setType(this->getName());
    if (DEBUG_CLOUD_SCHED) cout << "type->getType()"<<type->getType() << endl;

    if (os == NULL) throw cRuntimeError ("Node can't link its os at initialization.\n");

    icancloud_Base::initialize();
 //   if (DEBUG_CLOUD_SCHED) printf("\n Method[Machine]: ------->initialize--------FIN--------------------------------\n");

}

void Machine::finish(){
    icancloud_Base::finish();
}

void Machine::removeProcess (int pId){
    int numProcesses;

    os->removeProcess(pId);
    numProcesses = os->getNumProcessesRunning();

    if (numProcesses == 0){
        changeState(MACHINE_STATE_IDLE);
    }
}
