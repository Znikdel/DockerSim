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

#include "GeneralUser.h"
Define_Module(GeneralUser);

GeneralUser::~GeneralUser() {

	// TODO Auto-generated destructor stub
}

void GeneralUser::initialize(){

        printVMs = par("printVMs").boolValue();
        printJobs = par("printJobs").boolValue();
        AbstractCloudUser::initialize();
        print = false;
}

void GeneralUser::finish(){
    AbstractCloudUser::finish();
}


void GeneralUser::userInitialization(){

	// Define ..
		AbstractRequest* vms;
		cout<<"void GeneralUser::userInitialization()"<<endl;

	// Select the vms to start (all) and then send the request to the cloud manager.
		vms = selectVMs_ToStartUp ();
	// Start all vms
		if (vms != NULL)
		    startVMs(vms);
}

void GeneralUser::userFinalization (){

    // Define ..
        int finishQSize,i,k;
        string jobs;
        ostringstream userData;
        ostringstream userJobsData;
        ostringstream userVmsData;
        ofstream resFile;
        vmStatesLog_t* vm_state;
        vector<cModule*> mods;
        VM* vm;
    // Init ..

    // Begin..
        //Open the file
        if (print){
           resFile.open(file.c_str(), ios::app );
        }

    // Get the user data
        // UserName - startTime - totalTime -
        userData << userName.c_str() << ";" << startTime.dbl() << ";" << endTime.dbl() << endl;

        if (printJobs){
        // Get the jobs
            finishQSize = getJobResultsSize();

            for (i = 0; i < finishQSize; i++){
                jobs = printJobResults (getJobResults(i));
                userJobsData << jobs;
            }
        }

     // Get the vms
        if (printVMs){

            int size = wastedVMs.size();

            for (i = 0; i < size; i++){

                    vm = (*(wastedVMs.begin()));

                    userVmsData << "#" << vm->getTypeName() << "[" << vm->getName() << "]:";

                    for (k = 0; k < vm->getLogStatesQuantity(); k++){
                        vm_state = vm->getLogState(k);

                        if ( strcmp(vm_state->vm_state.c_str(),MACHINE_STATE_OFF) == 0) userVmsData << (vm_state->init_time_M) << ";";
                        userVmsData << (vm_state->vm_state) << ":" ;
                        userVmsData << (vm_state->init_time_M) << ";";
                    }

            }
            userVmsData << endl;
        }

    //Print everything and close the file
    if (print){
         resFile << userData.str().c_str();
         if (printJobs) resFile << userJobsData.str().c_str();
         if (printVMs) resFile << userVmsData.str().c_str();
         resFile << endl;                       // Put a blank line between users
         resFile.close();
     }

}
/* --------------------------------------------------------------------
 * ------------------------ Interact with VMs -------------------------
 * --------------------------------------------------------------------
 */

AbstractRequest* GeneralUser::selectVMs_ToStartUp (){

	//Define...
		unsigned int i;
		unsigned int setSize;
		bool reqFilled;
		int maxNumVMsToRequest;
		int size;
		RequestVM* req ;

	//Initialize...
		size = 0;
		//maxNumVMsToRequest = getWQ_size();
		maxNumVMsToRequest = getWQ_size()+getCWQ_size();
		cout<<"WQ Size--->"<<getWQ_size()<<endl;
		cout<<"CWQ Size--->"<<getCWQ_size()<<endl;
		setSize = vmsToBeSelected.size();
		cout <<"setSize of vmsToBeSelected by user--->"<< getUserId()<<":"<< setSize <<endl;
		req = new RequestVM();
		reqFilled = false;

	/*
	 *  Obtain all the VMs of the user. Initially all are into VM_NOT_REQUESTED state.
	 *  Then, user request for the VMs change state to free, and that is task of the cloud manager.
	 *  If user request a VM that is running, the cloud manager discard that request.
	 */
	for (i = 0; (i != setSize) && (maxNumVMsToRequest != 0);i++){
	    size = (((*(vmsToBeSelected.begin() + i))->quantity) - maxNumVMsToRequest);
        cout<<"((*(vmsToBeSelected.begin() + i))->quantity)--->"<<((*(vmsToBeSelected.begin() + i))->quantity) <<endl;
        cout<<"maxNumVMsToRequest--->"<<maxNumVMsToRequest<<endl;
        cout<<"Size--->"<<size<<endl;

	    if (size <= 0){
	        req->setNewSelection((*(vmsToBeSelected.begin() + i))->type->getType(),
	                             (*(vmsToBeSelected.begin() + i))->quantity);

	        (*(vmsToBeSelected.begin() + i))->quantity = (*(vmsToBeSelected.begin() + i))->quantity - size;
	        reqFilled = true;
	    } else {
	        req->setNewSelection((*(vmsToBeSelected.begin() + i))->type->getType(),
	                             (*(vmsToBeSelected.begin() + i))->quantity - size);
	        reqFilled = true;
            (*(vmsToBeSelected.begin() + i))->quantity = (*(vmsToBeSelected.begin() + i))->quantity - maxNumVMsToRequest;
	        break;
	    }

	}

	if (!reqFilled) req = NULL;

	AbstractRequest* reqA;
	reqA = dynamic_cast<AbstractRequest*>(req);

	return reqA;

}

