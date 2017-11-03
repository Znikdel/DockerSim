 // Module that implements an Operating System Module for a machine.
//
// This module is responsible for redirect all requests to  the corresponding subsytem
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @date 2010-11-30
//
// @author updated to iCanCloud by Gabriel González Castañé
// @date 2012-12-17

package icancloud.src.Architecture.NodeComponents.OperatingSystems.OperatingSystemModules;

import icancloud.src.Architecture.NodeComponents.OperatingSystems.SyscallManager.ISyscallManager;
import icancloud.src.Architecture.NodeComponents.OperatingSystems.SyscallManager.NodeSyscallManager.SyscallManager;
import icancloud.src.Architecture.NodeComponents.OperatingSystems.CPU_Schedulers.ICPU_Scheduler;
import icancloud.src.Architecture.NodeComponents.OperatingSystems.Storage.FileSystems.FSModule;
import icancloud.src.Architecture.NodeComponents.OperatingSystems.Storage.VirtualFileSystems.IVirtualFileSystem;
import icancloud.src.Architecture.NodeComponents.OperatingSystems.Storage.VolumeManagers.VolumeManagerModule;
import icancloud.src.Architecture.NodeComponents.OperatingSystems.SystemApps.RemoteStorageApp.RemoteStorageApp;
import icancloud.src.Architecture.NodeComponents.OperatingSystems.SystemApps.StatesApplication.StatesApplication;


module OperatingSystemModule
{
    parameters:

        double numCPUs;													// Number of CPUs
        double numFS = default(1);													// Number of File Systems
        double numStorageSystems;										// Number of Block Servers

        string vfsType = default("NodeVirtualFileSystem");				// Virtual File System type; 		
        string fsModuleType = default("Node_FileSystem");				// File System module type; 
        string vmModuleType = default("VolumeManagerModule");		    // Volume Manager module type
        string syscallManagerType = default("SyscallManager");
        string cpuSchedulerType;										// CPU Service type   			

        @display("i=icons/OOSS;bgb=309,340");

    gates:
        input fromApps[];			// Input gates from Applications
        input fromDockerEngine[];
        input fromStorageSystem[];	// Input gates from Block Servers
        input fromNet_TCP;			// Input gate from Network (Ethernet TCP)
        input fromCPU[];			// Input gates from CPU
        //***/
        input fromInputMemory;      // Input gate from imputs
        input fromOutputMemory;		// Input gate from output
        output toInputMemory;		// Output gate to inputs
        output toOutputMemory;		// Output gate to output

        output toApps[];			// Output gates to Applications
        output toDockerEngine[];
        output toStorageSystem[];	// Output gates to Block Servers
        output toNet_TCP;			// Output gate to Network (Ethernet TCP)
        output toCPU[];				// Output gates to CPU


    submodules:

        syscallManager: <syscallManagerType> like ISyscallManager {
            parameters:
                @display("p=74,147");
            gates:
                fromApps[];
                fromDockerEngine[];
          //      toDockerEngine[];
                
        }

        cpuScheduler: <cpuSchedulerType> like ICPU_Scheduler {
            parameters:
                numCPUs = numCPUs;
                @display("p=74,62");
            gates:
                fromCPU[numCPUs];
                toCPU[numCPUs];
        }

        node_vfs: <vfsType> like IVirtualFileSystem {
            parameters:
                numFS = numFS;
                @display("p=66,259");
            gates:
                fromFS[numFS];
                toFS[numFS];
        }

        fsModule[numFS]: FSModule {
            @display("p=159,260");
        }

        vmModule: VolumeManagerModule {
            parameters:
                numStorageSystems = numStorageSystems;
                numFS = numFS;
                @display("p=261,261");
            gates:
                fromFS[numFS];
                toFS[numFS];
                fromStorageSystem[numStorageSystems];
                toStorageSystem[numStorageSystems];
        }

        nodeStates: StatesApplication {
            @display("p=215,146;i=old/app");
            gates:
                fromOS;
                toOS;
        //        fromOSfromCon;
		//		toOStoCon;
          // //     fromContainers;
          //      toContainers;
        }

        remoteStorage: RemoteStorageApp {
            @display("p=215,72;i=old/app");
            gates:
                fromOS;
                toOS;
       //         fromOSfromCon;
	//			toOStoCon;
        //        fromOSContainers;
         //       toOSContainers;
        }

    connections allowunconnected:

        // to the remoteStorageApp
        syscallManager.fromApps++ <-- remoteStorage.toOS;
        syscallManager.toApps++ --> remoteStorage.fromOS;
        
  //      syscallManager.fromDockerEngine++ <-- remoteStorage.toOS++;
  //      syscallManager.toDockerEngine++ --> remoteStorage.fromOS++;

        // Connections between Service Redirector and Applications
        // to the statesApp
        syscallManager.fromApps++ <-- nodeStates.toOS;
        syscallManager.toApps++ --> nodeStates.fromOS;
        
  //      syscallManager.fromDockerEngine++ <-- nodeStates.toOS++;
 //       syscallManager.toDockerEngine++ --> nodeStates.fromOS++;
        
  //      syscallManager.fromDockerEngine[0] <--  fromDockerEngine[0];
  //      syscallManager.toDockerEngine[0] --> toDockerEngine[0];

        // Connections between Service Redirector and Network
        syscallManager.fromNet <-- fromNet_TCP;//networkService.toicancloud.src.PI;
        syscallManager.toNet --> toNet_TCP;//networkService.fromicancloud.src.PI;

        // Connections between Service Redirector and CPU Service
        syscallManager.fromCPU <-- cpuScheduler.toOsGate;
        syscallManager.toCPU --> cpuScheduler.fromOsGate;

        // Connections between Service Redirector and Memory	
        syscallManager.fromMemory <-- fromInputMemory;
        syscallManager.toMemory --> toInputMemory;

        // Connections between Memory and I/O Redirector		
        toOutputMemory <-- node_vfs.toMemory;
        fromOutputMemory --> node_vfs.fromMemory;

        // Connections between Virtual File System and File Systems
        for i=0..numFS-1 {
            node_vfs.fromFS[i] <-- fsModule[i].toIOR;
            node_vfs.toFS[i] --> fsModule[i].fromIOR;
        }

        // Connections between File Systems and Volume Manager
        for i=0..numFS-1 {
            vmModule.fromFS[i] <-- fsModule[i].toVM;
            vmModule.toFS[i] --> fsModule[i].fromVM;
        }

        // Connections between Volume Manager and Block Server
        for i=0..numStorageSystems-1 {
            vmModule.fromStorageSystem[i] <-- fromStorageSystem[i];
            vmModule.toStorageSystem[i] --> toStorageSystem[i];
        }

        // Connections between CPU Scheduler and CPU
        for i=0..numCPUs-1 {
            cpuScheduler.fromCPU[i] <-- fromCPU[i];
            cpuScheduler.toCPU[i] --> toCPU[i];
        }

}
