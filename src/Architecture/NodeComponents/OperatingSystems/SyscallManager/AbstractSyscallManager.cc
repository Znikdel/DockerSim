#include "AbstractSyscallManager.h"

AbstractSyscallManager::~AbstractSyscallManager(){
	
}


void AbstractSyscallManager::initialize(){
    if (DEBUG_CLOUD_SCHED) printf("\n Method[AbstractSyscallManager]: ------->initialize\n");

    // Init the super-class
    icancloud_Base::initialize();

    pendingJobs.clear();
    processesRunning.clear();

        // Initialize
        std::ostringstream osStream;

        // Set the moduleIdName
        osStream << "." << getId();
        moduleIdName = osStream.str();

        toAppGates = new cGateManager(this);
        fromAppGates = new cGateManager(this);

    //    toOSContainerGates = new cGateManager(this);
   //     fromOSContainerGates = new cGateManager(this);
        toDockerEngineGates= new cGateManager(this);
        fromDockerEngineGates= new cGateManager(this);

        fromMemoryGate = gate ("fromMemory");
        fromNetGate = gate ("fromNet");
        fromCPUGate = gate ("fromCPU");

        toMemoryGate = gate ("toMemory");
        toNetGate = gate ("toNet");
        toCPUGate = gate ("toCPU");

        processesRunning.clear();

        totalMemory_KB = totalStorage_KB = memoryFree_KB = storageFree_KB = 0;

        if (DEBUG_CLOUD_SCHED) printf("\n Method[AbstractSyscallManager]: ------->initialize---------------FIN-----------\n");

}


void AbstractSyscallManager::finish(){

	// Finish the super-class
	icancloud_Base::finish();
}

cGate* AbstractSyscallManager::getOutGate (cMessage *msg){

        // If msg arrive from Applications
		if (msg->arrivedOn("fromApps")){
			return (gate("toApps", msg->getArrivalGate()->getIndex()));
		}
	    if (msg->arrivedOn("fromDockerEngine")){
	            return (gate("toDockerEngine", msg->getArrivalGate()->getIndex()));
	        }

		// If msg arrive from Memory
		else if (msg->getArrivalGate()==fromMemoryGate){
			if (gate("toMemory")->getNextGate()->isConnected()){
				return (toMemoryGate);
			}
		}
		
		// If msg arrive from Net
		else if (msg->getArrivalGate()==fromNetGate){
			if (gate("toNet")->getNextGate()->isConnected()){
				return (toNetGate);
			}
		}
		
		// If msg arrive from CPU
		else if (msg->getArrivalGate()==fromCPUGate){
			if (gate("toCPU")->getNextGate()->isConnected()){
				return (toCPUGate);
			}
		}		
		
	// If gate not found!
	return NULL;
}

void AbstractSyscallManager::processSelfMessage(cMessage* msg){


    icancloud_Message* hmsg;

    hmsg = dynamic_cast<icancloud_Message*>(msg);

    if (hmsg->isSelfMessage()){

            bool found = false;

            for (int i = 0; (i < (int)pendingJobs.size()) && (!found); i++){
                if ((*(pendingJobs.begin() +i))->messageId == hmsg->getId()){

                    found = true;
                    createProcess((*(pendingJobs.begin() + i))->job, hmsg->getUid());
                    pendingJobs.erase(pendingJobs.begin() + i);

                }

            }

            if (!found) throw cRuntimeError("AbstractSyscallManager::processSelfMessage -> error. There is not pending jobs to be created\n");


        cancelAndDelete(msg);
    } else {
        throw cRuntimeError ("Unknown message at AbstractNode::handleMessage\n");
    }
}

void AbstractSyscallManager::processResponseMessage (icancloud_Message *sm){
		
	// Send back the message
	sendResponseMessage (sm);
}

int AbstractSyscallManager::searchUserId(int jobId){
    bool found = false;
    int uid = -1;

    for (int i = 0; (i < (int)processesRunning.size()) && (!found); i++){
        if ((*(processesRunning.begin() + i))->process->getId() == jobId){
            found = true;
            uid = (*(processesRunning.begin() + i))->process->getId();
        }
    }

    if (uid == -1) throw cRuntimeError("SyscallManager::searchUserId- Job with id = %i has not allocated as processes running\n", jobId);

    return uid;
}

void AbstractSyscallManager::removeAllProcesses(){

        for (int i = 0; i < (int)processesRunning.size(); i++)
            removeProcess((*(processesRunning.begin() + i))->process->getId());
}

void AbstractSyscallManager::allocateJob(icancloud_Base* job, simtime_t timeToStart, int uId){

    // Define ..
        icancloud_Message* msg;
        pendingJob* p_job;

        msg = new icancloud_Message ("process_job");

        msg->setUid(uId);

        // Creates the structure;
        p_job = new pendingJob();
        p_job->job = job;
        p_job->messageId = msg->getId();

        pendingJobs.push_back(p_job);

        scheduleAt (simTime()+timeToStart, msg);

}
void AbstractSyscallManager::allocateContainerJob(icancloud_Base* job, simtime_t timeToStart, int uId){

    // Define ..
        icancloud_Message* msg;
        pendingJob* p_job;

        msg = new icancloud_Message ("process_job");

        msg->setUid(uId);

        // Creates the structure;
        p_job = new pendingJob();
        p_job->job = job;
        p_job->messageId = msg->getId();

        pendingJobs.push_back(p_job);

        scheduleAt (simTime()+timeToStart, msg);

}

bool AbstractSyscallManager::isAppRunning(int pId){

    bool found = false;

    for (int i = 0; (i < (int)processesRunning.size()) && (!found); i++){
        if ((*(processesRunning.begin() + i))->process->getId() == pId){
            found = true;
        }
    }

    if (!found){
        for (int i = 0; (i < (int)pendingJobs.size()) && (!found); i++){
            if ((*(pendingJobs.begin() + i))->job->getId() == pId){
                found = true;
            }
        }
    }

    return found;

}
bool AbstractSyscallManager::isContainerRunning(int pId){

    bool found = false;

    for (int i = 0; (i < (int)processesRunning.size()) && (!found); i++){
        if ((*(processesRunning.begin() + i))->process->getId() == pId){
            found = true;
        }
    }

    if (!found){
        for (int i = 0; (i < (int)pendingJobs.size()) && (!found); i++){
            if ((*(pendingJobs.begin() + i))->job->getId() == pId){
                found = true;
            }
        }
    }

    return found;

}
icancloud_Base* AbstractSyscallManager::deleteJobFromStructures(int jobId){
    bool found = false;
    icancloud_Base* job;

    for (int i = 0; (i < (int)processesRunning.size()) && (!found); i++){
        if ((*(processesRunning.begin() + i))->process->getId() == jobId){
            found = true;
            job = (*(processesRunning.begin() + i))->process;
            processesRunning.erase(processesRunning.begin() + i);
        }
    }


    if (!found){
        for (int i = 0; (i < (int)pendingJobs.size()) && (!found); i++){
            if ((*(pendingJobs.begin() + i))->job->getId() == jobId){
                found = true;
                job = (*(pendingJobs.begin() + i))->job;
                pendingJobs.erase(pendingJobs.begin() + i);
            }
        }
    }

    return job;
}