AbstractRequest* GeneralUser::selectResourcesJob (jobBase* job){

	// Define ..
		VM* vm;
		Machine* machine;
		vector<VM*> selectedVMs;
		bool found;
		unsigned int i = 0;
		unsigned int j = 0;
		AbstractRequest* aReq;
		RequestVM* reqVM;
	// Init...
		vm = NULL;
		found = false;
		selectedVMs.clear();
		reqVM = new RequestVM();

	// The behavior of the selection of VMs is to get the first VM in free state.
	for (i = 0; i < machinesMap->size() && (!found); i++){

		for (j = 0; (j < (unsigned int)machinesMap->getSetQuantity(i)) && (!found); j++){

		    machine = machinesMap->getMachineByIndex(i,j);
		    vm = dynamic_cast<VM*>(machine);
			if (vm->getPendingOperation() == NOT_PENDING_OPS){
                if (vm->getState() == MACHINE_STATE_IDLE) {
                //if ((vm->getVmState() == MACHINE_STATE_IDLE) || (vm->getVmState() == MACHINE_STATE_RUNNING)) {
                    selectedVMs.insert(selectedVMs.begin(), vm);
                    //selectedVMs.insert(selectedVMs.begin(), vm);
                    found = true;

                }
			}
		}
	}

//		machine = machinesMap->getMachineByIndex(0,0);
//		vm = dynamic_cast<VM*>(machine);
//		selectedVMs.insert(selectedVMs.begin(), vm);

		reqVM->setVectorVM(selectedVMs);
		aReq = dynamic_cast<AbstractRequest*>(reqVM);

	return aReq;

}
AbstractRequest* GeneralUser::selectResourcesContainerJob (Container_jobBase* job){
//Wanna find VMs to run container jobs on it
cout<<"GeneralUser::selectResourcesContainerJob "<<endl;
    // Define ..
        VM* vm;
        Machine* machine;
        vector<VM*> selectedVMs;
        bool found;
        unsigned int i = 0;
        unsigned int j = 0;
        AbstractRequest* aReq;
        RequestVM* reqVM;
    // Init...
        vm = NULL;
        found = false;
        selectedVMs.clear();
        reqVM = new RequestVM();
        cout<<"job->getNumCopies()--->"<<job->getNumCopies()<<endl;
    // The behavior of the selection of VMs is to get the first VM in free state.
        //cout<< "machinesMap->size()--->"<<machinesMap->size()<<endl;
        for (i = 0; i < machinesMap->size() && (!found); i++){
        //cout<<"machinesMap->getSetQuantity(i)---->"<<machinesMap->getSetQuantity(i)<<endl;

            for (j = 0; (j < (unsigned int)machinesMap->getSetQuantity(i)) && (!found); j++){

                machine = machinesMap->getMachineByIndex(i,j);
                vm = dynamic_cast<VM*>(machine);
              //  cout<<"seleted VM name---->"<<vm->getFullName()<<endl;
              //  cout<<"vm->getPendingOperation() ---->"<<vm->getPendingOperation() <<endl;
                if (vm->getPendingOperation() == NOT_PENDING_OPS){ // NOT_PENDING_OPS=0
                //    cout<<"vm->getState()--->"<<vm->getState() <<endl;
                    if (vm->getState() == MACHINE_STATE_IDLE) {
                    //if ((vm->getVmState() == MACHINE_STATE_IDLE) || (vm->getVmState() == MACHINE_STATE_RUNNING)) {
                        selectedVMs.insert(selectedVMs.begin(), vm);
                        found = true;

                    }
                }
        }
    }

//      machine = machinesMap->getMachineByIndex(0,0);
//      vm = dynamic_cast<VM*>(machine);
//      selectedVMs.insert(selectedVMs.begin(), vm);

        reqVM->setVectorVM(selectedVMs);
        aReq = dynamic_cast<AbstractRequest*>(reqVM);

    return aReq;

}

