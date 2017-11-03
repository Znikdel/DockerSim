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
/// icancloud Message for container IO requests
///
message icancloud_Container_IO_Message extends icancloud_Message
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
