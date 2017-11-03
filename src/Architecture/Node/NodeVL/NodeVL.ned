
// Module that implements a Node with its virtualization layer (hypervisor).
//
// This class models a node with an hypervisor. It is able to execute applications at node and virtual machines 
//	requested by tenants.
//
// @author Gabriel González Castañé
// @date 2014-12-12

package icancloud.src.Architecture.Node.NodeVL;

import icancloud.src.Architecture.Node.INode;
import icancloud.src.Architecture.NodeComponents.Hardware.CPUs.ICPU_Module;
import icancloud.src.Architecture.NodeComponents.Hardware.Memories.MainMemories.IMainMemory;
import icancloud.src.Architecture.NodeComponents.Hardware.Storage.StorageSystems.StorageSystem;
import icancloud.src.Architecture.NodeComponents.OperatingSystems.OperatingSystemModules.OperatingSystemModule;
import icancloud.src.EnergySystem.EnergyMeter.EnergyMeter;
import icancloud.src.EnergySystem.PSU.IPSU;
import icancloud.src.Virtualization.Hypervisor.IHypervisor;
import icancloud.src.Virtualization.VirtualMachines.Containers.DockerEngine;
import inet.base.NotificationBoard;
import inet.linklayer.IExternalNic;
import inet.linklayer.IWiredNic;
import inet.linklayer.loopback.LoopbackInterface;
import inet.networklayer.common.InterfaceTable;
import inet.networklayer.ipv4.RoutingTable;
import inet.nodes.inet.NetworkLayer;
import inet.nodes.xmipv6.HomeAgent6;
import inet.transport.ITCP;


module NodeVL like INode

{
    parameters:

        @class("NodeVL");
        @node;
        @labels(node,ethernet-node,wireless-node);
        @display("i=old/telnet");

        // Default parameters
        int numExtInterfaces = default(0);														// Number of external interfaces
        int numNetworkInterfaces = default(1);													// As default, 1 network interface
        string tcpType = default(firstAvailable("TCP", "TCP_lwIP", "TCP_NSC", "TCP_None"));   	// Type of TCP protocol     
        bool IPForward = default(false);														// IP Forward        
        string ip = default("");															// Node's IP
        string routingFile = default("");														// Routing File
        int namid = default(-1);																// Nam ID
        bool forwardMulticast = default(false);													// Forwarding multicast as default		
        bool storageNode = default(false);														// is this a atorage Node? If no, then its a compute node
        int storage_local_port = default(2049);													// Port for listening the incoming connections
        string hostName = default("");															// Node's hostname       

        bool activeEnergyMeter = default(true);

        // Main Hardware subsystems        
        string cpuModuleType = default("CPU_Module");				// CPUModule type as CPU type
        string memoryType = default("RAMmemory");					// RAMmemory as memory type  
        string psuModuleType = default("MainPSU");					// PSU module  

         // Main Software configuration
        string hypervisorType = default("HypervisorNoOverhead");			// Main hypervisor		

        // Node parameters
        int numCores;												// Number of CPU cores		
        double memorySize_MB;										// Total amount of memory of this node (in MB)
        double memoryBlockSize_KB = default(128);
        int numStorageSystems = default(1);							// Number of Storage systems. As default, 1 storage system
        int storageSize_GB;									// Total amount of space for each storage system
        string initialState;										// state or the node  NODE_STATE_OFF | NODE_STATE_IDLE | NODE_STATE_RUNNING
        int numFS = default(1);													// Number of File Systems

    gates:
        inout pppg[] @labels(PPPFrame-conn);
        inout ethg[] @labels(EtherFrame-conn);

         //storage server
        input fromVMStorageServers[];
        output toVMStorageServers[];

        //cpu
        input fromVMCPU[];
        output toVMCPU[];

        //memory
        input fromVMMemoryI[];
        input fromVMMemoryO[];
        output toVMMemoryI[];
        output toVMMemoryO[];

        // Hypervisor net
        input fromVMNet[];
        output toVMNet[];


    submodules:

        notificationBoard: NotificationBoard {
            parameters:
                @display("p=53,155;is=s");
        }

        // network layer
        networkLayer: NetworkLayer {
            parameters:
                @display("p=333,304;q=queue");
                proxyARP = default(false);
        }

        routingTable: RoutingTable {
            parameters:
                @display("p=147,532;is=s");
                IPForward = IPForward;
                forwardMulticast = forwardMulticast;
                routingFile = routingFile;
        }

        // linklayer
        interfaceTable: InterfaceTable {
            parameters:
                @display("p=45,532;is=s");
        }

        lo0: LoopbackInterface {
            @display("p=147,412");
        }

        eth[sizeof(ethg)]: <default("EthernetInterface")> like IWiredNic {
            parameters:
                @display("p=378,475,row,60;q=txQueue");
        }
        ppp[sizeof(pppg)]: <default("PPPInterface")> like IWiredNic {
            parameters:
                @display("p=482,406,row,60;q=txQueue");
        }
        ext[numExtInterfaces]: <default("ExtInterface")> like IExternalNic {
            parameters:
                @display("p=275,412,row,10;q=txQueue;i=block/ifcard");
        }

        energyMeter: EnergyMeter {
            parameters:
                activeEnergyMeter = activeEnergyMeter;
                @display("p=45,459");
        }

        psu: <psuModuleType> like IPSU {
            @display("p=45,400");
        }

        tcp: <tcpType> like ITCP {
            parameters:
                @display("p=327,204");
        }

