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

#include "H_STORAGE_SCHED_FIFO.h"

Define_Module(H_STORAGE_SCHED_FIFO);

H_STORAGE_SCHED_FIFO::~H_STORAGE_SCHED_FIFO() {

}

void H_STORAGE_SCHED_FIFO::initialize(){

    H_StorageManager_Base::initialize();

}

void H_STORAGE_SCHED_FIFO::finish(){
    H_StorageManager_Base::finish();

}

void H_STORAGE_SCHED_FIFO::schedulingStorage(icancloud_Message *msg){

    icancloud_App_IO_Message* sm;

    int operation = msg->getOperation();

    if (msg->arrivedOn("fromNET_Manager")){

        sm =  check_and_cast <icancloud_App_IO_Message*>  (msg);

        if (operation == SM_DELETE_USER_FS) {

            sendRequestMessage(sm, toNodeStorageServers[0]);
        }
        else if (sm->getRemoteOperation()){

            int gate = getCellGate(sm->getUid(), sm->getPid());

            if (sm->getRemoteOperation()){
                // The request is a remote operation to be resolved in the target node!
                sendRequestMessage(sm,to_storageCell->getGate(gate));
            } else {
                // The request is a operation to be resolved in this node!
                showErrorMessage("An IO message arrives from H_NET_MANAGER to H_STORAGE_MANAGER and it is not a remote operation.\nProbably in the node target?");
            }

       } else {

               // The request is a operation to be resolved in this node!
               showErrorMessage("An IO message arrives from H_NET_MANAGER to H_STORAGE_MANAGER and it is not a remote operation.\nProbably in the node target?");

       }
    }
    else if (msg->arrivedOn("fromVMStorageServers")){

        sm =  check_and_cast <icancloud_App_IO_Message*>  (msg);

        long int ok;
        if (msg->getOperation() == SM_WRITE_FILE){
            ok = updateOperation(sm->getUid(), sm->getPid(), sm->getSize()/1024, true);
            if (ok < 0) showDebugMessage("H_STORAGE_SCHED_FIFO::schedulingStorage->user has not got enough disk space..\n");
        }
        else if (msg->getOperation() == SM_DELETE_FILE){
            ok = updateOperation(sm->getUid(), sm->getPid(), sm->getSize()/1024, false);
        }

        sendRequestMessage(msg, toNodeStorageServers[0]);
    }
    else if (msg->arrivedOn("from_storageCell"))
        sendRequestMessage(msg, toNET_Manager);
    else
        throw cRuntimeError ("Error in H_StorageManager::processRequestMessage manager. Unknown gate\n");

}





