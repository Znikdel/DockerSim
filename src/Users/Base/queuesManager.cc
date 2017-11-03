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

#include "queuesManager.h"

queuesManager::~queuesManager() {
}

void queuesManager::finish(){
    icancloud_Base::finish();

    waitingQueue->clear();
    runningQueue->clear();
    finishQueue->clear();

    jobResults.clear();

    containerWaitingQueue->clear();
    containerRunningQueue->clear();
    containerFinishQueue->clear();

    containerJobResults.clear();
}

void queuesManager::initialize(){

    icancloud_Base::initialize();
        // Initialize structures and parameters
            waitingQueue = new JobQueue();
            waitingQueue->clear();

            runningQueue = new JobQueue();
            runningQueue->clear();

            finishQueue = new JobQueue();
            finishQueue->clear();

            containerWaitingQueue = new Container_JobQueue();
            containerWaitingQueue->clear();

            containerRunningQueue = new Container_JobQueue();
            containerRunningQueue->clear();

            containerFinishQueue = new Container_JobQueue();
            containerFinishQueue->clear();

}


void queuesManager::addParsedJob (jobBase *job){waitingQueue->insert_job(job);};
void queuesManager::addParsedContainerJob (Container_jobBase *job){containerWaitingQueue->insert_job(job);};


int queuesManager::getWQ_size(){return waitingQueue->size();};

int queuesManager::getRQ_size(){return runningQueue->size();};

int queuesManager::getFQ_size(){return finishQueue->size();};

bool queuesManager::isEmpty_WQ(){return waitingQueue->isEmpty();};

bool queuesManager::isEmpty_RQ(){return runningQueue->isEmpty();};

bool queuesManager::isEmpty_FQ(){return finishQueue->isEmpty();};

/////////////////////CONTAINER

int queuesManager::getCWQ_size(){return containerWaitingQueue->size();};

int queuesManager::getCRQ_size(){return containerRunningQueue->size();};

int queuesManager::getCFQ_size(){return containerFinishQueue->size();};

bool queuesManager::isEmpty_CWQ(){return containerWaitingQueue->isEmpty();};

bool queuesManager::isEmpty_CRQ(){return containerRunningQueue->isEmpty();};

bool queuesManager::isEmpty_CFQ(){return containerFinishQueue->isEmpty();};
//////////////////////////////////////

bool queuesManager::eraseJob_FromWQ (int jobID){

	//Define...

	    unsigned int i;
		vector <jobBase*>::iterator jobIt;
		bool found;

	//Initialize...

		i = 0;
		found = false;

	//Search the job into jobsList
		for (i = 0; (waitingQueue->get_queue_size()) && (!found); i++ ){

			if (waitingQueue->getJob(i)->getId() == jobID){

				waitingQueue->removeJob(i);
				found = true;
			}
		}
		return found;
}

bool queuesManager::eraseJob_FromRQ (int jobID){

	//Define...

	    unsigned int i;
		vector <jobBase*>::iterator jobIt;
		bool found;

	//Initialize...

		i = 0;
		found = false;

	//Search the job into jobsList
		for (i = 0; (runningQueue->get_queue_size()) && (!found); i++ ){

			if (runningQueue->getJob(i)->getId() == jobID){

				runningQueue->removeJob(i);
				found = true;
			}
		}
		return found;
}

bool queuesManager::eraseJob_FromFQ (int jobID){

	//Define...

	    unsigned int i;
		vector <jobBase*>::iterator jobIt;
		bool found;
	//Initialize...

		i = 0;
		found = false;
	//Search the job into jobsList
		for (i = 0; (finishQueue->get_queue_size()) && (!found); i++ ){

			if (finishQueue->getJob(i)->getId() == jobID){

				finishQueue->removeJob(i);
				break;
				found = true;
			}
		}

		return found;
}

void queuesManager::pushBack_WQ(jobBase* job){
    (waitingQueue->insert_job(job));
}

void queuesManager::pushBack_RQ(jobBase* job){runningQueue->insert_job(job);};

void queuesManager::pushBack_FQ(jobBase* job){finishQueue->insert_job(job);};

void queuesManager::insert_WQ(jobBase* job, int position){waitingQueue->insert_job(job,position);};

void queuesManager::insert_RQ(jobBase* job, int position){runningQueue->insert_job(job,position);};

void queuesManager::insert_FQ(jobBase* job, int position){finishQueue->insert_job(job,position);};

