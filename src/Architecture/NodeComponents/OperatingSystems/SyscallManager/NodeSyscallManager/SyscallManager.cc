#include "SyscallManager.h"

Define_Module (SyscallManager);


SyscallManager::~SyscallManager(){
	
}


void SyscallManager::initialize(){

    // Init the super-class
    AbstractSyscallManager::initialize();

    statesAppPtr = NULL;
    ioManager = NULL;

    cModule* mod;

    mod = getParentModule()->getSubmodule("nodeStates");
    statesAppPtr = dynamic_cast <StatesApplication*> (mod);

    mod = getParentModule()->getSubmodule("remoteStorage");
    ioManager = dynamic_cast <RemoteStorageApp*> (mod);

    // Get module params

        remoteStorageGate = 0;
        statesAppGate = 1;

        for (int i=0; i<2; i++){
            toAppGates->linkGate("toApps", i);
            fromAppGates->linkGate("fromApps",i);
        }
  //   cout << "SyscallManager::initialize()"<<endl;
   //     for (int i=0; i<2; i++){
    //        toDockerEngineGates->linkGate("toDockerEngine", i);
     //       fromDockerEngineGates->linkGate("fromDockerEngine",i);
      //       }
}


void SyscallManager::finish(){

	// Finish the super-class
    AbstractSyscallManager::finish();
}


