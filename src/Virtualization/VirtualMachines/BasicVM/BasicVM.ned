//
// This module defines a Concrete Virtual Machine. 
//
// A virtual machine is defined to iCanCloud simulator as a type of machine without physical resources.
// The physical resources are managed by hypervisor and the virtual machine is linked to the hypervisor in order
// to perform those tasks.
//
// @author Gabriel Gonz&aacute;lez Casta&ntilde;&eacute
// @date 2012-10-23
//

package icancloud.src.Virtualization.VirtualMachines.BasicVM;

import icancloud.src.Architecture.NodeComponents.OperatingSystems.IOperatingSystem;
import icancloud.src.Virtualization.VirtualMachines.VM;
import icancloud.src.Virtualization.VirtualMachines.IVM;

module BasicVM extends VM like IVM
{
    parameters:
        @labels(vm,ethernet-vm);
        @display("i=old/telnet");
		@loose;
    submodules:

        osModule: <osModuleType> like IOperatingSystem {
            parameters:
                numCPUs = numCores;
                numStorageSystems = numStorageSystems;

                @display("p=108,64");
            gates:
                fromApps[];
                toApps[];
                fromCPU[numCores];
                toCPU[numCores];
                fromStorageSystem[numStorageSystems];
                toStorageSystem[numStorageSystems];
                //***/
                fromInputMemory;   				 // Input gates from inputs
                fromOutputMemory;					 // Input gate from output
                toInputMemory;					 // Output gates to inputs
                toOutputMemory;						 // Output gate to output

                fromNet_TCP;						 // Input gate from Network (Ethernet TCP)
                toNet_TCP;							 // Output gate to Network (Ethernet TCP)
                
                fromDockerEngine[];
        		toDockerEngine[];
        		
        //		fromOSContainers[];
        //		toOSContainers[];
                
        }     

    connections allowunconnected:

 		// Connections between CPU and Operating System

        for i=0..numCores-1 {
            fromNodeCPU[i] --> osModule.fromCPU[i];
            toNodeCPU[i] <-- osModule.toCPU[i];

        }

        // Connections between Block Servers and Operating System

        for i=0..numStorageSystems-1 {
            fromNodeStorageSystem[i] --> osModule.fromStorageSystem[i];
            toNodeStorageSystem[i] <-- osModule.toStorageSystem[i];
        }

        osModule.fromInputMemory <-- fromNodeMemoryI;
        osModule.toInputMemory --> toNodeMemoryI;

        osModule.fromOutputMemory <-- fromNodeMemoryO;
        osModule.toOutputMemory --> toNodeMemoryO;

		// Connection between network and Operating System

        osModule.fromNet_TCP <-- fromNodeNet;
        osModule.toNet_TCP --> toNodeNet;
        
//        osModule.fromOSContainers++ <-- fromOSContainers++;
//        osModule.toOSContainers++ --> toOSContainers++;
    
//    osModule.fromDockerEngine++ <-- fromDockerEngine++;
//    osModule.toDockerEngine++ --> toDockerEngine++;
		
		
}
