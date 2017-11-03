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

#include "Container_JobQueue.h"

Container_JobQueue::Container_JobQueue() {
	idQueue.clear();
}

Container_JobQueue::~Container_JobQueue() {
    idQueue.clear();

    Container_jobBase* job;

    for (int i = 0; i < get_queue_size(); i++){

        job = getJob(i);
        job->callFinish();
        job->deleteModule();
    }
}

void Container_JobQueue::insert_job (Container_jobBase* job, int position){

	int qSize;

	qSize = idQueue.size();

	if (position == -1) idQueue.push_back(job);

	else
	    if ((position <= qSize) && (position >= 0)) idQueue.insert(idQueue.begin()+position, job);

}

Container_jobBase* Container_JobQueue::getJob (int index){

	// Define ..
		Container_jobBase* job;
		vector<Container_jobBase*>::iterator it;
		int qSize;

	// Init..
		job = NULL;
		it = idQueue.begin();
		qSize = idQueue.size();

	if (index < qSize){

		it += index;
		job = (*it);

	}

	return job;

}

void Container_JobQueue::removeJob (int index){

	//Define ..
		vector<Container_jobBase*>::iterator idQueueIt;

	// Init ..
		idQueueIt = idQueue.begin();

	if (index <= (int)idQueue.size()){

		idQueueIt += index;
		idQueue.erase(idQueueIt);

	}

}

void Container_JobQueue::move_job_from_to (int initPos, int dstPos){

	//Define...
		vector<Container_jobBase*>::iterator it;
		Container_jobBase* job;

	//Init ..

		it = idQueue.begin();

	// Begin ..

		if (initPos != dstPos){

			it += dstPos + 1;

			idQueue.insert(it,job);
			removeJob(initPos);
		}

}

void Container_JobQueue::move_to_qDst  (int position_qSrc, Container_JobQueue* qDst, int position_qDst){

    cout<<"Container_JobQueue::move_to_qDst"<<endl;
	//Define
	    int qSize;
		Container_jobBase *job;

	//Initialize
		qSize = idQueue.size();
		job = NULL;

	//Erase from qSrc and insert into qDst;
		job = getJob(position_qSrc);

		if (   ((position_qSrc > -1) && (position_qSrc < qSize) ) ||
		        (position_qDst == -1)){

			qDst -> insert_job(job, position_qDst);
			removeJob(position_qSrc);

		} else {

			printf ("\nError inserting job in position: %d , into destination queue. Maybe the position of job doesn't exist?\n", position_qSrc);

		}
}

void Container_JobQueue::move_from_qSrc (int position_qSrc, Container_JobQueue* qSrc, int position_qDst){

	// Define ..
	    int qSize;
		Container_jobBase *job;

	// Initialize ..
        qSize = idQueue.size();
		job = NULL;

	// Begin ..;

		job = getJob(position_qSrc);

		if ( (position_qSrc > -1) && (position_qSrc < qSize) ) {

			insert_job(job, position_qDst);
			qSrc->removeJob(position_qSrc);

		} else {

			printf ("\nError inserting job in position: %d , into destination queue. Maybe the position of job doesn't exist?\n", position_qSrc);

		}

}

int Container_JobQueue::get_index_of_job(int jobID){
    // Define ..
        int jobIDAux;
        int i;
        Container_jobBase *job;
        bool found;

    // Initialize ..
        i = 0;
        job = NULL;
        found = false;

    // Begin ..;

        while ((i < (int)idQueue.size()) && (!found)){
            job = getJob(i);
            jobIDAux = job->getId();

            if (jobIDAux == jobID) {
                found = true;
            }else {
                i++;
            }
        }

        if (!found) i = -1;

        return i;
}
