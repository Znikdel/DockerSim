#include "CPUcore.h"
#define ZERO_DOUBLE 0.000000F

Define_Module (CPUcore);

CPUcore::~CPUcore(){

}

void CPUcore::initialize(int stage){

    if (stage != 2)
        ICore::initialize(stage);
    else
    {
        std::ostringstream osStream;

	    // Set the moduleIdName
	    osStream << "CPUcore." << getId();
	    moduleIdName = osStream.str();
	    
	    // Get the energy behaviour of cores
	    independentCores = getParentModule()-> par("indepentCores").boolValue();

	    // Get the speed parameter
	    speed = par ("speed").longValue();
	    tick_s = par ("tick_s").doubleValue();
	    current_speed = 0;
	    currentTime = 0;
	    executingMessage = false;

	    // Calculate the ipt
	    calculateIPT(speed);

	    // Gate IDs
	    inGate = gate ("in");
	    outGate = gate ("out");
	    	    
	    pendingMessage = NULL;	    
	    
	    showStartedModule ("CPU Core Speed:%d MIPS.  Tick:%f s.  IPT:%d",
			    			speed,
			    			tick_s.dbl(),
			    			ipt);
	    
	    /** The set of pstates */
	    actualPState = 0;

	    nodeState = MACHINE_STATE_OFF;
    }

}


void CPUcore::finish(){

    // Finish the super-class
    HWEnergyInterface::finish();
}


cGate* CPUcore::getOutGate (cMessage *msg){

	// If msg arrive from scheduler
	if (msg->getArrivalGate()==inGate){
		if (outGate->getNextGate()->isConnected()){
			return (outGate);
		} else {
			showErrorMessage("E_CPUcore: nextGate to outGate not connected");
		}
	} else {
		showErrorMessage("E_CPUcore: arrival gate different to inGate");
	}

	// If gate not found!
	return NULL;
}


void CPUcore::processSelfMessage (cMessage *msg){

			// Latency message...
		if (!strcmp (msg->getName(), SM_LATENCY_MESSAGE.c_str())){

			// Cast!
		    icancloud_App_CPU_Message *sm_cpu = check_and_cast<icancloud_App_CPU_Message *>(pendingMessage);
	
			// Init pending message...
			pendingMessage = NULL;
	
			// establish is a response msg
			sm_cpu->setIsResponse(true);			

			// Send message back!
			sendResponseMessage (sm_cpu);

			// Reset the time control
            currentTime = 0;

            // Deactivate the flag
            executingMessage = false;

		}

		else if (!strcmp (msg->getName(), "continueExecuting")){

		    icancloud_Message *msgToExecute;

		    msgToExecute = check_and_cast<icancloud_Message *>(pendingMessage);

            // Init pending message...
            pendingMessage = NULL;

            // Active the flag of continue executing
            executingMessage = true;

		    // Process the executing message
		    processRequestMessage(msgToExecute);
		}

		else
			showErrorMessage ("Unknown self message [%s]", msg->getName());	

		cancelAndDelete(msg);
}


