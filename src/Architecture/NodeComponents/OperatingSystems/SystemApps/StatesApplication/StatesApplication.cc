#include "StatesApplication.h"

Define_Module(StatesApplication);


StatesApplication::~StatesApplication(){
}


void StatesApplication::initialize(){

    icancloud_Base::initialize();

	std::ostringstream osStream;
    timeoutEvent = NULL;
    timeout = 1.0;


	sendToNetwork = false;
	sendToDisk = false;
	sendToMemory = false;
	sendToCpu = false;

	// Set the moduleIdName
	osStream << "DummyApplication." << getId();
	moduleIdName = osStream.str();

	// Init the super-class
	fromOSGate = gate ("fromOS");
	toOSGate = gate ("toOS");
//	fromOSGate = new cGateManager(this);
//	toOSGate =new cGateManager(this);
	//fromOSfromConGate = gate ("fromOSfromCon");
    //toOStoConGate = gate ("toOStoCon");

	// Load the parameters of the node;
	numCPUs = getParentModule()->getParentModule()->par("numCores");
	numBS = getParentModule()->getParentModule()->par("numStorageSystems");
	numNetworkDevices = getParentModule()->getParentModule()->par("numNetworkInterfaces");

	actualState = MACHINE_STATE_OFF;
}


void StatesApplication::finish(){

	// Finish the super-class
    icancloud_Base::finish();


}

cGate* StatesApplication::getOutGate (cMessage *msg){

    // If msg arrive from IOR
    if (msg->getArrivalGate()==fromOSGate){
        if (toOSGate->getNextGate()->isConnected()){
            return (toOSGate);
        }
    }

    // If gate not found!
    return NULL;
}

void StatesApplication::processSelfMessage (cMessage *msg){

    showErrorMessage ("This module does not receive auto messages!");

}


void StatesApplication::processRequestMessage (icancloud_Message *sm){
	showErrorMessage ("This module does not receive request messages!");
}


void StatesApplication::processResponseMessage (icancloud_Message *sm){
	showErrorMessage ("This module does not receive request messages!");
}

void StatesApplication::initState (string newState){
    if (DEBUG_CLOUD_SCHED) printf("\n Method[StatesApplication]: ------->initState\n");

    actualState = newState;        // A null state
    sendToNetwork = sendToDisk = sendToMemory = sendToCpu = true;
    send_msg_to_change_states(sendToCpu,sendToMemory,sendToNetwork,sendToDisk);
    if (DEBUG_CLOUD_SCHED) printf("\n Method[StatesApplication]: ------->initState----FIN---------------\n");

}


void StatesApplication::changeState(string newState){

    if (strcmp (newState.c_str(), actualState.c_str()) != 0){
    // From state to IDLE state
        if ( strcmp (MACHINE_STATE_IDLE, actualState.c_str()) == 0){

            if ( strcmp (MACHINE_STATE_IDLE, newState.c_str()) == 0){

                sendToNetwork = sendToDisk = sendToMemory = sendToCpu = false;

            } else if ( strcmp (MACHINE_STATE_RUNNING, newState.c_str()) == 0){

                sendToNetwork = sendToDisk = sendToMemory = sendToCpu = true;

            } else if ( strcmp (MACHINE_STATE_OFF, newState.c_str()) == 0){

                sendToNetwork = sendToDisk = sendToMemory = sendToCpu = true;

            }

        // From state to RUNNING state
        } else if ( strcmp (MACHINE_STATE_RUNNING, actualState.c_str()) == 0){

            if ( strcmp (MACHINE_STATE_IDLE, newState.c_str()) == 0){

                sendToNetwork = sendToDisk = sendToMemory = sendToCpu = true;

            } else if ( strcmp (MACHINE_STATE_RUNNING, newState.c_str()) == 0){

                sendToNetwork = sendToDisk = sendToMemory = sendToCpu = false;

            } else if ( strcmp (MACHINE_STATE_OFF, newState.c_str()) == 0){

                sendToNetwork = sendToDisk = sendToMemory = sendToCpu = true;

            }

        // From state to OFF state
        } else if ( strcmp (MACHINE_STATE_OFF, actualState.c_str()) == 0){

            if ( strcmp (MACHINE_STATE_IDLE, newState.c_str()) == 0){

                sendToNetwork = sendToDisk = sendToMemory = sendToCpu = true;

            } else if ( strcmp (MACHINE_STATE_RUNNING, newState.c_str()) == 0){

                sendToNetwork = sendToDisk = sendToMemory = sendToCpu = true;

            } else if ( strcmp (MACHINE_STATE_OFF, newState.c_str()) == 0){

                sendToNetwork = sendToDisk = sendToMemory = sendToCpu = false;

            }

        // From state to other unknown state
        } else {

            if ( strcmp (MACHINE_STATE_IDLE, newState.c_str()) == 0){

                sendToNetwork = sendToDisk = sendToMemory = sendToCpu = false;

            } else if ( strcmp (MACHINE_STATE_RUNNING, newState.c_str()) == 0){

                sendToNetwork = sendToDisk = sendToMemory = sendToCpu = false;

            } else if ( strcmp (MACHINE_STATE_OFF, newState.c_str()) == 0){

                sendToNetwork = sendToDisk = sendToMemory = sendToCpu = false;

            }

        }

        actualState = newState;

        send_msg_to_change_states(sendToCpu,sendToMemory,sendToNetwork,sendToDisk);
    }
}


