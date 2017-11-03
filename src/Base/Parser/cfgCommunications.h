#ifndef __CFG_COMMUNICATIONS_
#define __CFG_COMMUNICATIONS_

#include <omnetpp.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "icancloud_types.h"



/**
 * @class cfgCommunications cfgCommunications.h "cfgCommunications.h"
 *
 * Class that contains a list of servers.
 *
 * @author Gabriel Gonzalez Castañe
 * @date 2014-12-20
 */

class cfgCommunications{


    vector < vector <int> > communications;

	public:

		~cfgCommunications();
		cfgCommunications ();
		void parseFile (const char* fileName);
		int getNumProcesses ();
		int communicationsSize (int processRank);
		int communicationsElement(int processRank, int index);
		string toString ();
		void divide(const char* inputString, char separator);

};

#endif
