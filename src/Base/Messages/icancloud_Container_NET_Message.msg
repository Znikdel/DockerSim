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
cplusplus {{

	#include "icancloud_Message.h"
			
}}


class icancloud_Message;


///
/// icancloud Message for Container NET requests
///
message icancloud_Container_NET_Message extends icancloud_Message
{
    @customize(true);

    string destinationIP = "";		// Destination IP
    string localIP = "";			// Local IP
    int destinationPort = 0;		// Destination port
    int localPort = 0;				// Local port
    string connectionType = "";		// Connection type
    unsigned int size = 0;          // Request size
    string data = "";          		// Data sended
    string virtual_destinationIP = "";
    string virtual_localIP = "";
    int virtual_user = -1;
    int virtual_destinationPort = 0;		// Virtual Destination port
    int virtual_localPort = 0;				// Virtual Local port
	string fsType = "";						// Type of remote file system
	int targetPosition = 1;					// Target Position of remote file system
}
