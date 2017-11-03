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

#include <userBase.h>

void userBase::abandonSystem(){
cout<<"userBase::abandonSystem()"<<endl;
    RequestBase* request;
    request = new RequestBase();
    request->setOperation(REQUEST_ABANDON_SYSTEM);
    request->setUid(this->getId());

    managerPtr->userSendRequest(request);
}

void userBase::container_abandonSystem(){
    cout<<"userBase::container_abandonSystem()"<<endl;

    RequestBase* request;
    request = new RequestBase();
    request->setOperation(CONTAINER_REQUEST_ABANDON_SYSTEM);
    request->setUid(this->getId());

    managerPtr->userSendRequest(request);
}

userBase::~userBase() {
    configPreload.clear();
    configFS.clear();
}

void userBase::initialize(){

    queuesManager::initialize();
    pending_requests.clear();
    fsType = "LOCAL";
    configMPI = NULL;
    configPreload.clear();
    configFS.clear();
    waiting_for_system_response = new JobQueue();
    //Zahra Nikdel
    container_waiting_for_system_response = new Container_JobQueue();

    userID = this->getId();

    //Set up the manager
    cModule* managerMod;

       managerMod = this->getParentModule()->getSubmodule("manager");
       managerPtr = dynamic_cast<RequestsManagement*> (managerMod);

       if (managerPtr == NULL) throw cRuntimeError("userBase::initialize()->can not cast the manager to RequestManagement class\n");
}

void userBase::finish(){

    queuesManager::finish();
    fsType = "LOCAL";
    configMPI = NULL;
    configPreload.clear();
    configFS.clear();
    waiting_for_system_response->clear();
    container_waiting_for_system_response->clear();
}

void userBase::sendRequestToSystemManager (AbstractRequest* request){

         managerPtr->userSendRequest(request);

     // insert to wait until a response will be given
         pending_requests.push_back(request);

     // Move the job until the request will be atendeed..
         if (request->getOperation() == REQUEST_RESOURCES)
             waitingQueue->move_to_qDst(waitingQueue->get_index_of_job(request->getUid()), waiting_for_system_response, waiting_for_system_response->get_queue_size());
     // Move the job until the request will be atendeed..
         if (request->getOperation() == CONTAINER_REQUEST_RESOURCES)
             containerWaitingQueue->move_to_qDst(containerWaitingQueue->get_index_of_job(request->getUid()), container_waiting_for_system_response, container_waiting_for_system_response->get_queue_size());

}

void userBase::requestArrival(AbstractRequest* request){

    int i;
    AbstractRequest* req;
    bool found;

    found = false;

    for (i = 0; (i < (int)pending_requests.size()) && (!found); i++){

        req = (*(pending_requests.begin() + i));

        if (request->compareReq(req)){
            pending_requests.erase((pending_requests.begin() + i));
            found = true;
        }
    }

}

void userBase::requestArrivalError(AbstractRequest* request){

    int i;
    AbstractRequest* req;
    bool found;

    found = false;

    waiting_for_system_response->move_to_qDst(waiting_for_system_response->get_index_of_job(request->getUid()), waitingQueue, waitingQueue->get_queue_size());
    container_waiting_for_system_response->move_to_qDst(container_waiting_for_system_response->get_index_of_job(request->getUid()), containerWaitingQueue, containerWaitingQueue->get_queue_size());

    for (i = 0; (i < (int)pending_requests.size()) && (!found); i++){

        req = (*(pending_requests.begin() + i));

        if (request->compareReq(req)){
            pending_requests.erase((pending_requests.begin() + i));
            found = true;
        }
    }



}
