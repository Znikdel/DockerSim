cplusplus {{

	#include "icancloud_Message.h"
			
}}


class icancloud_Message;


///
/// icancloud Message for IO requests
///
message icancloud_App_IO_Message extends icancloud_Message
{
    @customize(true);

    string fileName = "";		// File name
    unsigned int offset = 0;	// Offset		
    unsigned int size = 0;		// Request size
    bool isPending = false;		// Is current request pending?			
    string nfs_destAddress = "";
    int nfs_destPort = - 1;
    int nfs_id = - 1;
    string nfs_type = "";
    int nfs_requestSize_KB = -1;
    int nfs_connectionID = -1;
}