void StatesApplication::send_msg_to_change_states (bool cpu, bool memory, bool network, bool disk){
    if (DEBUG_CLOUD_SCHED) printf("\n Method[StatesApplication]: ------->send_msg_to_change_states\n");

	vector<int> devicesIndexToChange;
	int i;

	devicesIndexToChange.clear();
	if (sendToNetwork){
	    for (i = 0; i < numNetworkDevices; i++)
						devicesIndexToChange.insert(devicesIndexToChange.end(), i);
		sendToNetwork = false;
		icancloud_request_changeState_network (actualState, devicesIndexToChange);
	    if (DEBUG_CLOUD_SCHED) printf("\n Method[StatesApplication]: ------->icancloud_request_changeState_network\n");

	}

	devicesIndexToChange.clear();
	if (sendToDisk){
		for (i = 0; i < numBS; i++)
					devicesIndexToChange.insert(devicesIndexToChange.end(), i);

		sendToDisk = false;
		icancloud_request_changeState_IO (actualState, devicesIndexToChange);
	    if (DEBUG_CLOUD_SCHED) printf("\n Method[StatesApplication]: ------->icancloud_request_changeState_IO\n");

	}

	devicesIndexToChange.clear();
	if (sendToMemory){
		sendToMemory = false;
		icancloud_request_changeState_memory (actualState);
	    if (DEBUG_CLOUD_SCHED) printf("\n Method[StatesApplication]: ------->icancloud_request_changeState_memory\n");

	}

	devicesIndexToChange.clear();
	if (sendToCpu){
		for (i = 0; i < numCPUs; i++)
			devicesIndexToChange.insert(devicesIndexToChange.end(), i);

		sendToCpu = false;
		icancloud_request_changeState_cpu (actualState, devicesIndexToChange);
	//    if (DEBUG_CLOUD_SCHED) printf("\n Method[StatesApplication]: ------->icancloud_request_changeState_cpu\n");

	}
    if (DEBUG_CLOUD_SCHED) printf("\n Method[StatesApplication]: ------->send_msg_to_change_states----FIN----------\n");


}

void StatesApplication::icancloud_request_changeState_IO (string newState, vector<int> devicesIndexToChange){

    Enter_Method_Silent();

    icancloud_App_IO_Message *sm_io;        // Request message!
    unsigned int i;
    vector <int>::iterator componentIndex;

        // Creates the message
        sm_io = new icancloud_App_IO_Message ();
        sm_io->setOperation (SM_CHANGE_DISK_STATE);

        // Set the corresponding parameters
        sm_io->setChangingState(newState);

        for (i = 0; i<devicesIndexToChange.size(); i++){
            componentIndex = devicesIndexToChange.begin() + i;
            sm_io->add_component_index_To_change_state((*componentIndex));
        }
        // Update message length
        //sm_io->updateLength ();

        if (DEBUG_AppSystem)
            showDebugMessage ("[AppSystemRequests] Changing disk state to %s\n", newState.c_str());

        // Send the request to the Operating System
        sendRequestMessage (sm_io, toOSGate);
}


void StatesApplication::icancloud_request_changeState_memory (string newState){

    Enter_Method_Silent();

    icancloud_App_MEM_Message *sm_mem;      // Request message!


        // Creates the message
        sm_mem = new icancloud_App_MEM_Message ();
        sm_mem->setOperation (SM_CHANGE_MEMORY_STATE);

        // Set the corresponding parameters
        sm_mem->setChangingState(newState);

        // Update message length
        sm_mem->updateLength ();
        //CloudManagerBase* cloudManagerPtr;
        if (DEBUG_AppSystem)
            showDebugMessage ("[AppSystemRequests] Changing memory state to %s\n", newState.c_str());

        // Send the request to the Operating System
        sendRequestMessage (sm_mem, toOSGate);
}

void StatesApplication::icancloud_request_changeState_cpu (string newState, vector<int> devicesIndexToChange){

    Enter_Method_Silent();

    icancloud_App_CPU_Message *sm_cpu;      // Request message!
    unsigned int i;
    vector <int>::iterator componentIndex;

        // Creates the message
        sm_cpu = new icancloud_App_CPU_Message ();
        sm_cpu->setOperation (SM_CHANGE_CPU_STATE);

        // Set the corresponding parameters
        sm_cpu->setChangingState(newState);

        for (i = 0; i<devicesIndexToChange.size(); i++){
            componentIndex = devicesIndexToChange.begin() + i;
            sm_cpu->add_component_index_To_change_state((*componentIndex));
        }
        // Update message length
        sm_cpu->updateLength ();

        //CloudManagerBase* cloudManagerPtr;
        if (DEBUG_AppSystem)
            showDebugMessage ("[AppSystemRequests] Changing CPU state to %s\n", newState.c_str());

        // Send the request to the Operating System
        sendRequestMessage (sm_cpu, toOSGate);
}

void StatesApplication::icancloud_request_changeState_network (string newState, vector<int> devicesIndexToChange){

    Enter_Method_Silent();
    if (DEBUG_CLOUD_SCHED) printf("\n Method[StatesApplication]: ------->icancloud_request_changeState_network\n");

    icancloud_App_NET_Message *sm_net;                  // Request message!
    unsigned int i;
    vector <int>::iterator componentIndex;

    // Creates the message
    sm_net = new icancloud_App_NET_Message ();
    sm_net->setOperation (SM_CHANGE_NET_STATE);

    // Set the corresponding parameters
    sm_net->setChangingState(newState);

    for (i = 0; i<devicesIndexToChange.size(); i++){
        componentIndex = devicesIndexToChange.begin() + i;
        sm_net->add_component_index_To_change_state((*componentIndex));
    }

    if (DEBUG_AppSystem)
                showDebugMessage ("[AppSystemRequests] Changing network state to %s\n", newState.c_str());


    // Send the request message to Network Service module
    sendRequestMessage (sm_net, toOSGate);
    if (DEBUG_CLOUD_SCHED) printf("\n Method[StatesApplication]: ------->icancloud_request_changeState_network---FIN----\n");

}