void queuesManager::move_WQ (int positionInitial, int positionFinal){waitingQueue->move_job_from_to(positionInitial, positionFinal);};

void queuesManager::move_RQ (int positionInitial, int positionFinal){runningQueue->move_job_from_to(positionInitial, positionFinal);};

void queuesManager::move_FQ (int positionInitial, int positionFinal){finishQueue->move_job_from_to(positionInitial, positionFinal);};

jobBase* queuesManager::getJob_WQ (int jobID){return get_job_by_ID(jobID, waitingQueue);};

jobBase* queuesManager::getJob_RQ (int jobID){return get_job_by_ID(jobID, runningQueue);};

jobBase* queuesManager::getJob_FQ (int jobID){return get_job_by_ID(jobID, finishQueue);};

jobBase* queuesManager::getJob_WQ_index  (int index){return get_job_by_index (index, waitingQueue);};

jobBase* queuesManager::getJob_RQ_index  (int index){return get_job_by_index (index, runningQueue);};

jobBase* queuesManager::getJob_FQ_index  (int index){return get_job_by_index (index, finishQueue);};

jobBase* queuesManager::getJobByModID_WQ (int modID){return get_job_by_ModID (modID, waitingQueue);};

jobBase* queuesManager::getJobByModID_RQ (int modID){return get_job_by_ModID (modID, runningQueue);};

jobBase* queuesManager::getJobByModID_FQ (int modID){return get_job_by_ModID (modID, finishQueue);};

int queuesManager::getIndex_WQ (int jobID){return getIndexOfJob(jobID, waitingQueue);};

int queuesManager::getIndex_RQ (int jobID){return getIndexOfJob (jobID, runningQueue);};

int queuesManager::getIndex_FQ (int jobID){return getIndexOfJob (jobID, finishQueue);};

void queuesManager::moveFromWQ_toRQ (int jobID){
    //Define
        jobBase* job;
        bool found;
        int index;
    //Initialize
        found = false;
        index = 0;
    //Erase from qSrc and insert into qDst;

         while ((index < waitingQueue->size()) && (!found)){

             job = waitingQueue->getJob(index);
                if (job->getId() == jobID){

                    found = true;
                    index++;
                }
            }

    waitingQueue->move_to_qDst(index ,runningQueue, runningQueue->get_queue_size());
}

void queuesManager::moveFromRQ_toFQ (int jobID){
    //Define
        jobBase* job;
        bool found;
        int index;
    //Initialize
        found = false;
        index = 0;
    //Erase from qSrc and insert into qDst;

         while ((index < runningQueue->size()) && (!found)){
             job = runningQueue->getJob(index);
             if (job->getId() == jobID){

                    found = true;
             }else{
                    index++;
             }
         }

    runningQueue->move_to_qDst(index ,finishQueue, finishQueue->get_queue_size());
}


void queuesManager::moveFromWQ_toFQ (int jobID){
    //Define
        jobBase* job;
        bool found;
        int index;
    //Initialize
        found = false;
        index = 0;
    //Erase from qSrc and insert into qDst;

         while ((index < waitingQueue->size()) && (!found)){

             job = waitingQueue->getJob(index);

                if (job->getId() == jobID){
                    found = true;
                }else{
                    index++;
                }
            }

    waitingQueue->move_to_qDst(index ,finishQueue, finishQueue->get_queue_size());
}

jobBase* queuesManager::get_job_by_ID (int jobID, JobQueue* jq){

	//Define...

	    unsigned int i;
		jobBase * job;
		vector <jobBase*>::iterator jobIt;
		bool found;

	//Initialize...
		found = false;
		i = 0;

	//Search the job into jobsList
		for (i = 0; (jq->get_queue_size()) && !(found); i++ ){

			if (jq->getJob(i)->getId() == jobID){

				job = jq->getJob(i);
				found = true;
			}
		}

		return job;

}

jobBase* queuesManager::get_job_by_index (int index, JobQueue* jq){

	// Define ...

		jobBase * job;

	// Initialize...

		job = NULL;

	// Begin ..

		if (index <= jq->get_queue_size()-1){

			job = jq->getJob(index);

		}

		return job;

}

jobBase* queuesManager::get_job_by_ModID (int modID, JobQueue* jq){

    // Define ...
        jobBase * job;
        bool found;
        int index;

    // Initialize...
        job = NULL;
        found = false;
        index = 0;

    // Begin ..

        while ((index < jq->get_queue_size() ) && (!found)){

            job = jq->getJob(index);

            if (job->getId() == modID){
                found = true;
            }

            index++;

        }

        return job;
}

