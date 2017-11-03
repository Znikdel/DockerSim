
// Module that implements a Node.
//
// This class models Computing node.
//
// @author Gabriel González Castañé
// @date 2014-12-12

package icancloud.src.Architecture.Node.Node;

import icancloud.src.Architecture.NodeComponents.Hardware.CPUs.ICPU_Module;
import icancloud.src.Architecture.NodeComponents.Hardware.Memories.MainMemories.IMainMemory;
import icancloud.src.Architecture.NodeComponents.Hardware.Storage.StorageSystems.StorageSystem;
import icancloud.src.Architecture.NodeComponents.OperatingSystems.OperatingSystemModules.OperatingSystemModule;
import icancloud.src.EnergySystem.EnergyMeter.EnergyMeter;
import icancloud.src.EnergySystem.PSU.IPSU;
import inet.base.NotificationBoard;
import inet.linklayer.IExternalNic;
import inet.linklayer.IWiredNic;
import inet.linklayer.loopback.LoopbackInterface;
import inet.networklayer.common.InterfaceTable;
import inet.networklayer.ipv4.RoutingTable;
import inet.nodes.inet.NetworkLayer;
import inet.transport.ITCP;

import icancloud.src.Architecture.Node.INode;

module Node like INode
{
    parameters:

        @class("Node");
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

        // Node parameters
        int numCores;												// Number of CPU cores		
        double memorySize_MB;										// Total amount of memory of this node (in MB)
        int numStorageSystems = default(1);							// Number of Storage systems. As default, 1 storage system
        int storageSize_GB;											// Total amount of space for each storage system
        string initialState;										// state or the node  NODE_STATE_OFF | NODE_STATE_IDLE | NODE_STATE_RUNNING

    gates:
        inout pppg[] @labels(PPPFrame-conn);
        inout ethg[] @labels(EtherFrame-conn);


    submodules:

        notificationBoard: NotificationBoard {
            parameters:
                @display("p=53,155;is=s");
        }

        // network layer
        networkLayer: NetworkLayer {
            parameters:
                @display("p=339,304;q=queue");
                proxyARP = default(false);
        }

        routingTable: RoutingTable {
            parameters:
                @display("p=53,381;is=s");
                IPForward = IPForward;
                forwardMulticast = forwardMulticast;
                routingFile = routingFile;
        }

        // linklayer
        interfaceTable: InterfaceTable {
            parameters:
                @display("p=53,322;is=s");
        }

        lo0: LoopbackInterface {
            @display("p=482,304");
        }

        eth[sizeof(ethg)]: <default("EthernetInterface")> like IWiredNic {
            parameters:
                @display("p=339,412,row,60;q=txQueue");
        }
        ppp[sizeof(pppg)]: <default("PPPInterface")> like IWiredNic {
            parameters:
                @display("p=482,406,row,60;q=txQueue");
        }
        ext[numExtInterfaces]: <default("ExtInterface")> like IExternalNic {
            parameters:
                @display("p=218,304,row,10;q=txQueue;i=block/ifcard");
        }

        energyMeter: EnergyMeter {
            parameters:
                activeEnergyMeter = activeEnergyMeter;
                @display("p=53,145");
        }

        psu: <psuModuleType> like IPSU {
            @display("p=53,57");
        }

        tcp: <tcpType> like ITCP {
            parameters:
                @display("p=339,198");
        }


        osModule: OperatingSystemModule {
            parameters:
                numCPUs = numCores;
                numStorageSystems = numStorageSystems;
                @display("p=339,52");
            gates:
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
                @display("p=197,51");
            gates:
                fromOS[numCores];
                toOS[numCores];

        }
        storageSystem: StorageSystem {
            parameters:
                numStorageSystems = numStorageSystems;
                @display("i=device/blockManager;p=463,52,row");
            gates:
                fromHypervisor[numStorageSystems];
                toDevice[numStorageSystems];

        }
        memory: <memoryType> like IMainMemory {
            parameters:
                size_MB = memorySize_MB;
                @display("p=463,153");
            gates:
                fromInput;
                toInput;
        }

        notificationBoard: NotificationBoard {
            parameters:
                @display("p=53,265;i=block/control");


        }



    connections allowunconnected:

        // CPUs
        for i=0..numCores-1 {
            // Connections between CPU and Hypervisor
            cpuModule.toOS++ --> osModule.fromCPU++;
            cpuModule.fromOS++ <-- osModule.toCPU++;

        }

        // Storage
        for i=0..numStorageSystems-1 {
            // Connections between Storage Servers and hypervisor
            storageSystem.toDevice++ --> osModule.fromCPU++;
            storageSystem.fromHypervisor++ <-- osModule.toStorageSystem++;
        }

        //Memory
        // Connections between Memory and hypervisor
        memory.toInput --> osModule.fromInputMemory;
        memory.fromInput <-- osModule.toInputMemory;
        memory.fromOutput <-- osModule.toOutputMemory;
        memory.toOutput --> osModule.fromOutputMemory;

        //NET
        // Connections between TCP and hypervisor
        osModule.toNet_TCP --> tcp.appIn++;
        osModule.fromNet_TCP <-- tcp.appOut++;

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
