cplusplus {{

	#include "icancloud_Message.h"
			
}}


class icancloud_Message;


///
/// icancloud Message for MEM requests
///
message icancloud_App_MEM_Message extends icancloud_Message
{
    @customize(true);

    unsigned int memSize = 0;		// Amount of memory
    unsigned int region = 0;		// Region of memory
}