        osModule: OperatingSystemModule {
            parameters:
                numFS = numFS;
                numCPUs = numCores;
                numStorageSystems = numStorageSystems;
                @display("p=339,52");
            gates:
                fromApps[];
                toApps[];
                fromCPU[numCores];
                toCPU[numCores];
                fromStorageSystem[numStorageSystems];
                toStorageSystem[numStorageSystems];
                fromInputMemory;
                fromOutputMemory;
                toInputMemory;
                toOutputMemory;
                fromNet_TCP;
                toNet_TCP;
                fromDockerEngine[];
                toDockerEngine[];
        }

        cpuModule: <cpuModuleType> like ICPU_Module {
            parameters:
                numCPUs = numCores;
                @display("p=230,93");
            gates:
                fromOS[numCores];
                toOS[numCores];

        }
        storageSystem: StorageSystem {
            parameters:
                numStorageSystems = numStorageSystems;
                @display("i=device/blockManager;p=210,204,row");
            gates:
                fromHypervisor[numStorageSystems];
                toDevice[numStorageSystems];

        }
        memory: <memoryType> like IMainMemory {
            parameters:
                blockSize_KB = memoryBlockSize_KB;
                size_MB = memorySize_MB;
                @display("p=65,180");
            gates:
                fromInput;
                toInput;
        }

        notificationBoard: NotificationBoard {
            parameters:
                @display("p=45,343;i=block/control");


        }

        hypervisor: <hypervisorType> like IHypervisor {
            parameters:
                numStorageServers = numStorageSystems; 							// Number of Storage Servers	
                numCPUs = numCores; 	 										// Number of CPUs
                memorySize_MB = memorySize_MB;
                blockSize_KB = memoryBlockSize_KB;
                numNetworkInterfaces = numNetworkInterfaces;
                ip = ip;
                storageSize_GB = storageSize_GB;
                storageApp_ModuleIndex = 0;
                @display("p=55,52;i=energy/Hypervisor_40");
            gates:

                //Hypervisor to array of gates connections
                //                Storage server
                fromVMStorageServers[];
                toVMStorageServers[];

                //cpu
                fromVMCPU[];
                toVMCPU[];

                //memory
                fromVMMemoryI[];
                fromVMMemoryO[];
                toVMMemoryI[];
                toVMMemoryO[];

				//Hypervisor to HW connections                
                fromNodeCPU[numCores];
                toNodeCPU[numCores];

                fromNodeStorageServers[numStorageSystems];
                toNodeStorageServers[numStorageSystems];

                fromNodeMemoryI;
                fromNodeMemoryO;
                toNodeMemoryI;
                toNodeMemoryO;

                fromNodeNet;			// Input gate from Network (Ethernet TCP)
                toNodeNet;			// Output gate to Network (Ethernet TCP)

        }

    connections allowunconnected:

        // Connections between CPU and Hypervisor
        for i=0..numCores-1 {
            cpuModule.toOS[i] --> hypervisor.fromNodeCPU[i];
            cpuModule.fromOS[i] <-- hypervisor.toNodeCPU[i];

        }

        // Connections between Storage Servers and hypervisor
        for i=0..numStorageSystems-1 {
            storageSystem.toDevice[i] --> hypervisor.fromNodeStorageServers[i];
            storageSystem.fromHypervisor[i] <-- hypervisor.toNodeStorageServers[i];
        }

        // Connections between Memory and hypervisor
        memory.toInput --> hypervisor.fromNodeMemoryI;
        memory.fromInput <-- hypervisor.toNodeMemoryI;
        memory.fromOutput <-- hypervisor.toNodeMemoryO;
        memory.toOutput --> hypervisor.fromNodeMemoryO;

		// Connections between TCP and hypervisor

        hypervisor.toNodeNet --> tcp.appIn++;
        hypervisor.fromNodeNet <-- tcp.appOut++;

		// Connections between the BS and the Operative System        
        for i=0..numStorageSystems-1 {
            osModule.fromStorageSystem++ <-- hypervisor.toVMStorageServers++;
            osModule.toStorageSystem++ --> hypervisor.fromVMStorageServers++;
        }
        //Conections of the Memory and Operative System
        osModule.fromInputMemory <-- hypervisor.toVMMemoryI++;
        osModule.toInputMemory --> hypervisor.fromVMMemoryI++;
        osModule.fromOutputMemory <-- hypervisor.toVMMemoryO++;
        osModule.toOutputMemory --> hypervisor.fromVMMemoryO++;

		//Conections of the CPU and Operative System
        osModule.fromCPU[0] <-- hypervisor.toVMCPU++;
        osModule.toCPU[0] --> hypervisor.fromVMCPU++;

        // Connections between Operating System and Network
        osModule.toNet_TCP --> hypervisor.fromVMNet++;
        osModule.fromNet_TCP <-- hypervisor.toVMNet++;

        // INET internals...
        tcp.ipOut --> networkLayer.transportIn++;
        tcp.ipIn <-- networkLayer.transportOut++;

        networkLayer.ifOut++ --> lo0.upperLayerIn;
        lo0.upperLayerOut --> networkLayer.ifIn++;

        // connections to network outside
        for i=0..sizeof(pppg)-1 {
            pppg[i] <--> ppp[i].phys;
            ppp[i].upperLayerOut --> networkLayer.ifIn++;
            ppp[i].upperLayerIn <-- networkLayer.ifOut++;
        }

        for i=0..sizeof(ethg)-1 {
            ethg[i] <--> eth[i].phys;
            eth[i].upperLayerOut --> networkLayer.ifIn++;
            eth[i].upperLayerIn <-- networkLayer.ifOut++;
        }

        for i=0..numExtInterfaces-1 {
            ext[i].upperLayerOut --> networkLayer.ifIn++;
            ext[i].upperLayerIn <-- networkLayer.ifOut++;
        }
}
