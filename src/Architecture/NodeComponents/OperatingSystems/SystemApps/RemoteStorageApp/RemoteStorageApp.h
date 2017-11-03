#ifndef __REMOTE_STORAGE_APP_H
#define __REMOTE_STORAGE_APP_H_
//
#include "icancloud_Base.h"

/**
 * @class RemoteStorage RemoteStorage.cc "RemoteStorage.h"
 *
 * This application is an application system managed by syscall manager in order to create connections
 * between compute nodes and remote storage servers. In addition it is also in charge of load tenant
 * files from preloadfiles (files that is going to be loaded before an user application execution) and the
 * file system structure.
 *
 * @author Gabriel Gonz&aacute;lez Casta&ntilde;&eacute;
 * @author @date 2013-03-04
 */

class RemoteStorageApp : virtual public icancloud_Base {

	protected:			

	/** Input gate from OS. */
	cGate* fromOSGate;

	/** Output gate to OS. */
	cGate* toOSGate;

	cGate* fromOSfromConGate;
	cGate* toOStoConGate;

	cMessage* selfProcessingMsg;

	struct preloadCreation{
		int uId;
		int pId;
		int spId;
		int numberOfIor;
		vector<icancloud_App_IO_Message*> sm_files;
	};

	vector <preloadCreation*> sm_files_creation;
	vector <preloadCreation*> sm_ior_creation;

    int localStoragePort;

    string storageConnectionType;

    struct listen_repetition_t{
        int uId;
        int pId;
    };

    typedef listen_repetition_t listen_repetition;

    vector<listen_repetition*> lrep;


	   /**
		* Destructor
		*/
		~RemoteStorageApp();

	   /**
 	 	*  Module initialization.
  		*/
	    void initialize();

	   /**
		* Module ending.
		*/
		void finish();

	   /**
		* Process a self message.
		* @param msg Self message.
		*/
		void processSelfMessage (cMessage *msg);

	   /**
		* Process a request message.
		* @param sm Request message.
		*/
		void processRequestMessage (icancloud_Message *sm);

	   /**
		* Process a response message from a module in the local node. Empty method.
		* All response messages are computed by TCPSocket::CallbackInterface
		* @param sm Request message.
		*/
		void processResponseMessage (icancloud_Message *sm);
		
		/**
		* Get the out Gate to the module that sent <b>msg</b>.
		* @param msg Arrived message.
		* @return Gate (out) to module that sent <b>msg</b> or NULL if gate not found.
		*/
		cGate* getOutGate (cMessage *msg);


		/**
		 * This method set the established connection.
		 *
		 * This method must be invoked when the response of a SM_CREATE_CONNECTION operation arrives.
		 * The response message will contain the corresponding connection descriptor stored in parameter communicationId.
		 *
		 * @param sm Response message.
		 */
		void setEstablishedConnection (icancloud_App_NET_Message *sm_net);

		/**
		 * Process a NET call response
		 */
		void processNetCallResponse (icancloud_App_NET_Message *sm_net);


		/**
		 * Process an I/O call response
		 */
		void processIOCallResponse (icancloud_App_IO_Message *sm_io);

	// ----------------- --------------------------- ----------------- //
	// ----------------- Operations with connections ----------------- //
	// ----------------- --------------------------- ----------------- //


	public:

		void closeConnection (int uId, int pId);

		void initialize_storage_data(int localPort);

		int getStoragePort(){return localStoragePort;};

		void create_listen (int uId, int pId);


	// ----------------- -------------- ----------------- //
	// ----------------- Remote Storage ----------------- //
	// ----------------- -------------- ----------------- //

		void connect_to_storage_node(vector<string> destinationIPs, string fsType, int destPort, int uid, int pid, string vIP, int jobId);

		void createFilesToPreload(int uid, int pid, int spId, string vmIP, vector<preload_T*> filesToPreload, vector<fsStructure_T*> fsPaths, bool remoteHost);

		void deleteUserFSFiles(int uid, int pid);

	protected:

		void setDataStorage_EstablishedConnection (icancloud_App_NET_Message *sm_net);

};

#endif