void GeneralUser::requestAttended (AbstractRequest* req){

	// Define ..
		int exceededVMs;
		int i;
		RequestVM* reqVM;

	// Initialize ..
		i = 0;
		cout<<">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>.GeneralUser::requestAttended  >>>>>>>>>>>>>>>>>>>>>>>."<<endl;
	//	cout<<"operation: "<<req->getOperation()<<endl;
	// Begin ..
	if (req->getOperation() == REQUEST_START_VM){
        cout<< "REQUEST_START_VM"<< endl;

        reqVM = dynamic_cast<RequestVM*>(req);

		exceededVMs = reqVM->getVMQuantity() -  (getWQ_size()+getCWQ_size());

		if (isEmpty_WQ()&& isEmpty_CWQ()){
			while (i !=  reqVM->getVMQuantity()) {
				shutdown_VM (reqVM->getVM(i));
				reqVM->eraseVM(i);
			}

		} else if (exceededVMs > 0){

			while (exceededVMs != 0){
				shutdown_VM(reqVM->getVM(reqVM->getVMQuantity() -1 ));
				reqVM->eraseVM(reqVM->getVMQuantity() - 1 );
				exceededVMs--;
			}
		}

	} else if ((req->getOperation() == REQUEST_LOCAL_STORAGE) || (req->getOperation() == REQUEST_REMOTE_STORAGE)||(req->getOperation() == CONTAINER_REQUEST_LOCAL_STORAGE) || (req->getOperation() == CONTAINER_REQUEST_REMOTE_STORAGE)){
        cout<< "REQUEST_LOCAL_STORAGE"<< endl;

	    // Only execute the pending jobs that the vms of the request have assigned..
        executePendingJobs ();

	} else if( (req->getOperation() == REQUEST_FREE_RESOURCES) ||( (req->getOperation() == CONTAINER_REQUEST_FREE_RESOURCES))){
	    cout<< "REQUEST_FREE_RESOURCES"<< endl;
		// User has pending jobs | requests?
		//if ( (getWQ_size() == 0) && (getRQ_size() == 0) && (checkAllVMShutdown())){
	    if ( (getWQ_size() == 0) && (getRQ_size() == 0) &&  (getCWQ_size() == 0) && (getCRQ_size() == 0) && (checkAllVMShutdown())){
			finalizeUser();
		}
	}
	else {
		showErrorMessage("Unknown request operation in BasicBehavior: %i", req->getOperation());
	}
	cout<<">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>.GeneralUser::requestAttended  ENDENENENENENENED >>>>>>>>>>>>>>>>>>>>>>>."<<endl;
}

void GeneralUser::requestErrorDeleted (AbstractRequest* req){

	printf ("request deleted: - ");
}

/* --------------------------------------------------------------------
 * ------------------------- Interact with jobs -----------------------
 * --------------------------------------------------------------------
 */

//UserJob* GeneralUser::selectJob(){
jobBase* GeneralUser::selectJob(){
	// Define ..
		jobBase *job;
	//	UserJob* jobC;

    // Init ..
		job = NULL;

	//get the job from the cloudManager

		if (! isEmpty_WQ()){

			job =  getJob_WQ_index(0);

		} else {

			job = NULL;

		}

	//	jobC = dynamic_cast<UserJob*> (job);

	//return jobC;
		return job;
}
//Container_UserJob* GeneralUser::selectContainerJob(){
Container_jobBase* GeneralUser::selectContainerJob(){
cout<< " #######################GeneralUser::selectContainerJob()"<<endl;
    // Define ..
        Container_jobBase *job;
     //   Container_UserJob* jobC;

    // Init ..
        job = NULL;

    //get the job from the cloudManager

        if (! isEmpty_CWQ()){

            job =  getJob_CWQ_index(0);

        } else {

            job = NULL;

        }

     //  jobC = dynamic_cast<Container_UserJob*> (job);
        cout<< " #######################GeneralUser::selectContainerJob()------ENDENDEND---######"<<endl;

    return job;
}


