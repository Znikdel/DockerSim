
/**
 * @class Container_jobBase Container_jobBase.h Container_jobBase.cc
 *
 * This class models a job.
 *
 * @author Gabriel Gonz&aacute;lez Casta&ntilde;&eacute
 * @date 2014-12-12
 */

#ifndef Container_jobBase_H_
#define Container_jobBase_H_

#include "Container_JobResultsSet.h"
#include "API_OS.h"

class API_OS;

class Container_jobBase : virtual public API_OS {
//class Container_jobBase{
protected:

		/** The Apps module name **/
    	string appType;

        /** Attributes for creating the structures of FS and preload files*/
        vector <fsStructure_T*> fsStructures;
        vector <preload_T*> preloadFiles;

        /** A set of results. The system generates one set per iteration **/
        Container_JobResultsSet* Container_JobResults;

        /* Original paramters */
        int numCopies;
        string originalName;
        int jobState;
        int commId;

protected:

    	/*
    	 * Destructor
    	 */
    	~Container_jobBase();

        /**
         * Module initialization.
         */
         virtual void initialize();

        /**
         * Module ending.
         */
         virtual void finish();

        /**
         * Process a self message.
         * @param msg Self message.
         */
         virtual void processSelfMessage (cMessage *msg) = 0;

        /**
         * Process a request message.
         * @param sm Request message.
         */
         virtual void processRequestMessage (icancloud_Message *sm) = 0;

        /**
         * Process a response message from a module in the local node.
         * @param sm Request message.
         */
         virtual void processResponseMessage (icancloud_Message *sm) = 0;

public:

        /**
        * Start the app execution.
        */
        virtual void startExecution () = 0;

        /*
        * Getters and setters for routes at file system
        */
    	void setFSElement(fsStructure_T* elto){fsStructures.push_back(elto);};
    	fsStructure_T* getFSElement(int index){return (*(fsStructures.begin() + index));};
    	int getFSSize(){return fsStructures.size();};
    	vector<fsStructure_T*> getFSComplete(){return fsStructures;};

        /*
        * Getters and setters for routes at file system
        */
    	void setPreloadFile(preload_T* elto){preloadFiles.push_back(elto);};
    	preload_T* getPreloadFile(int index){return (*(preloadFiles.begin() + index));};
    	int getPreloadSize(){return preloadFiles.size();};
    	vector<preload_T*> getPreloadComplete(){return preloadFiles;};
    	void clearPreloadFiles(){preloadFiles.clear();}

    	void setAppType(string name){appType = name;};
    	string getAppType(){return originalName;}; // TODO: Analyze to change the name to apptype

    	void addResults(Container_JobResultsSet* newResult){Container_JobResults = newResult;};
    	Container_JobResultsSet* getResults(){return Container_JobResults;};

        void setJob_startTime(){Container_JobResults->setJob_startTime();};
        void setJob_endTime(){Container_JobResults->setJob_endTime();};

        simtime_t getJob_startTime(){return Container_JobResults->getJob_startTime();};
        simtime_t getJob_endTime(){return Container_JobResults->getJob_endTime();};

        void setOriginalName(string name){originalName = name;};
        string getOriginalName(){return originalName;};
        void setNumCopies(int copies){numCopies = copies;};
        int getNumCopies(){return numCopies;};

        int getCommId(){return commId;};
        void setCommId(int commId_){commId = commId_;};

        int getJobId(){return this->getId();};
        double getTimeToStart();

};

#endif /* Container_jobBase */
