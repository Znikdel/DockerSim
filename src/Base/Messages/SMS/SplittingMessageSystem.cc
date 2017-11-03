#include "SplittingMessageSystem.h"

const bool SplittingMessageSystem::verboseMode = false;



int SplittingMessageSystem::searchRequest (cMessage* request){

	bool found;
	int index;
	vector <icancloud_Request*>::iterator reqIterator;

		// Init...
		found = false;
		index = 0;

		// Search the request!
		for (reqIterator = requestVector.begin();
			 (reqIterator != requestVector.end() && (!found));
			 ++reqIterator){

			 	// Found?
			 	if ((*reqIterator)->getParentRequest() == request)
			 		found = true;
			 	else
			 		index++;
		}

		// not found...
		if (!found)
			index = NOT_FOUND;

	return index;
}


int SplittingMessageSystem::getNumberOfSubRequest (cMessage* request){

	int index;

		// Search...
		index = searchRequest (request);

		// Request found...
		if (index != NOT_FOUND)
			return ((requestVector[index])->getNumSubRequest());
		else
			throw cRuntimeError("[SplittingMessageSystem::removeRequest] Request not found!!!");
}


bool SplittingMessageSystem::arrivesAllSubRequest (cMessage* request){

	int index;

		// Search...
		index = searchRequest (request);

		// Request found...
		if (index != NOT_FOUND){
			return ((requestVector[index])->arrivesAllSubRequest());
		}
		else
			throw cRuntimeError("[SplittingMessageSystem::arrivesAllSubRequest] Request not found!!!");
}


void SplittingMessageSystem::addRequest (cMessage* request, unsigned int numSubRequests){

	icancloud_Request *newRequest;
	
		newRequest = new icancloud_Request (request, numSubRequests);
    	requestVector.push_back (newRequest);
}


void SplittingMessageSystem::removeRequest (cMessage* request){

	int index;
	vector <icancloud_Request*>::iterator reqIterator;

		// Search for the corresponding request
		index = searchRequest (request);

		// Request found... remove it!
		if (index != NOT_FOUND){
			reqIterator = requestVector.begin();
			reqIterator = reqIterator + index;
			(*reqIterator)->clearSubRequests();
			delete (*reqIterator);
			requestVector.erase (reqIterator);
		}
		else
			throw cRuntimeError("[SplittingMessageSystem::removeRequest] Request not found!!!");
}


void SplittingMessageSystem::setSubRequest (cMessage* request, cMessage* subRequest, unsigned int subReqIndex){

	int index;

		// Search...
		index = searchRequest (request);

		// Request found...
		if (index != NOT_FOUND){

			// Check if subReqIndex is correct!
			(requestVector[index])->setSubRequest (subRequest, subReqIndex);
		}
		else
			throw cRuntimeError("[SplittingMessageSystem::setSubRequest] Request not found!!!");
}


icancloud_Message* SplittingMessageSystem::popSubRequest (cMessage* parentRequest, int subRequestIndex){

	icancloud_Message* subRequest;
	int index;

		// Init...
		subRequest = NULL;

		// Search for the parent request
		index = searchRequest (parentRequest);

		// found!
		if (index!=NOT_FOUND){			
			subRequest = (requestVector[index])->popSubRequest (subRequestIndex);
		}

	return subRequest;
}


icancloud_Message* SplittingMessageSystem::popNextSubRequest (cMessage* parentRequest){
	
	icancloud_Message* subRequest;
	int index;

		// Init...
		subRequest = NULL;

		// Search for the parent request
		index = searchRequest (parentRequest);

		// found!
		if (index!=NOT_FOUND){			
			subRequest = (requestVector[index])->popNextSubRequest ();
		}

	return subRequest;
}


void SplittingMessageSystem::arrivesSubRequest (cMessage* subRequest, cMessage* parentRequest){

	icancloud_Message *subReqMsg;
	std::ostringstream info;
	int indexParent;
	int indexSubRequest;


		// Search for the parent request
		indexParent = searchRequest (parentRequest);
		
		// Cast to icancloud_Message
		subReqMsg = check_and_cast<icancloud_Message *>(subRequest);

		// found!
		if (indexParent != NOT_FOUND){

			// Get the subRequest index!
			indexSubRequest = subReqMsg->getCurrentRequest();

			// Link!
			(requestVector[indexParent])->arrivesSubRequest(subRequest, indexSubRequest);

			delete(subRequest);
		}

		else{
			info << "Parent request not found! subRequest info:"
				 << subReqMsg->contentsToString(verboseMode);
			throw cRuntimeError(info.str().c_str());
		}
}


int SplittingMessageSystem::getNumberOfRequests(){
	return requestVector.size();
}


string SplittingMessageSystem::requestToString (cMessage* request, bool printContents){

	std::ostringstream info;
	int index;
		

			// Search for the request...
			index = searchRequest (request);
	
			// Request not found...
			if (index==NOT_FOUND)
				info << "Request Not Found!" << endl;
			else{
				if (printContents)
					info << requestToStringByIndex (index);
			}		

	return info.str();
}

SplittingMessageSystem::~SplittingMessageSystem(){

}
