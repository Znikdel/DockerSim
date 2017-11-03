 /*
  * This class implements a class for a message redirector class and is responsible for operating system functionalities.
  * It controls the amount of memory, storage of the base system, the quantity of applications linked to the machine,
  * and linking and unlinking tenant processes to the system. Also manages states changes of system and the remote storage
  * requests (ioManager) methods
  * .
  * @author - updated to iCanCloud by Gabriel González Castañé
  * @date 2012-05-17
  */
#ifndef __SYSCALLMANAGER_H_
#define __SYSCALLMANAGER_H_

#include "AbstractSyscallManager.h"
#include "StatesApplication.h"
#include "RemoteStorageApp.h"
#include "AbstractNode.h"

class AbstractNode;

class SyscallManager : virtual public AbstractSyscallManager{

protected:

    StatesApplication* statesAppPtr;                   // Pointer to the states application
    RemoteStorageApp* ioManager;                       // Pointer to the io manager

    /** Gate for remote storage application */
    int remoteStorageGate;

    /** gate for states application */
    int statesAppGate;

    AbstractNode* nodePtr;


	   /**
	    * Destructor.
	    */    		
	    ~SyscallManager();
	  	        			  	    	    
	   /**
	 	*  Module initialization.
	 	*/
	    void initialize();
	    
	   /**
	 	* Module ending.
	 	*/ 
	    void finish();

	   /**
		* Process a request message.
		* @param sm Request message.
		*/
		void processRequestMessage (icancloud_Message *sm);


public:

        /*
         * This method removes from the control structure the process using the parameters to search it
         */
		void removeProcess(int pid);

		/*
		 * This method create a process at the system linking it to the system
		 */
		int createProcess(icancloud_Base* job, int uid);

        /*
         *  This method initialize the storage port for the remote storage nodes and the initial state.
         */
        void initializeSystemApps(int storagePort, string state);


        /*******************************************************************************
         *                          Operations with storage
         *******************************************************************************/

        /*
         *  This method closes the connections of the user uId, process pId
         */
        void closeConnection (int uId, int pId){ioManager->closeConnection(uId, pId);};

        /*
         * This method returns the local port for connecting the remote storage connections
         */
        int getRemoteStoragePort(){return ioManager->getStoragePort();};                       // To operate with remote storage port


        /*
         * This method allow the connection from a compute node to a storage node. It is invoked by the cloud manager and the responsible
         * of the data storage connection is the remote storage app allocated at target (data storage) node.
         */
        void connectToRemoteStorage (vector<string> destinationIPs, string fsType, int localPort, int uId, int pId, string ipFrom, int jobId){ioManager->connect_to_storage_node (destinationIPs, fsType, localPort, uId,pId, ipFrom.c_str(), jobId);};

        /*
         * This method creates a listen connection in the data storage node, in order to wait until the compute node (which hosts
         * the vm) completes the connection. Host node requests a 'remote storage connection' against the storageLocalPort of the storage node.
         */
        void createDataStorageListen (int uId, int pId){ioManager->create_listen (uId, pId);};

        /*
         * This method load remote files and configure the filesystems for the account of the user into the remote storage nodes selected by the scheduler of the cloud manager
         */
        void setRemoteFiles ( int uId, int pId, int spId, string ipFrom, vector<preload_T*> filesToPreload, vector<fsStructure_T*> fsPaths){ioManager->createFilesToPreload(uId,pId,spId, ipFrom, filesToPreload, fsPaths, true);};

        /*
         * This method delete the fs and the files from the remote storage nodes that have allocated files from him.
         */
        void deleteUserFSFiles( int uId, int pId){ioManager->deleteUserFSFiles(uId, pId);};

        /*
         * This method create files into the local filesystem of the node.
         */
        void createLocalFiles (int uId, int pId, int spId, string ipFrom, vector<preload_T*> filesToPreload, vector<fsStructure_T*> fsPaths){ioManager->createFilesToPreload(uId,pId,spId, ipFrom, filesToPreload, fsPaths, false);};

        /*
         *  This method returns the state of the node
         */
        string getState (){return statesAppPtr->getState();};

        /*
         *  This method changes the state of the node calling to the states application
         */
        void changeState(string newState);

        /*
         * Setter for the manager
         */
        void setManager(icancloud_Base* manager){nodePtr = check_and_cast<AbstractNode*>(manager);};

private:

        /*
         * This method notify to node that an event has been executed
         */
        void notifyManager (icancloud_Message* sms);

};

#endif