void GeneralUser::jobHasFinished (jobBase* job){

    UserJob* jobC;
    Machine* m;
    jobC = dynamic_cast<UserJob*>(job);
    m = jobC->getMachine();

    VM* vm = check_and_cast<VM*>(m);
    cout<< "jobHasFinished for --->"<< vm->getFullName() << endl;

    if (jobC == NULL) throw cRuntimeError ("GeneralUser::jobHasFinished->job can not be casted to CloudJob\n");

    if ((vm->getNumProcessesRunning() == 1) && (getWQ_size() == 0) && (getCWQ_size() == 0)) {


        // if the vm is not more needed, request to shutdown it

        shutdown_VM(vm);

    }

}
void GeneralUser::containerjobHasFinished (Container_jobBase* job)
{

    Container_UserJob* jobC;
    Machine* m;

    jobC = dynamic_cast<Container_UserJob*>(job);
    m = jobC->getMachine();

    VM* vm = check_and_cast<VM*>(m);

    if (jobC == NULL) throw cRuntimeError ("GeneralUser::Container jobHasFinished->job can not be casted to CloudJob\n");

    if ((vm->getNumProcessesRunning() == 1) && (getCWQ_size() == 0) && (getWQ_size() == 0)) {


        // if the vm is not more needed, request to shutdown it

        shutdown_VM(vm);

    }


}


/* --------------------------------------------------------------------
 * ------------------------- User Behavior ----------------------------
 * --------------------------------------------------------------------
 */

