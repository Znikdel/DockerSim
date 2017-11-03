cplusplus {{
	#include "icancloud_App_IO_Message.h"
	#include "icancloud_File.h"					
}}


//struct fileBranch;
class icancloud_App_IO_Message;

class noncobject icancloud_File;

///
/// icancloud Message for disk requests (branch list)
///
message icancloud_BlockList_Message extends icancloud_App_IO_Message
{
    @customize(true);

    icancloud_File file;		// Branch list
}