void CPUcore::processRequestMessage (icancloud_Message *sm){

	icancloud_App_CPU_Message *sm_cpu;
	simtime_t cpuTime = 0;

	string nodeState;
	int operation;
	bool executionComplete ;
	
	// Initialize
	executionComplete = true;
	// Casting to debug!
	sm_cpu = check_and_cast<icancloud_App_CPU_Message *>(sm);

	operation = sm_cpu->getOperation();

	if (operation != SM_CHANGE_CPU_STATE){

	    if (actualPState == 0){
	        throw cRuntimeError("The cpu core has received a message to compute and it is in off state");
	    }

		if (pendingMessage != NULL)
			showErrorMessage ("Core currently processing another computing block!");		
		
		// Link pending message
		pendingMessage = sm_cpu;	
		
		cpuTime = sm_cpu->getCpuTime();

		// Uses time
		if (cpuTime > 0.0){

			// Executes completely the CB
			if ( (cpuTime > 0.0) && (cpuTime <= 0.1) ){
				changeState (C0_P11);
			} else if ( (cpuTime > 0.0) && (cpuTime <= 0.2) ){
				changeState (C0_P10);
			} else if ( (cpuTime > 0.0) && (cpuTime <= 0.3) ){
				changeState (C0_P9);
			} else if ( (cpuTime > 0.0) && (cpuTime <= 0.4) ){
				changeState (C0_P7);
			} else if ( (cpuTime > 0.0) && (cpuTime <= 0.5) ){
				changeState (C0_P6);
			} else if ( (cpuTime > 0.0) && (cpuTime <= 0.6) ){
				changeState (C0_P5);
			} else if ( (cpuTime > 0.0) && (cpuTime <= 0.7) ){
				changeState (C0_P4);
			} else if ( (cpuTime > 0.0) && (cpuTime <= 0.8) ){
				changeState (C0_P2);
			} else if ( (cpuTime > 0.0) && (cpuTime <= 0.9) ){
				changeState (C0_P1);
			} else if ( (cpuTime > 0.0) && (cpuTime <= 1.0) ){
				changeState (C0_P0);
			}

			if (
			   (sm_cpu->getQuantum() == INFINITE_QUANTUM) ||
			   ((sm_cpu->getQuantum() * tick_s.dbl()) > ((sm_cpu->getCpuTime()).dbl()))
			   ){
				cpuTime = getMaximumTimeToExecute (sm_cpu->getCpuTime());
			}
			
			// Executes a slice
	        // TODO: The cpu scheduler should increase the speed of the cpu depending on the calculated load..
			else{
				cpuTime = tick_s.dbl() * sm_cpu->getQuantum();
				sm_cpu->executeTime (cpuTime);				
			}
		}

		// Execute current computing block completely.

		else{

			if ((sm_cpu->getQuantum() == INFINITE_QUANTUM) || (sm_cpu->getQuantum() >= ceil (sm_cpu->getRemainingMIs()/ipt))){

			    if (!executingMessage){

			        currentTime = getTimeToExecuteCompletely (sm_cpu->getRemainingMIs());
			        sm_cpu->executeMIs (sm_cpu->getRemainingMIs());
			    }

                if (currentTime <= 1){
                      executionComplete = true;
                }
                // 1 is a factor that should be adjusted depending on the fast than the dvfs increases!
                else{
                    changeDeviceState(INCREMENT_SPEED);
                    currentTime = currentTime - 1;
                    executionComplete = false;
                }
                cpuTime = currentTime;
			}

			// Execute the corresponding number of ticks
			else{
	            // TODO: The cpu scheduler should increase the speed of the cpu depending on the calculated load..
				cpuTime = tick_s.dbl() * sm_cpu->getQuantum();
				sm_cpu->executeMIs (ipt * sm_cpu->getQuantum());
			}
		}
		
		if (DEBUG_CPUcore)
	    	showDebugMessage ("[%s] Processing Cb. Computing time:%f - %s",
	    						moduleIdName.c_str(),
	    						cpuTime.dbl(),
	    						sm_cpu->contentsToString(DEBUG_MSG_CPUcore).c_str());		
		
		if (executionComplete)	{
		    cMessage* message = new cMessage (SM_LATENCY_MESSAGE.c_str());
		    scheduleAt (simTime()+cpuTime, message);
		}
		else {
		    cMessage* message = new cMessage ("continueExecuting");
		    scheduleAt (simTime()+cpuTime, message);
		}

	} else {
		// The node is changing the state!
		nodeState = sm_cpu->getChangingState().c_str();

		changeDeviceState (nodeState.c_str());

		delete(sm);
	}

}


void CPUcore::processResponseMessage (icancloud_Message *sm){
	showErrorMessage ("This module cannot receive response messages!");
}


simtime_t CPUcore::getTimeToExecuteCompletely (long int remainingMIs){
	
	simtime_t cpuTime;
	double instructionsPerSecond = ((1/tick_s.dbl())*ipt);

	cpuTime = ((remainingMIs/instructionsPerSecond));

	return cpuTime;

}


simtime_t CPUcore::getMaximumTimeToExecute (simtime_t remainingTime){
	
	simtime_t cpuTime;

		cpuTime = ((int) ceil (remainingTime.dbl() / (tick_s.dbl()))) * (tick_s.dbl());

	return cpuTime;
}


void CPUcore::changeDeviceState (string state, unsigned componentIndex){

	string pstate;
	int pstatePosition = actualPState;

	const char * const PStates[] = { "off", "c0_p11", "c0_p10", "c0_p9", "c0_p8", "c0_p7", "c0_p6", "c0_p5", "c0_p4", "c0_p3", "c0_p2", "c0_p1", "c0_p0" };
	if (strcmp (state.c_str(),MACHINE_STATE_IDLE ) == 0) {

		nodeState = MACHINE_STATE_IDLE;
		changeState (C0_P11);

	} else if (strcmp (state.c_str(),MACHINE_STATE_RUNNING ) == 0) {

		nodeState = MACHINE_STATE_RUNNING;
		changeState (C0_P10);

	} else if (strcmp (state.c_str(),MACHINE_STATE_OFF ) == 0) {

		nodeState = MACHINE_STATE_OFF;
		changeState (OFF);

	} else if (strcmp (state.c_str(),INCREMENT_SPEED ) == 0) {

		nodeState = MACHINE_STATE_RUNNING;
		if (pstatePosition < 12){
		    pstatePosition++;
			pstate = PStates[pstatePosition++];
			changeState (pstate);
		}

	} else if (strcmp (state.c_str(),DECREMENT_SPEED ) == 0) {

		nodeState = MACHINE_STATE_RUNNING;
		if (pstatePosition > 1){
		    pstatePosition--;
			pstate = PStates[pstatePosition--];
			changeState (pstate);
		}
	} else  {

        nodeState = MACHINE_STATE_OFF;
        changeState (OFF);
	}
}


