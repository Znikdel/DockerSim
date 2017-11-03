#include "cfgCommunications.h"


cfgCommunications::~cfgCommunications (){
		
	communications.clear();
}


cfgCommunications::cfgCommunications (){
    communications.clear();
}


void cfgCommunications::parseFile(const char* fileName){

	FILE *serversFD;
	char processCommunications [LINE_SIZE];
	std::ostringstream info;


		// Init...
		bzero (processCommunications, LINE_SIZE);

		// Open the servers file
		serversFD = fopen (fileName, "r");

		// There is no servers file
		if (serversFD != NULL){
			// Read the number of entries
			while (fgets (processCommunications, LINE_SIZE, serversFD) != NULL)
			    divide(processCommunications, ' ');


		    fclose(serversFD);
        }
}


int cfgCommunications::getNumProcesses(){

	return (communications.size());
}


int cfgCommunications::communicationsSize (int processRank){
    return (*(communications.begin() + processRank)).size();
}


int cfgCommunications::communicationsElement (int processRank, int index){
    return (*( (*(communications.begin() + processRank)).begin() + index ));
}

string cfgCommunications::toString (){
	
	int i;
	std::ostringstream info;

		if (communications.size() == 0)
			info << "communications is empty!" << endl;

		else{
			info << "Printing app communications from cfgCommunications class!..." << endl;

			for (i=0; i<((int)communications.size()); i++){
			    info << "process [" << i << "]: ";
			    for (int j = 0; j < (int) communicationsSize(i); j++){
			        info << communicationsElement(i,j) << " ";
			    }
			    info << endl;
			}
		}
	return info.str();
}

void cfgCommunications::divide(const char* inputString, char separator){

    vector<int> parts;
    int a;

    parts.clear();

    std::istringstream f(inputString);

    std::string s;
    while (std::getline(f, s, separator)) {
        a = atoi(s.c_str());
        if (a != -1)
            parts.push_back(a);
    }

    if (parts.size() == 0) parts.push_back(-1);

    communications.push_back(parts);

}
