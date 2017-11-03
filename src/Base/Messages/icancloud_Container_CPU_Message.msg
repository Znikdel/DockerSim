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
/// icancloud Message for container CPU requests
///
message icancloud_Container_CPU_Message extends icancloud_Message
{
    @customize(true);

    simtime_t cpuTime = 0.0;				// CPU execution time
    unsigned long totalMIs = 0;			// Number of instructions to be executed.
    unsigned long remainingMIs = 0;		// Remaining instructions to be executed.
    int quantum = 0;					// Quantum used in Round-Robin algorithm (measured in ticks)
    int cpuPriority = 0;					// Priority 
}



