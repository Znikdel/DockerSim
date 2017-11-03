#include "CPU_Scheduler_RR.h"

Define_Module (CPU_Scheduler_RR);



CPU_Scheduler_RR::~CPU_Scheduler_RR(){
		
	requestsQueue.clear();			
}


void CPU_Scheduler_RR::initialize(){

	std::ostringstream osStream;
	unsigned int i;


	    // Set the moduleIdName
		osStream << "CPU_Scheduler_RR." << getId();
		moduleIdName = osStream.str();

	    // Init the super-class
	    icancloud_Base::initialize();	
	    
	    // Get module parameters
	    numCPUs = par ("numCPUs");
	    quantum = par ("quantum");
	    
	    
	    // State of CPUs
		isCPU_Idle = new bool [numCPUs];
		
		// Init state to idle!
		for (i=0; i<numCPUs; i++)
			isCPU_Idle[i] = true;	    
	    	    		
		// Init requests queue		
		requestsQueue.clear();	
			
	    
		    // Init the gate IDs to/from Scheduler
		    fromOsGate = gate ("fromOsGate");
		    toOsGate = gate ("toOsGate");
	
		    // Init the gates IDs to/from BlockServers
		    toCPUGate = new cGate* [numCPUs];
		    fromCPUGate = new cGate* [numCPUs];
	
		    for (i=0; i<numCPUs; i++){
				toCPUGate [i] = gate ("toCPU", i);
				fromCPUGate [i] = gate ("fromCPU", i);
		    }    
}


void CPU_Scheduler_RR::finish(){

	// Finish the super-class
	icancloud_Base::finish();
}


cGate* CPU_Scheduler_RR::getOutGate (cMessage *msg){

	unsigned int i;

		// If msg arrive from Output
		if (msg->getArrivalGate()==fromOsGate){
			if (gate("toOsGate")->getNextGate()->isConnected()){
				return (toOsGate);
			}
		}

		// If msg arrive from Inputs
		else if (msg->arrivedOn("fromCPU")){
			for (i=0; i<numCPUs; i++)
				if (msg->arrivedOn ("fromCPU", i))
					return (gate("toCPU", i));
		}

	// If gate not found!
	return NULL;
}


void CPU_Scheduler_RR::processSelfMessage (cMessage *msg){
	showErrorMessage ("Unknown self message [%s]", msg->getName());
}


void CPU_Scheduler_RR::processRequestMessage (icancloud_Message *sm){	

	int cpuIndex;
	icancloud_App_CPU_Message *sm_cpu;
	

		// Casting to debug!
		sm_cpu = check_and_cast<icancloud_App_CPU_Message *>(sm);
		
		// Assign infinite quantum
		sm_cpu->setQuantum(quantum);
				
		// Search for an empty cpu core
		cpuIndex = searchIdleCPU();
		
		// All CPUs are busy
		if (cpuIndex == NOT_FOUND){
			
			if (DEBUG_CPU_Scheduler_RR)
				showDebugMessage ("Enqueing computing block. All CPUs are busy: %s", sm_cpu->contentsToString(DEBUG_MSG_CPU_Scheduler_RR).c_str());
			
			// Enqueue current computing block
			requestsQueue.insert (sm_cpu);
		}
		
		// At least, one cpu core is idle
		else{
			
			if (DEBUG_CPU_Scheduler_RR)
				showDebugMessage ("Sending computing block to CPU[%d]:%s", cpuIndex, sm_cpu->contentsToString(DEBUG_MSG_CPU_Scheduler_RR).c_str());
			
			// Assign cpu core
			sm_cpu->setNextModuleIndex(cpuIndex);			
			
			// Update state!
			isCPU_Idle[cpuIndex]=false;		
			sendRequestMessage (sm_cpu, toCPUGate[cpuIndex]);
		}
}


void CPU_Scheduler_RR::processResponseMessage (icancloud_Message *sm){

	unsigned int cpuIndex;	
	cMessage* unqueuedMessage;
	icancloud_Message *nextRequest;
	icancloud_App_CPU_Message *sm_cpu;
	icancloud_App_CPU_Message *sm_cpuNext;
	
		// Cast
		sm_cpu = check_and_cast<icancloud_App_CPU_Message *>(sm);

		// Update cpu state!
		cpuIndex = sm_cpu->getNextModuleIndex();

		if ((cpuIndex >= numCPUs) || (cpuIndex < 0))
			showErrorMessage ("CPU index error (%d). There are %d CPUs attached. %s\n",
								cpuIndex,
								numCPUs,
								sm->contentsToString(true).c_str());
		else
			isCPU_Idle[cpuIndex] = true;
			
		// Current computing block has not been completely executed
		if ((sm_cpu->getRemainingMIs() > 0) || (sm_cpu->getCpuTime() > 0.0000001)){		
			
			sm_cpu->setIsResponse(false);
			requestsQueue.insert (sm_cpu);
		}		
		else{
			
			if (DEBUG_CPU_Scheduler_RR)
				showDebugMessage ("Computing block finished! Sending back to App:%s", sm_cpu->contentsToString(DEBUG_MSG_CPU_Scheduler_RR).c_str());
					
			sm_cpu->setIsResponse(true);
			sendResponseMessage (sm_cpu);
		}		

		// There are pending requests
		if (!requestsQueue.empty()){
			
			// Pop
			unqueuedMessage = (cMessage *) requestsQueue.pop();

			// Dynamic cast!
			nextRequest = check_and_cast<icancloud_Message *>(unqueuedMessage);

			// set the cpu for the return
			nextRequest->setNextModuleIndex(cpuIndex);

			// Update state!
			isCPU_Idle[cpuIndex]=false;	
			
			sm_cpuNext = check_and_cast<icancloud_App_CPU_Message *>(nextRequest);		
			
			// Debug
			if (DEBUG_CPU_Scheduler_RR)
				showDebugMessage ("Sending computing block to CPU[%d]:%s", cpuIndex, sm_cpuNext->contentsToString(DEBUG_MSG_CPU_Scheduler_RR).c_str());
										
			// Send!
			sendRequestMessage (nextRequest, toCPUGate[cpuIndex]);
		}	
}


int CPU_Scheduler_RR::searchIdleCPU (){
	
	unsigned int i;
	bool found;
	int result;
	
		// Init
		i = 0;
		found = false;
	
		// Search for an idle CPU
		while ((i<numCPUs) && (!found)){			
			
			if (isCPU_Idle[i])
				found = true;
			else
				i++;
		}
				
		// Result
		if (found)
			result = i;
		else
			result = NOT_FOUND;		
	
	return result;
}

