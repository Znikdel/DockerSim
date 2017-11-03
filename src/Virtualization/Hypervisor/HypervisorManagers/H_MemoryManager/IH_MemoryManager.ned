//
// Module interface that defines the behavior of a Memory hypervisor controller.
//
// @author Gabriel Gonz&aacute;lez Casta&ntilde;&eacute
// @date 2012-10-23
//
package icancloud.src.Virtualization.Hypervisor.HypervisorManagers.H_MemoryManager;

moduleinterface IH_MemoryManager {

	parameters:
		double blockSize_KB;
		double memorySize_MB;
		
		double memoryOverhead_MB;
    gates:
		input   fromVMMemoryI[];
        input   fromVMMemoryO[];
        output  toVMMemoryI[];
        output  toVMMemoryO[];
        
        input   fromNodeMemoryI;
        input   fromNodeMemoryO;
        output  toNodeMemoryI;
        output  toNodeMemoryO;
}

