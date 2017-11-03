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

#ifndef ABSTRACT_REMOTE_FS_H_
#define ABSTRACT_REMOTE_FS_H_


#include "icancloud_Message.h"
#include "icancloud_App_IO_Message.h"

class Abstract_Remote_FS {

	protected:

		int pId;		/** Connection identifier*/

	    struct connection{
			string destAddress;			/** Destination address. */
			int destPort;				/** Local port. */
			int	connectionID;
	    };
	    typedef struct connection connect;

	    vector <connect*> connections;

		string netType;

		int dataSize_KB;			/** cell data: requestSize for NFS and strideSize for PFS */

	    bool active;

	    vector <icancloud_Message*> requests_queue;

		unsigned int numSettingServers;	/** Number of servers */
	    int numServersForFS;

	public:

	    Abstract_Remote_FS();
		virtual ~Abstract_Remote_FS();

		void clean_cell();

	    void setCellID (int Id);
	    int getCellID ();

	    void setConnection(string destAddress_var, int destPort, string netType_, int connectionId);
	    void setConnectionId(int connectionId,int index_connection);

	    string getDestAddress (int index_connection);
	    int getDestPort (int index_connection);
	    int getConnectionId(int index_connection);
	    string getNetType ();

	    void setDataSize_KB (int RequestSize_KB);
	    int getDataSize_KB ();

	    int getNumServers(){return numServersForFS;};

	    void activeCell ();
	    void deactiveCell ();
	    bool isActive ();

	    int getServerCellPosition (string destAddress_, int destPort_, int connectionID_, string netType_);
	    int getServerCellPosition (icancloud_App_IO_Message* sm);

	    bool existsConnection (string destAddress_, int destPort_);

	    bool equalCell (int serverCellPosition, string destAddress_, int destPort_,
						int connectionID_, string netType_);

	    void setNumServersForFS(int num){numServersForFS = num;};
	    bool allServersSetted(){return ((int)numSettingServers == 0);};
	    void incrementSettedServers(){numSettingServers ++;};

	    void enqueueRequest(icancloud_Message* request);
	    icancloud_Message* popEnqueueRequest();
	    bool hasPendingRequests();
	    int getPendingRequestsSize ();
public:

	    virtual int getServerToSend(icancloud_Message* sm) = 0;
	    virtual icancloud_App_IO_Message* hasMoreMessages() = 0;
	    virtual void initializeCell() = 0;
	    virtual bool splitRequest (cMessage *msg) = 0;
	    virtual icancloud_App_IO_Message* popSubRequest () = 0;
	    virtual icancloud_Message* popNextSubRequest (cMessage* parentRequest) = 0;
	    virtual void arrivesSubRequest (cMessage* subRequest, cMessage* parentRequest) = 0;
	    virtual cMessage* arrivesAllSubRequest (cMessage* request) = 0;
	    virtual bool removeRequest (cMessage* request) = 0;

};

#endif /* ABSTRACT_REMOTE_FS_H_ */