void GeneralUser::schedule(){
cout<<"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^66GeneralUser::schedule()^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"<<endl;
	Enter_Method_Silent();

	// Define ..

		UserJob* job;
		jobBase* jobB;
		Container_UserJob* cJob;
		Container_jobBase* cJobB;
		AbstractRequest* reqB;
        AbstractRequest* cReqB;
        RequestVM* reqVM;
		RequestVM* cReqVM;
		vector <VM*> setToExecute;
		unsigned int i;
		int quantityVMFree;
		int waitingQSize;
		vector<RequestVM*> requests_vector_storage;
		vector<RequestVM*>::iterator reqIt;
		bool breakScheduling;
		VM* vm;

	// Init ..

		job = NULL;
		setToExecute.clear();
		requests_vector_storage.clear();
		quantityVMFree = 0;
		waitingQSize = 0;
		breakScheduling = false;

		// Begin the behavior of the user.
		jobB=selectJob();
		job = dynamic_cast<UserJob*> (jobB); // Job comes from waiting queue

		while ((job != NULL) && (!breakScheduling)){

			 reqB= selectResourcesJob (job);
			 reqVM = dynamic_cast<RequestVM*>(reqB);
			 cout<<"reqVM->getVMQuantity()--->"<<reqVM->getVMQuantity()<<endl;
			if (reqVM->getVMQuantity() != 0){

				// Allocate the set of machines where the job is going to execute into the own job

					vm = dynamic_cast<VM*>(reqVM->getVM(0));

					job->setMachine(vm);
					jobB = dynamic_cast<jobBase*>(job);

					if ((vm == NULL) || (jobB == NULL)){
					    throw cRuntimeError ("GeneralUser::schedule() -> vm or job == NULL\n");
					}

					createFSforJob (job, job->getMachine()->getIP(), vm->getNodeSetName(), vm->getNodeName(), vm->getPid());
					delete(reqVM);

			} else {

				// check if the APP needs more VMs than existent free (to allocate smaller jobs)
				for (i = 0; i != machinesMap->size(); i++){
					quantityVMFree += machinesMap->countONMachines(i);
				}
				cout<<"quantityVMFree--->"<<quantityVMFree<<endl;
				waitingQSize = getWQ_size();

				if ( (quantityVMFree > 0) && (waitingQSize >= 1) ){

					// There are not enough VMs to execute the first job.
					// It is moved from the first position to the last to execute other job that needs less resources
                    jobB = dynamic_cast<jobBase*>(job);
				    eraseJob_FromWQ(job->getJobId());
				    pushBack_WQ(jobB);

				}

				breakScheduling = true;

			}

			jobB=selectJob();
		    job = dynamic_cast<UserJob*> (jobB); // Job comes from waiting queue
		}


		cJobB = selectContainerJob();
	//	cout<<"cJobB->getNumCopies()--->"<<cJobB->getNumCopies()<<endl;
	    cJob =  dynamic_cast<Container_UserJob*> (cJobB); // cJob comes from waiting queue
        breakScheduling = false;
        quantityVMFree = 0;
        waitingQSize = 0;
        cout<<"before while:"<<endl;
	        while ((cJob != NULL) && (!breakScheduling)){
	            cout<<"General user scheduling container--->"<<cJob->getFullName()<<endl;
	             cReqB= selectResourcesContainerJob (cJob); // get first free VM
	             cReqVM = dynamic_cast<RequestVM*>(cReqB);
	             cout<<"cReqVM->getVMQuantity()--->"<<cReqVM->getVMQuantity()<<endl;
	            if (cReqVM->getVMQuantity() != 0){

	                // Allocate the set of machines where the cJob is going to execute into the own cJob

	                    vm = dynamic_cast<VM*>(cReqVM->getVM(0));
	                    cout<<vm->getFullName()<<endl;
	                    cJob->setMachine(vm);
	                    cJobB = dynamic_cast<Container_jobBase*>(cJob);

	                    if ((vm == NULL) || (cJobB == NULL)){
	                        throw cRuntimeError ("GeneralUser::schedule() -> vm or  ContainerJob == NULL\n");
	                    }

	                    createFSforContainerJob (cJob, cJob->getMachine()->getIP(), vm->getNodeSetName(), vm->getNodeName(), vm->getPid());
	                    delete(cReqVM);

	            } else {

	                // check if the APP needs more VMs than existent free (to allocate smaller jobs)
	                for (i = 0; i != machinesMap->size(); i++){
	                    quantityVMFree += machinesMap->countONMachines(i);
	                }
	                cout << "quantityVMFree---->"<<quantityVMFree<<endl;
	                waitingQSize = getCWQ_size();

	                if ( (quantityVMFree > 0) && (waitingQSize >= 1) ){

	                    // There are not enough VMs to execute the first cJob.
	                    // It is moved from the first position to the last to execute other cJob that needs less resources
	                    cJobB = dynamic_cast<Container_jobBase*>(cJob);
	                    eraseJob_FromCWQ(cJob->getJobId());
	                    pushBack_CWQ(cJobB);

	                }

	                breakScheduling = true;

	            }

	            cJobB = selectContainerJob();
	            cJob =  dynamic_cast<Container_UserJob*> (cJobB); // cJob comes from waiting queue

	            if (cJob!=NULL) cout<<"cJob--->"<<cJob->getFullName()<<endl;
	          }
	        cout<<"after while:"<<endl;


	        if ( (cJob == NULL && job==NULL) ){
	            cout<<"(cJob == NULL && job==NULL)"<<endl;
	            for (i = 0; i < (unsigned int)machinesMap->getMapQuantity(); i++){
	                for (int j = 0; j < (int)machinesMap->getSetQuantity(i); j++){

	                    Machine* machine = machinesMap->getMachineByIndex(i,j);
	                    VM* vm = dynamic_cast<VM*>(machine);

	                    if ( (vm->getPendingOperation() == NOT_PENDING_OPS) &&
	                         (vm->getState() == MACHINE_STATE_IDLE) &&
	                         (vm->getNumProcessesRunning() == 0) ){
	                        cout<<"calling shutdown vm"<<endl;
	                            shutdown_VM(vm);
	                    }
	                }
	            }

	        }
            cout<<"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^66GeneralUser::schedule()^^^^^^^^^^ENDEND^^^^^^^^^^^^^^^^^^^^^"<<endl;

}





