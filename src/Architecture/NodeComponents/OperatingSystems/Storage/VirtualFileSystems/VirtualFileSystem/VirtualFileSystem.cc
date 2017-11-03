#include "VirtualFileSystem.h"

Define_Module(VirtualFileSystem);


VirtualFileSystem::~VirtualFileSystem(){
	
	pathTable.clear();
}


void VirtualFileSystem::initialize(){

	int i;
	std::ostringstream osStream;
	string ior_data;

		// Set the moduleIdName
		osStream << "VirtualFileSystem." << getId();
		moduleIdName = osStream.str();

		// Init the super-class
		icancloud_Base::initialize();

		// Get module parameters	    
		numFS = par ("numFS");
		storageClientIndex = 0;

	    // Get gate Ids
		fromMemoryGate = gate ("fromMemory");
		toMemoryGate = gate ("toMemory");

	    // Init the gate IDs to/from Input gates...
	    fromFSGates = new cGate* [numFS];
	    toFSGates = new cGate* [numFS];

		for (i=0; i<numFS; i++){
			fromFSGates[i] = gate ("fromFS", i);
			toFSGates[i] = gate ("toFS", i);
	    }

}


void VirtualFileSystem::finish(){

	// Finish the super-class
	icancloud_Base::finish();
}


cGate* VirtualFileSystem::getOutGate (cMessage *msg){

	int i;

		// If msg arrive from cache
		if (msg->getArrivalGate()==fromMemoryGate){
			if (toMemoryGate->getNextGate()->isConnected()){
				return (toMemoryGate);
			}
		}

		// If msg arrive from FS's
		else if (msg->arrivedOn("fromFS")){
			for (i=0; i<numFS; i++)
				if (msg->arrivedOn ("fromFS", i))
					return (toFSGates[i]);
		}

	// If gate not found!
	return NULL;
}


void VirtualFileSystem::processSelfMessage (cMessage *msg){
	showErrorMessage ("Unknown self message [%s]", msg->getName());
}


void VirtualFileSystem::processRequestMessage (icancloud_Message *sm){

	string fileName;
	bool found;
	string::size_type searchResult;

	icancloud_App_IO_Message *sm_io;
	vector <pathEntry>::iterator vIterator;
	int operation;

	unsigned int index;
	string moduleType;

	string path;
	pathEntry newEntry;

		// Cast!
		sm_io = check_and_cast <icancloud_App_IO_Message*> (sm);

		// Init
		found = false;
		operation = sm_io->getOperation();

		if (operation == SM_SET_IOR){

			path = sm_io->getFileName();

			// Exists the path?
			 for (vIterator=pathTable.begin();((vIterator!=pathTable.end()) && (!found)); vIterator++){

				searchResult = path.find((*vIterator).path,0);

				 if (searchResult == 0){
					found=true;
				 }
			}

			if (!found){

				newEntry.type = LOCAL_FS_TYPE;
				newEntry.path = path.c_str();
				newEntry.index = 0;
				pathTable.push_back (newEntry);
			}

			sm_io->setIsResponse(true);
			sendResponseMessage (sm_io);

		} else if (operation == SM_DELETE_USER_FS){
            sm_io->setIsResponse(true);
            sendResponseMessage (sm_io);
		}
		else {
			// Request came from FS. Remote operation! Send to memory...
			if (sm_io->getRemoteOperation()){
				sendRequestMessage (sm_io, toMemoryGate);
			}

			// Request came from memory!
			else{

				// Get the file name
				fileName = sm_io->getFileName();

				 // Search the path!
				 for (vIterator=pathTable.begin();((vIterator!=pathTable.end()) && (!found)); vIterator++){

					searchResult = fileName.find((*vIterator).path,0);

					 if (searchResult == 0){
						found=true;
						index = (*vIterator).index;
						moduleType = (*vIterator).type;
					 }
				}

				// Set the Module index, if found!
				if (found){

					// Is a FS destination
					if (moduleType == LOCAL_FS_TYPE){

						if (DEBUG_IO_Rediretor)
							showDebugMessage ("Redirecting request to FS[%u] %s", index, sm_io->contentsToString(DEBUG_MSG_IO_Rediretor).c_str());

						sm_io->setNextModuleIndex (index);
						sm_io->setRemoteOperation (false);
						sendRequestMessage (sm_io, toFSGates[index]);
					}
					 else {
						showErrorMessage ("moduleType unknown in virtual filesystem[%s]", moduleType.c_str());
					}
				}
				// Is a remote Operation? Set Remote operation field and send message to corresponding module
				else{
					if (DEBUG_IO_Rediretor)
						showDebugMessage ("Redirecting request to APP[%u] - ServerID:%d %s", storageClientIndex, index, sm_io->contentsToString(DEBUG_MSG_IO_Rediretor).c_str());

					sm_io->setNextModuleIndex (storageClientIndex);
					sm_io->setConnectionId(index);
					sm_io->setRemoteOperation (true);
					sendRequestMessage (sm_io, toMemoryGate);
				}

			}
		}
}


void VirtualFileSystem::processResponseMessage (icancloud_Message *sm){

	// Send back the message
	sendResponseMessage (sm);
}


string VirtualFileSystem::IORTableToString (){

	std::ostringstream osStream;	
	unsigned int i;

		osStream << "Virtual File System table..." << endl;

		for (i=0; i<pathTable.size(); i++){		

			osStream << "  Entry[" << i << "] Prefix-Path:" << pathTable[i].path.c_str() <<
						"  Index:" << pathTable[i].index <<
						"  Type:" <<  pathTable[i].type.c_str() << endl;
		}

	return osStream.str();
}