int queuesManager::getIndexOfJob (int jobID, JobQueue *jq){

	//Define...
	    int i;
	    int index;
	    bool found;

	//Initialize...
		i = 0;
		index = -1;
		found = false;

	//Search the job into jobsList

		for (i = 0; (jq->get_queue_size()) && !(found); i++ ){

			if (jq->getJob(i)->getId() == jobID){

				index = i;
				found = true;
			}
		}

		return index;
}

//////////////////Container
bool queuesManager::eraseJob_FromCWQ (int jobID){

    //Define...

        unsigned int i;
        vector <Container_jobBase*>::iterator jobIt;
        bool found;

    //Initialize...

        i = 0;
        found = false;

    //Search the job into jobsList
        for (i = 0; (containerWaitingQueue->get_queue_size()) && (!found); i++ ){

            if (containerWaitingQueue->getJob(i)->getId() == jobID){

                containerWaitingQueue->removeJob(i);
                found = true;
            }
        }
        return found;
}

bool queuesManager::eraseJob_FromCRQ (int jobID){

    //Define...

        unsigned int i;
        vector <Container_jobBase*>::iterator jobIt;
        bool found;

    //Initialize...

        i = 0;
        found = false;

    //Search the job into jobsList
        for (i = 0; (containerRunningQueue->get_queue_size()) && (!found); i++ ){

            if (containerRunningQueue->getJob(i)->getId() == jobID){

                containerRunningQueue->removeJob(i);
                found = true;
            }
        }
        return found;
}

bool queuesManager::eraseJob_FromCFQ (int jobID){

    //Define...

        unsigned int i;
        vector <Container_jobBase*>::iterator jobIt;
        bool found;
    //Initialize...

        i = 0;
        found = false;
    //Search the job into jobsList
        for (i = 0; (containerFinishQueue->get_queue_size()) && (!found); i++ ){

            if (containerFinishQueue->getJob(i)->getId() == jobID){

                containerFinishQueue->removeJob(i);
                break;
                found = true;
            }
        }

        return found;
}

void queuesManager::pushBack_CWQ(Container_jobBase* job){
cout<<"pushBack_CWQ"<<endl;
    (containerWaitingQueue->insert_job(job));
}

void queuesManager::pushBack_CRQ(Container_jobBase* job){
    cout<<"pushBack_CRQ"<<endl;

    containerRunningQueue->insert_job(job);};

void queuesManager::pushBack_CFQ(Container_jobBase* job){
    cout<<"pushBack_CFQ"<<endl;

    containerFinishQueue->insert_job(job);};

void queuesManager::insert_CWQ(Container_jobBase* job, int position){
    cout<<"insert_CWQ"<<endl;

    containerWaitingQueue->insert_job(job,position);};

void queuesManager::insert_CRQ(Container_jobBase* job, int position){
    cout<<"insert_CRQ"<<endl;

    containerRunningQueue->insert_job(job,position);};

void queuesManager::insert_CFQ(Container_jobBase* job, int position){
    cout<<"insert_CFQ"<<endl;

    containerFinishQueue->insert_job(job,position);};

void queuesManager::move_CWQ (int positionInitial, int positionFinal){

    cout<<"move_CWQ"<<endl;
    containerWaitingQueue->move_job_from_to(positionInitial, positionFinal);};

void queuesManager::move_CRQ (int positionInitial, int positionFinal){
    cout<<"move_CRQ"<<endl;

    containerRunningQueue->move_job_from_to(positionInitial, positionFinal);};

void queuesManager::move_CFQ (int positionInitial, int positionFinal){

    cout<<"move_CFQ"<<endl;

    containerFinishQueue->move_job_from_to(positionInitial, positionFinal);};

Container_jobBase* queuesManager::getJob_CWQ (int jobID){return get_container_job_by_ID(jobID, containerWaitingQueue);};

Container_jobBase* queuesManager::getJob_CRQ (int jobID){return get_container_job_by_ID(jobID, containerRunningQueue);};

Container_jobBase* queuesManager::getJob_CFQ (int jobID){return get_container_job_by_ID(jobID, containerFinishQueue);};

Container_jobBase* queuesManager::getJob_CWQ_index  (int index){return get_container_job_by_index (index, containerWaitingQueue);};

Container_jobBase* queuesManager::getJob_CRQ_index  (int index){return get_container_job_by_index (index, containerRunningQueue);};

