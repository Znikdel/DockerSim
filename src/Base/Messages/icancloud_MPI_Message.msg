cplusplus {{

	#include "icancloud_App_NET_Message.h"
			
}}


class icancloud_App_NET_Message;


///
/// icancloud Message for MPI calls
///
message icancloud_MPI_Message extends icancloud_App_NET_Message
{
    @customize(true);

    unsigned int sourceRank = 0;	// Rank of the source process
    unsigned int destRank = 0;		// Rank of the destination process
    string fileName = "";			// File name
    int offset = 0;					// Offset				
}
