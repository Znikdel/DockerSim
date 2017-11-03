//
// This class defines a Manager responsible for translate virtual ports and virtual ip address to real physical ports and ip address.
//
// @author Gabriel Gonz&aacute;lez Casta&ntilde;&eacute
// @date 2012-10-23
//


#ifndef LOCALNET_MANAGER_H_
#define LOCALNET_MANAGER_H_

#include "PortAddressTranslation.h"
#include "NetworkManager.h"

#define ROOT_PORT 0                     /** Root port */
#define REGISTERED_INITIAL_PORT 1024    /** Initial Registered port number */
#define DYNAMIC_INITIAL_PORT 49152      /** Initial dynamic port number */
#define LAST_PORT 65535                 /** Last dynamic port number */

class LocalNetManager : public icancloud_Base {

protected:

	PortAddressTranslation* pat;
	string ip_LocalNode;

	NetworkManager* netManagerPtr;
public:

	virtual ~LocalNetManager();

	void initialize();

	void finish();
   /**
	* Get the out Gate to the module that sent <b>msg</b>.
	* @param msg Arrived message.
	* @return Gate (out) to module that sent <b>msg</b> or NULL if gate not found.
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


	void initializePAT (string nodeIP);

	void createVM(icancloud_Message* sm);

	/*
	 *  Create an entry into the ports connections
	 */
	void manage_listen(icancloud_Message* sm);

	/*
	 * This method change a connection creation message from virtual layer to physical layer translating
	 * the ip addresses virtual to physical. The message operation is to connect this node with a storage node
	 */
	int manage_create_storage_Connection(icancloud_Message* sm);

	/*
	 * This method change a connection creation message from virtual layer to physical layer translating
     * the ip addresses virtual to physical
     */
	int manage_createConnection(icancloud_Message* sm);

	/*
	 *  When the connection is stablished, the client receives a port to the connection
	 *  that has to be allocated into the structure
	 */
	void connectionStablished(icancloud_Message* sm);

	/*
	 * Erase all the data from the structures of a virtual machine..
	 */
	vector<icancloud_App_NET_Message*> manage_close_connections(int uId, int pId);

	/*
	 * Translate from real to virtual port
	 */
	void manage_receiveMessage(icancloud_Message* sm);

	/*
	 * Translate from virtual to real port
	 */
	void manage_sendMessage(icancloud_Message* sm);

	/*
	 * Erase the data from the structures associated to a connection
	 */
	void manage_close_single_connection(icancloud_Message* sm);


private:

	/*
	 * Get all the connections id from the pat
	 */
		vector<int> getConnectionsIDs(int uId, int pId);

};

#endif /* USER_VIRTUALPORT_CELL_H_ */
