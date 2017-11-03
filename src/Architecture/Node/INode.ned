//
// Module interface that implements a Node.
//
// @author Gabriel González Castañé
// @date 2014-12-12

package icancloud.src.Architecture.Node;

import icancloud.src.Architecture.NodeComponents.Hardware.CPUs.ICPU_Module;
import icancloud.src.Architecture.NodeComponents.Hardware.Memories.MainMemories.IMainMemory;
import icancloud.src.Architecture.NodeComponents.Hardware.Storage.StorageSystems.StorageSystem;
import icancloud.src.Architecture.NodeComponents.OperatingSystems.OperatingSystemModules.OperatingSystemModule;
import icancloud.src.EnergySystem.EnergyMeter.EnergyMeter;
import icancloud.src.EnergySystem.PSU.IPSU;
import icancloud.src.Virtualization.Hypervisor.IHypervisor;
import inet.base.NotificationBoard;
import inet.linklayer.IExternalNic;
import inet.linklayer.IWiredNic;
import inet.linklayer.loopback.LoopbackInterface;
import inet.networklayer.common.InterfaceTable;
import inet.networklayer.ipv4.RoutingTable;
import inet.nodes.inet.NetworkLayer;
import inet.transport.ITCP;

moduleinterface INode

{
    parameters:

        @node;
        @display("i=old/telnet");
       		       
        // Main Hardware subsystems
        // CPU        
            string cpuModuleType;													// CPUModule type as CPU type
            int numCores;															// Number of CPU cores		
        // MEMORY
            string memoryType;														// RAMmemory as memory type  
            double memorySize_MB;													// Total amount of memory of this node (in MB)
        // Storage
            bool storageNode;														// is this a atorage Node? If no, then its a compute node
            int storage_local_port;													// Port for listening the incoming connections
            string hostName;														// Node's hostname       
    		int storageSize_GB;												// Total amount of space for each storage system
    		int numStorageSystems;													// Number of Storage systems. As default, 1 storage system
        // Network
            int numExtInterfaces;													// Number of external interfaces
            int numNetworkInterfaces;												// As default, 1 network interface
            string tcpType;   														// Type of TCP protocol     
            bool IPForward;															// IP Forward        
            string ip;															// Node's IP
            string routingFile;														// Routing File
            int namid;																// Nam ID
            bool forwardMulticast;	

		//Energy
            bool activeEnergyMeter;
    		string psuModuleType;													// PSU module
            string initialState;													// state or the node  NODE_STATE_OFF | NODE_STATE_IDLE | NODE_STATE_RUNNING        

    gates:
        inout pppg[] @labels(PPPFrame-conn);
        inout ethg[] @labels(EtherFrame-conn);
}
