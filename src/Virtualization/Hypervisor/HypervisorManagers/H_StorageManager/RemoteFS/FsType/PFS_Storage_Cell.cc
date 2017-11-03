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

#include "PFS_Storage_Cell.h"

PFS_Storage_Cell::PFS_Storage_Cell(){
	requests_queue.clear();
	active = false;
	strideSize = -1;
	pId = -1;
	netType = "";
	SMS_pfs = NULL;

	connections.clear();
}

PFS_Storage_Cell::PFS_Storage_Cell(int n_strideSizeKB){
	requests_queue.clear();
	active = false;

	pId = -1;	netType = "";

	strideSize = n_strideSizeKB;
    SMS_pfs = NULL;
	connections.clear();

}

PFS_Storage_Cell::~PFS_Storage_Cell() {

	SMS_pfs->clear();
	delete (SMS_pfs);

}

int PFS_Storage_Cell::getServerToSend(icancloud_Message* sm){
	return sm->getConnectionId();
}

icancloud_App_IO_Message* PFS_Storage_Cell::hasMoreMessages(){
	return SMS_pfs->getFirstSubRequest();
}

void PFS_Storage_Cell::initializeCell() {

    SMS_pfs = new SMS_PFS (strideSize, connections.size());
}

bool PFS_Storage_Cell::splitRequest (cMessage *msg){

	icancloud_App_IO_Message* sm_io;
	bool system;

	sm_io = check_and_cast <icancloud_App_IO_Message*> (msg);

	if (sm_io->getNfs_connectionID() == -1){

		system = true;
		SMS_pfs->splitRequest (msg);

	} else {

		system = false;

	}

	return system;

}

icancloud_App_IO_Message* PFS_Storage_Cell::popSubRequest (){
	return SMS_pfs->popSubRequest();
}

icancloud_Message* PFS_Storage_Cell::popNextSubRequest (cMessage* parentRequest){

	//return  SMS_pfs->popNextSubRequest (parentRequest);
    return  NULL;
}

void PFS_Storage_Cell::arrivesSubRequest (cMessage* subRequest, cMessage* parentRequest){
    SMS_pfs->arrivesSubRequest (subRequest, parentRequest);
}

cMessage* PFS_Storage_Cell::arrivesAllSubRequest (cMessage* request){
	bool ok = false;
	icancloud_App_IO_Message* parentRequest_io;
	icancloud_App_IO_Message* sm_io;

	sm_io = check_and_cast <icancloud_App_IO_Message*> (request);

	if (SMS_pfs->arrivesAllSubRequest (sm_io)){

	    ok = true;

	    parentRequest_io = sm_io;
	}

	if (!ok) {
		parentRequest_io = NULL;
	}
	return parentRequest_io;
}

bool PFS_Storage_Cell::removeRequest (cMessage* request){
	SMS_pfs->removeRequest (request);

	bool result = false;

	if (requests_queue.size() == 0){
		result = true;
	}

	return result;
}


