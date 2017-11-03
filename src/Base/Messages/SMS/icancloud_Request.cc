#include "icancloud_Request.h"


icancloud_Request::icancloud_Request (){
	arrivedSubRequests = 0;
	parentRequest = NULL;
}


icancloud_Request::icancloud_Request (cMessage *newParent){
	arrivedSubRequests = 0;
	parentRequest = newParent;
}


icancloud_Request::icancloud_Request (cMessage *newParent, unsigned int numSubReq){
	
	unsigned int i;
	
		arrivedSubRequests = 0;
		parentRequest = newParent;
		subRequests.reserve (numSubReq);
		
		for (i=0; i<numSubReq; i++)
			subRequests.push_back (NULL);
}


icancloud_Request::~icancloud_Request (){
	
	int i;		

		// Removes messages...
		for (i=0; i<((int)subRequests.size()); i++){
			
			if (subRequests[i] != NULL)				
				delete (subRequests[i]);
				subRequests[i] = NULL;
		}
		
	subRequests.clear();		
}


cMessage* icancloud_Request::getParentRequest (){
	return parentRequest;	
}


void icancloud_Request::setParentRequest (cMessage* newParent){
	parentRequest = newParent;
}


unsigned int icancloud_Request::getNumSubRequest (){
	return subRequests.size();
}


unsigned int icancloud_Request::getNumArrivedSubRequest (){
	return arrivedSubRequests;
}


void icancloud_Request::setSubRequest (cMessage* subRequest, unsigned int index){
	
	if (index < subRequests.size())
		subRequests[index] = subRequest;
	else		
		throw cRuntimeError("[icancloud_Request.setSubRequest] Index out of bounds!");	
	
}


void icancloud_Request::addSubRequest (cMessage* subRequest){
	subRequests.push_back (subRequest);
}


icancloud_Message* icancloud_Request::getSubRequest (unsigned int index){	
	
	icancloud_Message* sm;
		
		sm = NULL;
	
		if (index < subRequests.size()){
			if (subRequests[index] != NULL)
			sm = check_and_cast<icancloud_Message *> (subRequests[index]);
		}
		else
			throw cRuntimeError("[icancloud_Request.getSubRequest] Index out of bounds!");
		
	return sm;
}


icancloud_Message* icancloud_Request::popSubRequest (unsigned int index){
	
	icancloud_Message* subRequest;
	
		subRequest = NULL;
		
		if (index < subRequests.size()){			
			if (subRequests[index] != NULL){
				subRequest = check_and_cast<icancloud_Message *> (subRequests[index]);
				subRequests[index] = NULL;			
			}		
		}
		else
			throw cRuntimeError("[icancloud_Request.getSubRequest] Index out of bounds!");
		
	return subRequest;
}


icancloud_Message* icancloud_Request::popNextSubRequest (){
	
	icancloud_Message* subRequest;
	int i;
	bool found;

		// Init...
		subRequest = NULL;
		found = false;
		i=0;

		// Search for the next subRequest
		while ((!found) && (i<((int)subRequests.size()))){

			if (subRequests[i] != NULL){

				// Casting
				subRequest = check_and_cast<icancloud_Message *>(subRequests[i]);

               // Found!
                if (!subRequest->getIsResponse()){
                    //subRequests.erase(subRequests.begin() + i);
                    //subRequests[i] = NULL;
                    found=true;
                }

			}
			
			// Try with next subRequest...
			if (!found)
				i++;			
		}

	return subRequest;
}


bool icancloud_Request::arrivesAllSubRequest (){
	
	return (arrivedSubRequests == subRequests.size());
}


void icancloud_Request::arrivesSubRequest (cMessage* subRequest, unsigned int index){

	if (index < subRequests.size()){
		
		if (subRequests[index] == NULL){
			subRequests[index] = subRequest;
		}
		else{
		    subRequests[index] = NULL;
		}
        arrivedSubRequests++;
	}
	else{
		throw cRuntimeError("[icancloud_Request.arrivesSubRequest] Index out of bounds!");
	}
}

void icancloud_Request::clearSubRequests(){
    // Removes messages...
    for (int i=0; i<((int)subRequests.size()); i++){

        if (subRequests[i] != NULL)
            delete (subRequests[i]);
            subRequests[i] = NULL;
    }
}
void icancloud_Request::clear (){
	
	int i;	

		if (parentRequest != NULL){
			delete (parentRequest);
			parentRequest = NULL;
		}
	
		// Removes messages...
		for (i=0; i<((int)subRequests.size()); i++){
			
			if (subRequests[i] != NULL)
				delete (subRequests[i]);
				subRequests[i] = NULL;
		}	
}

