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

#include "Container_JobResultsSet.h"

Container_JobResultsSet::Container_JobResultsSet() {
    container_JobResultsSet.clear();
    jobID = "";
    job_startTime = SimTime();
    job_endTime = SimTime();
}

Container_JobResultsSet::~Container_JobResultsSet() {
    container_JobResultsSet.clear();
    jobID = "";
}

void Container_JobResultsSet::finishJobResultsSet(){
    container_JobResultsSet.clear();
    jobID = "";
}

int Container_JobResultsSet::getJobResultSize (){

    return container_JobResultsSet.size();

}

Container_JobResults* Container_JobResultsSet::getJobResultSet (string value){

	vector<Container_JobResults*>::iterator it;
	bool found = false;
	Container_JobResults* jobRes = NULL;

	for (unsigned int i = 0; (i < container_JobResultsSet.size()) && (!found); i++){

	    if (strcmp ((*(container_JobResultsSet.begin() + i))->getName().c_str(), value.c_str()) == 0){
	        found = true;
	        jobRes = (*(container_JobResultsSet.begin() + i));
	    }
	}

	return jobRes;
}

void Container_JobResultsSet::newJobResultSet(string jobResult){
    Container_JobResults* jobRes;

    // check if the value exists
    for (unsigned int i = 0; (i < container_JobResultsSet.size()); i++){
        jobRes = (*(container_JobResultsSet.begin() + i));
        if (strcmp (jobRes->getName().c_str(), jobResult.c_str()) == 0){
            throw cRuntimeError("Error: Container_JobResultsSet::newJobResultSet->%s. The value %s exists previously.", jobResult.c_str());
        }
    }

    jobRes = new Container_JobResults();
    jobRes->setName(jobResult);
    container_JobResultsSet.push_back(jobRes);
}

void Container_JobResultsSet::setJobResult (string jobResultsName, int value){
    std::ostringstream info;
    info << value;
    setJobResult_p (jobResultsName, info.str());
}

void Container_JobResultsSet::setJobResult (string jobResultsName, double value){
    std::ostringstream info;
    info << value;
    setJobResult_p (jobResultsName, info.str());
}

void Container_JobResultsSet::setJobResult (string jobResultsName, simtime_t value){
    std::ostringstream info;
    info << value.dbl();
    setJobResult_p (jobResultsName, info.str());
}

void Container_JobResultsSet::setJobResult (string jobResultsName, bool value){
    if (value)
        setJobResult_p (jobResultsName, "true");
    else
        setJobResult_p (jobResultsName, "false");

}

void Container_JobResultsSet::setJobResult (string jobResultsName, string value){
    setJobResult_p (jobResultsName, value.c_str());
}

void Container_JobResultsSet::setJobResult (int indexJobResult, int value){
    std::ostringstream info;
    info << value;
    setJobResult_p (indexJobResult, info.str());
}

void Container_JobResultsSet::setJobResult (int indexJobResult, double value){
    std::ostringstream info;
    info << value;
    setJobResult_p (indexJobResult, info.str());
}

void Container_JobResultsSet::setJobResult (int indexJobResult, simtime_t value){
    std::ostringstream info;
    info << value.dbl();
    setJobResult_p (indexJobResult, info.str());
}

void Container_JobResultsSet::setJobResult (int indexJobResult, bool value){
    if (value)
        setJobResult_p (indexJobResult, "true");
    else
        setJobResult_p (indexJobResult, "false");
}

void Container_JobResultsSet::setJobResult (int indexJobResult, string value){
    setJobResult_p (indexJobResult, value.c_str());
}

Container_JobResultsSet* Container_JobResultsSet::dup (){

    Container_JobResultsSet* set;
    Container_JobResults* container_JobResults;
    set = new Container_JobResultsSet();

    for (int i = 0; i < (int) container_JobResultsSet.size(); i++){

        container_JobResults = getJobResultSet(i)->dup();
        set->newJobResultSet(container_JobResults->getName());
        for (int j = 0; j < container_JobResults->getValuesSize(); j++){
            set->setJobResult(i,container_JobResults->getValue(j));
        }
    }

    set->setJobID(getJobID());
    set->setMachineType(getMachineType());
    set->setJob_startTime(getJob_startTime());
    set->setJob_endTime(getJob_endTime());

    return set;
}

void Container_JobResultsSet::setJob_startTime(simtime_t simtime){
    job_startTime = simtime;
}

void Container_JobResultsSet::setJob_endTime(simtime_t simtime){
    job_endTime = simtime;
}

string Container_JobResultsSet::toString(){

	// Define..
	std::ostringstream info;
	vector <Container_JobResults*>::iterator it;

	if (container_JobResultsSet.size() == 0)
		info << "  There are no results yet!";
	else
		info << "\n\t\t\t";

	for (it = container_JobResultsSet.begin(); it != container_JobResultsSet.end() ; it++){

		info << "{" << (*it)->toString() << "} - " ;
	}

	return info.str();
}

void Container_JobResultsSet::setJobResult_p (string jobResultsName, string value){
        vector<Container_JobResults*>::iterator it;
        bool found = false;
        Container_JobResults* jobRes = NULL;

        for (int i = 0; (i < (int)container_JobResultsSet.size()) && (!found); i++){

            if (strcmp ((*(container_JobResultsSet.begin()) + i)->getName().c_str(), jobResultsName.c_str()) == 0){
                found = true;
                (*(container_JobResultsSet.begin() + i))->setValue(value);
            }
        }

        if (!found){
            jobRes = new Container_JobResults();
            jobRes->setName(jobResultsName.c_str());
        }
}

void Container_JobResultsSet::setJobResult_p (int indexJobResult, string value){
    (*(container_JobResultsSet.begin() + indexJobResult))->setValue(value);
}
