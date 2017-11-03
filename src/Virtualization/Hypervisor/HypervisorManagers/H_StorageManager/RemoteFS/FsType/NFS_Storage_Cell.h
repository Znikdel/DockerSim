//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef NFS_CELL_H_
#define NFS_CELL_H_

#include "Abstract_Remote_FS.h"
#include "SMS_NFS.h"

class NFS_Storage_Cell : public Abstract_Remote_FS{

	protected:

	    SMS_NFS *SMS_nfs;			/** SplittingMessageSystem Object*/

	public:

	    NFS_Storage_Cell();
	    ~NFS_Storage_Cell();

	    virtual int getServerToSend(icancloud_Message* sm);
	    virtual icancloud_App_IO_Message* hasMoreMessages();
	    virtual void initializeCell();
	    virtual bool splitRequest (cMessage *msg);
	    virtual icancloud_App_IO_Message* popSubRequest ();
	    virtual icancloud_Message* popNextSubRequest (cMessage* parentRequest);
	    virtual void arrivesSubRequest (cMessage* subRequest, cMessage* parentRequest);
	    virtual cMessage* arrivesAllSubRequest (cMessage* request);
	    virtual bool removeRequest (cMessage* request);

};

#endif /* NFS_CELL_H_ */
