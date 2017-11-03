//
// class Storage_cell_basic.h defines a generic storage cell
//
// A storage cell is responsible for the remote storage system.
//
// @author Gabriel Gonz&aacute;lez Casta&ntilde;&eacute
// @date 2012-10-23
//


#ifndef STORAGE_CELL_BASIC_H_
#define STORAGE_CELL_BASIC_H_

#include "AbstractStorageCell.h"
#include "NFS_Storage_Cell.h"
#include "PFS_Storage_Cell.h"

class Storage_cell_basic : public AbstractStorageCell {

protected:

	icancloud_App_IO_Message* pendingMessage;

public:

	virtual ~Storage_cell_basic();

	virtual void initialize();
   /**
	* Process a self message.
	* @param msg Self message.
	*/
	virtual void processSelfMessage (cMessage *msg);

   /**
	* Process a request message.
	* @param sm Request message.
	*/
	virtual void processRequestMessage (icancloud_Message *sm);

   /**
	* Process a response message.
	* @param sm Request message.
	*/
	virtual void processResponseMessage (icancloud_Message *sm);


	bool setRemoteData (icancloud_App_NET_Message* sm_net);

private:
	struct replica_T{
	    int commId;
	    int uId;
	    int pId;
	    int machinesleft;
	    icancloud_App_NET_Message* sms;
	};

	typedef replica_T replica_t;

	vector<replica_t*> replicas;

};

#endif /* STORAGE_CELL_BASIC_H_ */