void SyscallManager::processRequestMessage (icancloud_Message *sm){

	// if the msg comes from the node, the vmID has not setted by the msg controller
	// it will be marked as node, to the hypervisor

	int pid = sm->getPid();
	int uid = sm->getUid();

	int operation;

	// Label the message. It comes from the OS
	if (uid == -1) sm->setUid(0);
	if (pid == -1) sm->setPid(0);

	operation = sm->getOperation();

	// Msg cames from Network
	if (sm->getArrivalGate() == fromNetGate){
		
		if ((operation == SM_VM_ACTIVATION) ||
			(operation == SM_ITERATIVE_PRECOPY) ||
			(operation == SM_STOP_AND_DOWN_VM)){

			sendRequestMessage (sm, toAppGates->getGate(remoteStorageGate));
		//    sendRequestMessage (sm, toDockerEngineGates->getGate(remoteStorageGate));

		}else {

			sendRequestMessage (sm, toAppGates->getGate(sm->getNextModuleIndex()));
        //    sendRequestMessage (sm, toDockerEngineGates->getGate(sm->getNextModuleIndex()));


		}
	}
	
	// Msg cames from CPU
	else if (sm->getArrivalGate() == fromCPUGate){		
		showErrorMessage ("This module cannot receive request messages from CPU!!!");
	}
	
	
	// Msg cames from Memory
	else if (sm->getArrivalGate() == fromMemoryGate){
			
		sendRequestMessage (sm, toAppGates->getGate(sm->getNextModuleIndex()));
   //     sendRequestMessage (sm, toDockerEngineGates->getGate(sm->getNextModuleIndex()));

	}	
	
	// Msg cames from applications
	else{
		
		// I/O operation?
		if ((operation == SM_OPEN_FILE)   	 	 	  ||
			(operation == SM_CLOSE_FILE)  	  		  ||
			(operation == SM_READ_FILE)   			  ||
			(operation == SM_WRITE_FILE)  			  ||
			(operation == SM_CREATE_FILE) 	 		  ||
			(operation == SM_DELETE_FILE) 			  ||
			(operation == SM_CHANGE_DISK_STATE)		  ||
			(operation == SM_SET_HBS_TO_REMOTE)		  ||
			(operation == SM_DELETE_USER_FS)
			){

				// Remote operation? to NET
				if (sm->getRemoteOperation()){
					
					unsigned int aux = remoteStorageGate;
					if (sm->getNextModuleIndex() == aux){
						sendRequestMessage (sm, toAppGates->getGate(remoteStorageGate));
			        //    sendRequestMessage (sm, toDockerEngineGates->getGate(remoteStorageGate));

					}else{
						sendRequestMessage (sm, toNetGate);
					}

				}
				
				// Local operation? to local FS
				else{

					sendRequestMessage (sm, toMemoryGate);

				}
		// Set ior, a new vm is being allocated ..
		} else if(operation == SM_SET_IOR){
			icancloud_App_IO_Message* sm_io;

			sm_io = check_and_cast<icancloud_App_IO_Message*>(sm);

			// Dup and send the request to create the vm into the local net manager
			icancloud_App_NET_Message* sm_net;

			sm_net = new icancloud_App_NET_Message();
			sm_net->setUid(sm->getUid());
			sm_net->setPid(sm->getPid());
			sm_net->setOperation(sm->getOperation());
			sm_net->setLocalIP(sm_io->getNfs_destAddress());

			sendRequestMessage (sm_net, toNetGate);

			// Send request to create the ior in the fs
			sm_io->setNfs_destAddress(NULL);
			sendRequestMessage (sm_io, toMemoryGate);

		}
		
		// MPI operation?
		else if ((operation == MPI_SEND) ||
				 (operation == MPI_RECV) ||
				 (operation == MPI_BARRIER_UP)   ||
				 (operation == MPI_BARRIER_DOWN) ||
				 (operation == MPI_BCAST)   ||
				 (operation == MPI_SCATTER) ||
				 (operation == MPI_GATHER)){
			
			sendRequestMessage (sm, toNetGate);
		}
		
		
		// CPU operation?
		else if ((operation == SM_CPU_EXEC) || (operation == SM_CHANGE_CPU_STATE)) {

			sendRequestMessage (sm, toCPUGate);			
		}

		// MEM operation?
		else if ((operation == SM_MEM_ALLOCATE) || (operation == SM_MEM_RELEASE) ||	 (operation == SM_CHANGE_MEMORY_STATE)){

			sendRequestMessage (sm, toMemoryGate);			
		}


		// Net operation?		
		else if ((operation == SM_CREATE_CONNECTION) || (operation == SM_LISTEN_CONNECTION) ||
				(operation == SM_SEND_DATA_NET) || (operation == SM_CHANGE_NET_STATE)){

			sendRequestMessage (sm, toNetGate);			
		}			
		
		// Remote Storage or migration operation?
		else if (
				(operation == SM_VM_REQUEST_CONNECTION_TO_STORAGE) || (operation == SM_NODE_REQUEST_CONNECTION_TO_MIGRATE) ||
				(operation == SM_MIGRATION_REQUEST_LISTEN) || (operation == SM_UNBLOCK_HBS_TO_REMOTE) ||
				(operation == SM_CLOSE_CONNECTION) || (operation == SM_CLOSE_VM_CONNECTIONS) ||
				(operation == SM_VM_ACTIVATION) || (operation == SM_ITERATIVE_PRECOPY) ||
				(operation == SM_STOP_AND_DOWN_VM) || (operation == SM_CONNECTION_CONTENTS)
				){

			sendRequestMessage (sm, toNetGate);
		}

		// Set hypervisor contents (vm migration)
		else if (operation == SET_MIGRATION_CONNECTIONS){

			icancloud_Migration_Message *sm_hmem_data;
			icancloud_Migration_Message *sm_hbs_data;

		    sm_hbs_data = dynamic_cast<icancloud_Migration_Message *> (sm);

			if (sm_hbs_data->getMemorySizeKB() != 0){
				sm_hmem_data = sm_hbs_data->dup();
				sm_hmem_data->setOperation(ALLOCATE_MIGRATION_DATA);
				sendRequestMessage (sm_hmem_data, toMemoryGate);
			}

			// In the net manager redirect to the bs manager.
			sendRequestMessage (sm_hbs_data, toNetGate);

		}

		// Set memory and disk data (vm migration)
		else if (operation == ALLOCATE_MIGRATION_DATA){

			icancloud_Migration_Message *sm_hbs_data;
			sm_hbs_data = dynamic_cast<icancloud_Migration_Message *> (sm);

			if (sm_hbs_data->getMemorySizeKB() != 0){
				sendRequestMessage (sm, toMemoryGate);
			}

			if (sm_hbs_data->getDiskSizeKB() != 0){
				sendRequestMessage (sm_hbs_data, toNetGate);
			}
		}

		// Get memory and disk/connections data (vm migration)
		else if ((operation == GET_MIGRATION_DATA) || (operation == GET_MIGRATION_CONNECTIONS)){
			icancloud_Migration_Message *sm_hmem_data;
			icancloud_Migration_Message *sm_hbs_data;
			sm_hbs_data = dynamic_cast<icancloud_Migration_Message *> (sm);

			sm_hmem_data = sm_hbs_data->dup();

			sendRequestMessage (sm_hmem_data, toMemoryGate);
			sendRequestMessage (sm_hbs_data, toNetGate);

		}
		else if((operation == SM_NOTIFY_USER_FS_DELETED)||
		        (operation == SM_NOTIFY_PRELOAD_FINALIZATION) ||
		        (operation == SM_NOTIFY_USER_CONNECTIONS_CLOSED)
		        ){
		    notifyManager(sm);
		    delete(sm);
		}
		// Unknown operation! -> Error!!!
		else
			showErrorMessage ("Unknown operation:%i %s",sm->getOperation(), sm->operationToString().c_str());
	}

}

