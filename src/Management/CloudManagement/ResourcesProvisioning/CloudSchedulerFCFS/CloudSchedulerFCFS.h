
/**
 * @class CloudSchedulerFSFC- CloudSchedulerFSFC.h CloudSchedulerFSFC.cc
 *
 * This is an scheduler First come first serve.
 * The first request that arrives at scheduler is the first that will be served.
 *
 * @authors: Gabriel Gonz&aacute;lez Casta&ntilde;&eacute
 */

#ifndef SCHEDULER_FSFC_H_
#define SCHEDULER_FSFC_H_

#include "AbstractCloudScheduler.h"

class CloudSchedulerFCFS : public AbstractCloudScheduler{

	protected:

		double minimum_percent_storageNode_free;
		int maximum_number_of_processes_per_node;

         //Flags To control the energy printing
             bool printNodePowerConsumption;        // print Node instant consumption
             bool printNodeEnergyConsumed;          // print Node energy consumed
             bool printComponentsPowerConsumption;  // print main subsytems instant consumption
             bool printComponentsEnergyConsumed;    // print main subsytems instant consumption
             bool printDataCenterPowerConsumption;  // print all the data-center energyConsumed
             bool printDataCenterEnergyConsumed;    // print all the data-center instant consumption
             bool printDockerResult;
             double dc_EnergyConsumed;  // For storing the total data center energy consumed.


    virtual void initialize();

 // -------------------------------------------------------------------------------
 // --------------------- implementation of virtual methods -----------------------
 // -------------------------------------------------------------------------------

        /*
         * Initialization of internal structures defined by user at schuler.
         */
		void setupScheduler();

        /*
         * This method is the scheduling core. Depending on the operation of
         * the request (USER_START_VMS, USER_START_VMS_REENQUEUE, USER_REMOTE_STORAGE, USER_LOCAL_STORAGE, USER_SHUTDOWN_VM)
         * The scheduling invokes different methods.
         * This method block the incoming of requests, deriving them to a temporal queue until this method finishes.
         * The time between the echeduling events are setted by time_between_scheduling_events as an alarm at AbstractCloudManager.
         * If this interval is too short, it is possible that it produces starving.
         */
        void schedule ();

        /*
         * This method returns the node where the virtual machine given as parameter (vm) is going to be allocated.
         */
		AbstractNode* selectNode (AbstractRequest* req);

        /*
         * This method returns a set of node(s) that vm's is going to use for
         * storaging data. The parameter fsType is for selecting NFS, PVFS, or another type of
         * file system requested by user
         */
        vector<AbstractNode*> selectStorageNodes (AbstractRequest* st_req);

        /*
         * This method returns true if the scheduler decides to shutdown the node after the vm closing.
         * It only will switch off the node if the vm as parameter is the only one at the node
         */
        bool shutdownNodeIfIDLE(){return true;};

        /*
         *  This method is activated when a shutdown request is going to be executed. It has to return a
         *  vector with the nodes where the vm has remote storage, or an empty vector if it only has
         *  local storage. It is the scheduler responsibility, the managing and control of where are the vms allocated
         *  and which nodes it is using for remote storage.
         */
        vector<AbstractNode*> remoteShutdown (AbstractRequest* req);

        /*
         * This method is invoked when a virtual machine has finalized.
         */
		void freeResources (int uId, int pId, AbstractNode* computingNode);

        /*
         * This method defines the data that will be printed in 'logName' file at 'OUTPUT_ DIRECTORY'
         * if the printEnergyToFile and printEnergyTrace are active
         */
		void printEnergyValues();
		void printDockerResults();

		/*
		 *  This method is invoked before to finalize the simulation
		 */
		void finalizeScheduler();

  private:

		/*
		 * This method, selects the heterogeneous node set that is able to allocate an amount of memory and a set of cores..
		 */
		int selectNodeSet  (string setName, int vmcpu, int vmmemory);



};

#endif