Container_jobBase* queuesManager::getJob_CFQ_index  (int index){return get_container_job_by_index (index, containerFinishQueue);};

Container_jobBase* queuesManager::getJobByModID_CWQ (int modID){return get_container_job_by_ModID (modID, containerWaitingQueue);};

Container_jobBase* queuesManager::getJobByModID_CRQ (int modID){return get_container_job_by_ModID (modID, containerRunningQueue);};

Container_jobBase* queuesManager::getJobByModID_CFQ (int modID){return get_container_job_by_ModID (modID, containerFinishQueue);};

int queuesManager::getIndex_CWQ (int jobID){return getIndexOfContainerJob(jobID, containerWaitingQueue);};

int queuesManager::getIndex_CRQ (int jobID){return getIndexOfContainerJob (jobID, containerRunningQueue);};

int queuesManager::getIndex_CFQ (int jobID){return getIndexOfContainerJob (jobID, containerFinishQueue);};

void queuesManager::moveFromCWQ_toCRQ (int jobID){
    //Define
        Container_jobBase* job;
        bool found;
        int index;
    //Initialize
        found = false;
        index = 0;
    //Erase from qSrc and insert into qDst;

         while ((index < containerWaitingQueue->size()) && (!found)){

             job = containerWaitingQueue->getJob(index);
                if (job->getId() == jobID){

                    found = true;
                    index++;
                }
            }

    containerWaitingQueue->move_to_qDst(index ,containerRunningQueue, containerRunningQueue->get_queue_size());
}

void queuesManager::moveFromCRQ_toCFQ (int jobID){
    //Define
        Container_jobBase* job;
        bool found;
        int index;
    //Initialize
        found = false;
        index = 0;
    //Erase from qSrc and insert into qDst;

         while ((index < containerRunningQueue->size()) && (!found)){
             job = containerRunningQueue->getJob(index);
             if (job->getId() == jobID){

                    found = true;
             }else{
                    index++;
             }
         }

    containerRunningQueue->move_to_qDst(index ,containerFinishQueue, containerFinishQueue->get_queue_size());
}


void queuesManager::moveFromCWQ_toCFQ (int jobID){
    //Define
        Container_jobBase* job;
        bool found;
        int index;
    //Initialize
        found = false;
        index = 0;
    //Erase from qSrc and insert into qDst;

         while ((index < containerWaitingQueue->size()) && (!found)){

             job = containerWaitingQueue->getJob(index);

                if (job->getId() == jobID){
                    found = true;
                }else{
                    index++;
                }
            }

    containerWaitingQueue->move_to_qDst(index ,containerFinishQueue, containerFinishQueue->get_queue_size());
}

Container_jobBase* queuesManager::get_container_job_by_ID (int jobID, Container_JobQueue* jq){

    //Define...

        unsigned int i;
        Container_jobBase * job;
        vector <Container_jobBase*>::iterator jobIt;
        bool found;

    //Initialize...
        found = false;
        i = 0;

    //Search the job into jobsList
        for (i = 0; (jq->get_queue_size()) && !(found); i++ ){

            if (jq->getJob(i)->getId() == jobID){

                job = jq->getJob(i);
                found = true;
            }
        }

        return job;

}

Container_jobBase* queuesManager::get_container_job_by_index (int index, Container_JobQueue* jq){

    // Define ...

        Container_jobBase * job;

    // Initialize...

        job = NULL;

    // Begin ..

        if (index <= jq->get_queue_size()-1){

            job = jq->getJob(index);

        }

        return job;

}

Container_jobBase* queuesManager::get_container_job_by_ModID (int modID, Container_JobQueue* jq){

    // Define ...
        Container_jobBase * job;
        bool found;
        int index;

    // Initialize...
        job = NULL;
        found = false;
        index = 0;

    // Begin ..

        while ((index < jq->get_queue_size() ) && (!found)){

            job = jq->getJob(index);

            if (job->getId() == modID){
                found = true;
            }

            index++;

        }

        return job;
}

int queuesManager::getIndexOfContainerJob (int jobID, Container_JobQueue *jq){

    //Define...
        int i;
        int index;
        bool found;

    //Initialize...
        i = 0;
        index = -1;
        found = false;

    //Search the job into jobsList

        for (i = 0; (jq->get_queue_size()) && !(found); i++ ){

            if (jq->getJob(i)->getId() == jobID){

                index = i;
                found = true;
            }
        }

        return index;
}

///////////////////////
