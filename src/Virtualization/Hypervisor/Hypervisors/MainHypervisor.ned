//
// This module defines a Main Hypervisor
//
// Each VM is linked inside this module to each controller.
// The hypervisor has four main controllers. Each controller is responsible for a physical resource: CPU, memory, network and storage.
//
// @author Gabriel Gonz&aacute;lez Casta&ntilde;&eacute
// @date 2012-10-23
//


package icancloud.src.Virtualization.Hypervisor.Hypervisors;

import icancloud.src.Virtualization.Hypervisor.HypervisorManagers.H_CPUManager.IH_CPUManager;
import icancloud.src.Virtualization.Hypervisor.HypervisorManagers.H_CPUManager.Schedulers.**.*;
import icancloud.src.Virtualization.Hypervisor.HypervisorManagers.H_MemoryManager.IH_MemoryManager;
import icancloud.src.Virtualization.Hypervisor.HypervisorManagers.H_MemoryManager.Schedulers.**.*;
import icancloud.src.Virtualization.Hypervisor.HypervisorManagers.H_NetworkManager.Managers.H_NETManager;
import icancloud.src.Virtualization.Hypervisor.HypervisorManagers.H_NetworkManager.Schedulers.**.*;
import icancloud.src.Virtualization.Hypervisor.HypervisorManagers.H_NetworkManager.NetworkServices.INetworkService;
import icancloud.src.Virtualization.Hypervisor.HypervisorManagers.H_StorageManager.H_StorageManager.H_StorageManagerModule;
import icancloud.src.Virtualization.Hypervisor.IHypervisor;

module MainHypervisor like IHypervisor
{
    parameters:
        @class("Hypervisor");
        @display("i=energy/Hypervisor_40;p=208,236");
        
        int numStorageServers; 									// Number of physical Storage Servers	
        int numCPUs; 	 										// Number of physical CPUs         
        double memorySize_MB;        							// Amount of physical memory (MiB)
		double blockSize_KB;									// size of blocks (Kb)		
        int storageSize_GB;										// Amount of physical storage size (GiB)       
        int numNetworkInterfaces;								// Number of physical network interfaces 
        string ip;												// Node's IP
        int storageApp_ModuleIndex;								// Index of the storageApp
        int connectionTimeOut = default(15);					// Time limit from a message send by hypervisor until the reception of the response
        string networkServiceType = default("NetworkService");	// Network Service type

        // Network file system parameters
        int requestSize_KB = default(8);						// Request size (in KB)

        //Parallel file system parameters
        int strideSize_KB = default(1024);						// Stride Size (in KB)
		
    gates:
        //Storage server
        input fromVMStorageServers[];
        output toVMStorageServers[];

        input fromNodeStorageServers[numStorageServers];
        output toNodeStorageServers[numStorageServers];

        //cpu
        input fromVMCPU[];
        output toVMCPU[];

        input fromNodeCPU[numCPUs];
        output toNodeCPU[numCPUs];

        //memory
        input fromVMMemoryI[];
        input fromVMMemoryO[];
        output toVMMemoryI[];
        output toVMMemoryO[];

        input fromNodeMemoryI;
        input fromNodeMemoryO;
        output toNodeMemoryI;
        output toNodeMemoryO;

		//network
        input fromVMNet[];
        output toVMNet[];
        input fromNodeNet;			// Input gate from Network (Ethernet TCP)
        output toNodeNet;			// Output gate to Network (Ethernet TCP)


    submodules:

        memoryManager: <memScheduler> like IH_MemoryManager {
            parameters:
				blockSize_KB = blockSize_KB;
				memorySize_MB = memorySize_MB;
				memoryOverhead_MB = memoryOverhead_MB;
                @display("i=icons/Redirector;p=365,72");
            gates:
                fromVMMemoryI[];
                fromVMMemoryO[];
                toVMMemoryI[];
                toVMMemoryO[];

                fromNodeMemoryI;
                fromNodeMemoryO;
                toNodeMemoryI;
                toNodeMemoryO;
        }

