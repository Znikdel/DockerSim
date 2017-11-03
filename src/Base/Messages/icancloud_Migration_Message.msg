cplusplus {{

	#include "icancloud_App_NET_Message.h"
	#include <vector>
	
}}


class noncobject VmPtr;


class icancloud_App_NET_Message;

//
// icancloud Message for migration requests
//
message icancloud_Migration_Message extends icancloud_App_NET_Message
{
    @customize(true);
    
    int memorySizeKB = 0;
    int diskSizeKB = 0;
    string virtualIP = "";
    
	bool remoteStorage = false;
	int standardRequests = 0;
	int deleteRequests = 0;
	int remoteStorageUsedKB = 0;
	
}

