//
// Class AbstractStorageCell
//
// A storage cell is responsible for the remote storage system.
// Abstract methods:
//    virtual void processSelfMessage (cMessage *msg);
//    virtual void processRequestMessage (icancloud_Message *sm);
//    virtual void processResponseMessage (icancloud_Message *sm);
//    virtual bool setRemoteData (icancloud_App_NET_Message* sm_net);
//
// @author Gabriel Gonz&aacute;lez Casta&ntilde;&eacute
// @date 2012-10-23
//
#ifndef ABSTRACTSTORAGECELL_H_
#define ABSTRACTSTORAGECELL_H_

#include <vector>
#include "icancloud_Base.h"
#include "Abstract_Remote_FS.h"

using std::vector;


class AbstractStorageCell : public icancloud_Base {

protected:

    int uId;                    // id for User
    double storageSizeGB;       // Storage size available for a user
	string virtualIP;           // Virtual IP of suurce process
	int vmTotalBlocks_KB;       // total blocks available of the VM
	int remainingBlocks_KB;     // remaining free blocks at VM

	bool remoteStorage;         // If the remote storage (NFS, PFS, ..) is available
	int numStandardRequests;    // The number of standard requests sent
	int numDeleteRequests;      // The number of delete requests sent
	int remoteStorageUsed;      // The remote storage used.

	vector<Abstract_Remote_FS*> remote_storage_cells;   // A vector composed by remote storage cells that represents different remote fs configured at vm

	//remoteParameters
		string fs_type;                 // NFS || PFS
		int nfs_requestSize_KB;         // NFS parameter
		int pfs_strideSize_KB;          // PFS parameter

	// GATES
		cGate* from_H_StorageManager;
		cGate* to_H_StorageManager;


protected:
	AbstractStorageCell();
	void init (int newNodeGate);

	virtual ~AbstractStorageCell();

	void finish();

	virtual void initialize();

	/**
	* Get the outGate to the module that sent <b>msg</b>
	* @param msg Arrived message.
	* @return. Gate Id (out) to module that sent <b>msg</b> or NOT_FOUND if gate not found.
	*/
	cGate* getOutGate (cMessage *msg);

//	virtual void finish() = 0;

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
	* Process a response message.
	* @param sm Request message.
	*/
	virtual void processResponseMessage (icancloud_Message *sm) = 0;

	/*
	 * This method sets the remote data from a fiven network message.
	 */
	virtual bool setRemoteData (icancloud_App_NET_Message* sm_net) = 0;

	void setVirtualIP(string vip);

	void setTotalStorageSize(int blocks_KB);
	void incTotalStorageSize(int blocks_KB);
	void setRemainingStorageSize(int blocks_KB);
	void setRemoteStorage (bool remote);

	void setCell (int uId, string vip, int diskSize);

	bool active_remote_storage(int pId, string destAddress, int destPort, int connectionID, string netType);

	string getVirtualIP();
	int getTotalStorageSize();
	int getRemainingStorageSize();
	bool hasRemoteStorage();

	Abstract_Remote_FS* getRemoteStorage (string destAddress, int destPort, int connectionID, string netType);
	Abstract_Remote_FS* getRemoteStorage_byPosition (unsigned int position);

	bool existsRemoteCell  (string destAddress, int destPort,
							int connectionID, string netType);
	int getRemoteCellPosition (int pId);

	bool isActiveRemoteCell (string destAddress, int destPort, int serverID, string netType);

	int incRemainingVMStorage(int size);
	int decRemainingVMStorage(int size);

	void freeRemainingVMStorage();

	//Remote storage values
	int getStandardRequests();
	int getDeleteRequests();
	void initStandardRequests();
	void initDeleteRequests();

	void setStandardRequests(int number);
	void setDeleteRequests(int number);

	void setRemoteStorageUsed(int size);
	int getRemoteStorageUsed();

	void incStandardRequests();
	void incDeleteRequests();
	void incRemoteStorageUsed(int size);
	void decRemoteStorageUsed(int size);

		// For migrating vms
		int get_remote_storage_cells_size();
		Abstract_Remote_FS* get_remote_storage_cell (int index);
		vector<Abstract_Remote_FS*> get_remote_storage_vector();
		void initialize_remote_storage_vector();
		void set_remote_storage_vector(vector<Abstract_Remote_FS*> cells);

	// Setters and getters of attrributes
		void set_num_waiting_connections (int num, int id);
		bool dec_num_waiting_connections (int pId);

		void set_maxConnections (int pId, int num);
		int get_maxConnections (int pId);

		int get_nfs_requestSize_KB();
		int get_pfs_strideSize_KB();

	void insert_remote_storage_cells(Abstract_Remote_FS* cell);

	void sendRemoteRequest (icancloud_App_IO_Message* nextSubRequest, Abstract_Remote_FS* remote_storage_cell);

public:

	void connectGates(cGate* oGate, cGate* iGate );
};

#endif /* BS_CELL_H_ */
