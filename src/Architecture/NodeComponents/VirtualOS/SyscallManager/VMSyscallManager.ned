// Module that implements a Generic Service Redirector.
//
// This module redirects the requests to the corresponding module:
//	 - Network Service for remote requests.
//	 - CPU Service for local computing requests.
//	 - I/O Service for local data requests .
//
//
// Operation list:
//
// - I/O Requests
//
// - <b>Open a file</b><br>
//		- MessageName: icancloud.src.App_IO_Message <br>
// 		- Operation: SM_OPEN_FILE<br>
//
// - <b>Close a file</b><br>
//		- MessageName: icancloud.src.App_IO_Message <br>
// 		- Operation: SM_CLOSE_FILE<br>
//
// - <b>Read a file</b><br>
//		- MessageName: icancloud.src.App_IO_Message <br>
// 		- Operation: SM_READ_FILE<br>
//
// - <b>Write a file</b><br>
//		- MessageName: icancloud.src.App_IO_Message <br>
// 		- Operation: SM_WRITE_FILE<br>
//
// - <b>Creaate a file</b><br>
//		- MessageName: icancloud.src.App_IO_Message <br>
// 		- Operation: SM_CREATE_FILE<br>
//
// - <b>Delete a file</b><br>
//		- MessageName: icancloud.src.App_IO_Message <br>
// 		- Operation: SM_DELETE_FILE<br>
//
//
//
// @author - cleanup and rewrited by Gabriel González Castañé -
// @date october 2014


package icancloud.src.Architecture.NodeComponents.VirtualOS.SyscallManager;

import icancloud.src.Architecture.NodeComponents.OperatingSystems.SyscallManager.ISyscallManager;

simple VMSyscallManager like ISyscallManager

{
    gates:
        input fromApps[];				// Input gates from Applications
        input fromDockerEngine[];				// Input gates from Applications
   //     input fromOSContainers[];
        
        input fromMemory;				// Input gate from Memory
        input fromNet;					// Input gate from Network
        input fromCPU;					// Input gate from CPU

        output toApps[];				// Output gates to Applications
        output toDockerEngine[];
 //       output toOSContainers[];
        
        output toMemory;				// Output gate to Memory
        output toNet;					// Output gate to Network
        output toCPU;	

}
