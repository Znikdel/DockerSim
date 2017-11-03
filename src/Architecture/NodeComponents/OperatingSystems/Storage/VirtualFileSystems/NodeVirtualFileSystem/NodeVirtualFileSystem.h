#ifndef __VIRTUAL_FILE_SYSTEM_H_
#define __VIRTUAL_FILE_SYSTEM_H_

#include <omnetpp.h>
#include "icancloud_Base.h"
#include "icancloud_App_IO_Message.h"
using std::vector;


/**
 * @class NodeVirtualFileSystem NodeVirtualFileSystem.h "NodeVirtualFileSystem.h"
 *
 * Class that represents a I/O Redirector
 *
 * This I/O Redirector contains information about the redirections to the 
 * corresponding FS or to a corresponding APP module. (See FileConfigManager module for a more
 * detailed information about the config file).
 *
 * This module contains a table with paths. Each one of these paths is associated
 * to an index and a module type. This index correspond to a FS in the node's FS 
 * array or to an APP in node's APP array.
 *
 * Local operations are sent to local File Systems: Parameter remoteOperation=false
 * Remote operations are sent to corresponding client application: Parameter remoteOperation=true
 *
 * This class is a copy of VirtualFileSystem supporting management of tenants
 *
 * @author updated to iCanCloud by Gabriel González Castañé
 * @date 2012-05-17
 */
class NodeVirtualFileSystem :public icancloud_Base{

	/**
	 * Structure that represents a path table entry.
	 */
	struct PathEntry{
		string type;				/**< Destination module type (DEST_FS or DEST_APP) */
		string path;				/**< Path */
		unsigned int index;			/**< Connection ID */		
	};
	typedef struct PathEntry pathEntry;

	struct uPathEntry{
		int userID;
		vector <pathEntry> pathTable;
	};

	protected:
	  	
	    /** Number of File Systems. */
	    int numFS;
	    
	    /** storage app client index */
	    int storageClientIndex;

	    /** Just the table per user. */
	    vector <uPathEntry> userPathTable;

		/** Input Memory from Cache. */
		cGate* fromMemoryGate;

		/** Output Memory to Cache. */
		cGate* toMemoryGate;

		/** Input gatess from File Systems. */
		cGate** fromFSGates;

		/** Output gates to File Systems. */
		cGate** toFSGates;


	   /**
		* Destructor
		*/
		~NodeVirtualFileSystem();


	   /**
	 	* Module initialization.
	 	*/
	    void initialize();

	   /**
	 	* Module ending.
	 	*/
	    void finish();


	private:

	   /**
		* Get the outGate to the module that sent <b>msg</b>
		* @param msg Arrived message.
		* @return. Gate Id (out) to module that sent <b>msg</b> or NOT_FOUND if gate not found.
		*/
		cGate* getOutGate (cMessage *msg);

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
		* Process a response message.
		* @param sm Request message.
		*/
		void processResponseMessage (icancloud_Message *sm);

		/*
		 * Delete the user and the paths from the userPathTable
		 */
		void deleteUser (int user);

	   /**
		* Parse the fs Table contents to a string
		* @return fs Table in string format.
		*/
		string fsTableToString ();

};

#endif