        storageManager:  H_StorageManagerModule { 
            parameters:
                numStorageServers = numStorageServers;
                // Storage remote parameters                        
                requestSize_KB = requestSize_KB;
                strideSize_KB = strideSize_KB;
                storageManagerSched = storageScheduler;
                ioOverhead = ioOverhead;
                @display("i=icons/Redirector;p=77,182");
            gates:
                fromVMStorageServers[];
                toVMStorageServers[];
                fromNodeStorageServers[numStorageServers];
                toNodeStorageServers[numStorageServers];
                fromNET_Manager;
                toNET_Manager;
        }

        cpuManager: <cpuScheduler> like IH_CPUManager {
            parameters:
                numCPUs = numCPUs;
                computingOverhead = computingOverhead;
                @display("i=icons/Redirector;p=365,182");
            gates:
                fromVMCPU[];
                toVMCPU[];

                fromNodeCPU[numCPUs];
                toNodeCPU[numCPUs];
        }

        networkService: <networkServiceType> like INetworkService {
            parameters:
                localIP = ip;
                @display("i=icons/Redirector;p=214,72");
        }

        netManager: H_NETManager {
            parameters:
                networkOverhead = networkOverhead;
                storageApp_ModuleIndex = storageApp_ModuleIndex;
                connectionTimeOut = connectionTimeOut;
                schedulerType = netScheduler;
                @display("i=icons/Redirector;p=77,72");
            gates:

                fromVMNet[];
                toVMNet[];
                fromNodeNet;
                toNodeNet;
                fromHStorageManager;
                toHStorageManager;
        }

    connections allowunconnected:

        // Connections between VMCPU and NodeCPU		 
        for i=0..numCPUs-1 {
            cpuManager.fromNodeCPU[i] <-- fromNodeCPU[i];
            cpuManager.toNodeCPU[i] --> toNodeCPU[i];
        }
        // Connection between node OS and hypervisor
        cpuManager.fromVMCPU++ <-- fromVMCPU++;
        cpuManager.toVMCPU++ --> toVMCPU++;
        memoryManager.toVMMemoryI++ --> toVMMemoryI++;
        memoryManager.fromVMMemoryI++ <-- fromVMMemoryI++;
        memoryManager.fromVMMemoryO++ <-- fromVMMemoryO++;
        memoryManager.toVMMemoryO++ --> toVMMemoryO++;
        
        for i=0..numStorageServers-1 {
        	storageManager.fromVMStorageServers++ <-- fromVMStorageServers++;
        	storageManager.toVMStorageServers++ --> toVMStorageServers++;
        }
        
        netManager.fromVMNet++ <-- fromVMNet++;
        netManager.toVMNet++ --> toVMNet++;
        
		// Connections between VM Memory and Node Memory
        memoryManager.fromNodeMemoryI <-- fromNodeMemoryI;
        memoryManager.fromNodeMemoryO <-- fromNodeMemoryO;
        memoryManager.toNodeMemoryI --> toNodeMemoryI;
        memoryManager.toNodeMemoryO --> toNodeMemoryO;


        // Connections between VM Storage Servers and Node Storage Servers
        for i=0..numStorageServers-1 {
            storageManager.fromNodeStorageServers[i] <-- fromNodeStorageServers[i];
            storageManager.toNodeStorageServers[i] --> toNodeStorageServers[i];
        }

        // Connections between Network Manager and Storage Server Manager
        netManager.fromHStorageManager <-- storageManager.toNET_Manager;
        netManager.toHStorageManager --> storageManager.fromNET_Manager;

        // Connections between VM Network and Network Service
        netManager.fromNodeNet <-- networkService.toNetManager;
        netManager.toNodeNet --> networkService.fromNetManager;

        // Connections between Network Service and Networks
        networkService.fromNetTCP <-- fromNodeNet;
        networkService.toNetTCP --> toNodeNet;

}