void CPUcore::changeState (string energyState, unsigned componentIndex){

	double relation_speed;
	int min_speed = 0;
	int statesSize = (e_getStatesSize()-1);

	// calculate the relation_speed;
	min_speed = speed - (speed * 0.6);
	relation_speed = ( (speed - min_speed) / statesSize);

	// The cpu is completely off.
	if (strcmp (energyState.c_str(), OFF) == 0){
		actualPState = 0;
		current_speed = 0;
	}

	// P-state 0 - 2.40 GHz
	else if (strcmp (energyState.c_str(), C0_P0) == 0){
		current_speed = speed;
		actualPState = 12;
	}

	// P-state 1 - 2.27 GHz
	else if (strcmp (energyState.c_str(),C0_P1) == 0){
	    current_speed = (speed - relation_speed);
		actualPState = 11;
	}

	// P-state 2 - 2.13 GHz
	else if (strcmp (energyState.c_str(),C0_P2) == 0){
		current_speed = (speed - (relation_speed * 2));
		actualPState = 10;
	}

	// P-state 3 - 2.00 GHz
	else if (strcmp (energyState.c_str(),C0_P3) == 0){
		current_speed = (speed -(relation_speed * 3));
		actualPState = 9;
	}

	// P-state 4 - 1.87 GHz
	else if (strcmp (energyState.c_str(),C0_P4) == 0){
		current_speed = (speed -(relation_speed * 4));
		actualPState = 8;
	}

	// P-state 5 - 1.73 GHz
	else if (strcmp (energyState.c_str(),C0_P5) == 0){
		current_speed = (speed -(relation_speed * 5));
		actualPState = 7;
	}

	// P-state 6 - 1.60 GHz
	else if (strcmp (energyState.c_str(),C0_P6) == 0){
		current_speed = (speed -(relation_speed * 6));
		actualPState = 6;
	}

	// P-state 7 - 1.47 GHz
 	else if (strcmp (energyState.c_str(),C0_P7) == 0){
 		current_speed = (speed -(relation_speed * 7));
 		actualPState = 5;
 	}

	// P-state 8 - 1.33 GHz
 	else if (strcmp (energyState.c_str(),C0_P8) == 0){
 		current_speed = (speed -(relation_speed * 8));
 		actualPState = 4;
 	}

	// P-state 9 - 1.20 GHz
 	else if (strcmp (energyState.c_str(),C0_P9) == 0){
 		current_speed = (speed -(relation_speed * 9));
 		actualPState = 3;
 	}

	// P-state 10 - 1.07 GHz
 	else if (strcmp (energyState.c_str(),C0_P10) == 0){
 		current_speed = (speed -(relation_speed * 10));
 		actualPState = 2;
 	}

	// P-state 11 - 933 GHz
 	else if (strcmp (energyState.c_str(),C0_P11) == 0){
 		current_speed = (speed -(relation_speed * 11));
 		actualPState = 1;
 	}

//	// Stops CPU main internal clocks via software; bus interface unit and APIC are kept running at full speed.
// 	else if (strcmp (energyState.c_str(),C1_Halt) == 0){
//
// 	}
//
//	// Stops CPU main internal clocks via hardware and reduces CPU voltage; bus interface unit and APIC are kept running at full speed.
// 	else if (strcmp (energyState.c_str(),C2_StopGrant) == 0){
//
// 	}
//
//	// Stops all CPU internal and external clocks
// 	else if (strcmp (energyState.c_str(),C3_Sleep) == 0){
//
// 	}
//
//	// Reduces CPU voltage
// 	else if (strcmp (energyState.c_str(),C4_DeeperSleep) == 0){
//
// 	}
//
//	// Reduces CPU voltage even more and turns off the memory cache
// 	else if (strcmp (energyState.c_str(),C5_EnhancedDeeperSleep) == 0){
//
// 	}
//
// 	else {
//
// 	}

	calculateIPT(current_speed);
	e_changeState (energyState);

}