void SyscallManager::notifyManager (icancloud_Message* sms){

    nodePtr->notifyManager(sms);

}

void SyscallManager::removeProcess(int pid){

    icancloud_Base* job = deleteJobFromStructures(pid);

    if (job != NULL){
       int gateIdx = job->gate("fromOS")->getPreviousGate()->getId();
        cout<<"SyscallManager::removeProcess--->gate(fromOS)->getPreviousGate()--->"<<job->gate("fromOS")->getPreviousGate()->getFullName()<<endl;
        cout<<"SyscallManager::removeProcess--->gate index"<<gateIdx<<endl;
   //     int cgateIdx = job->gate("fromOSfromCon")->getPreviousGate()->getId();

       // int position = toAppGates->searchGate(gateIdx);

        int position = toDockerEngineGates->searchGate(gateIdx);

    //    fromAppGates->freeGate(position);
     //   toAppGates->freeGate(position);

        fromDockerEngineGates->freeGate(position);
        toDockerEngineGates->freeGate(position);
        job->callFinish();

    }

}

int SyscallManager::createProcess(icancloud_Base* job, int uid){

    if (job == NULL) throw cRuntimeError("SyscallManager::createJob, error with dynamic casting. Entry parameter cannot cast to jobBase.\n");

 //   int newIndexFrom = fromAppGates->newGate("fromApps");
 //   int newIndexTo = toAppGates->newGate("toApps");
cout <<" SyscallManager::createProcess"<<endl;
    int newIndexFrom = fromDockerEngineGates->newGate("fromDockerEngine");
    int newIndexTo = toDockerEngineGates->newGate("toDockerEngine");
    //get the app previously created
    job->changeParentTo(this);

    //Connect the modules (app created and node selected)
       //fromAppGates->connectIn(job->gate("fromOS"), newIndexFrom);
       //toAppGates->connectOut(job->gate("toOS"), newIndexTo);


    fromDockerEngineGates->connectIn(job->gate("fromOS"), newIndexFrom);
    toDockerEngineGates->connectOut(job->gate("toOS"), newIndexTo);

//    fromDockerEngineGates->connectIn(job->gate("fromDockerEngine"), newIndexFrom);// Second one is for docker
 //    toDockerEngineGates->connectOut(job->gate("toDockerEngine"), newIndexTo);

        processRunning* proc;
        proc = new processRunning();
        proc->process = job;
        proc->uid = uid;
        processesRunning.push_back(proc);

      return newIndexTo;

 //   return cnewIndexTo;

}

void SyscallManager::initializeSystemApps(int storagePort, string state){
  //  if (DEBUG_CLOUD_SCHED) printf("\n Method[SyscallManager]: ------->initializeSystemApps\n");
  //  if (DEBUG_CLOUD_SCHED) printf("\n Method[SyscallManager]: ------->initializeSystemApps------------storagePort----%i\n",storagePort);
    if (DEBUG_CLOUD_SCHED) printf("\n Method[SyscallManager]: ------->initializeSystemApps------------state----%s\n",state);

    statesAppPtr->initState(state);
    // Initialize the local port if this is a storage node..
    if (storagePort != -1) ioManager -> initialize_storage_data (storagePort);
 //   if (DEBUG_CLOUD_SCHED) printf("\n Method[SyscallManager]: ------->initializeSystemApps-------FIN--------------\n");

}

void SyscallManager::changeState(string newState){
    if ( strcmp (MACHINE_STATE_OFF, newState.c_str()) == 0){
        resetSystem();
    }
    statesAppPtr->changeState(newState);
}

