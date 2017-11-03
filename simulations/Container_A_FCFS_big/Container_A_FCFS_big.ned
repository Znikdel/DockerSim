package icancloud.simulations.Container_A_FCFS_big;

import ned.DatarateChannel;
import inet.nodes.ethernet.EtherSwitch;
import icancloud.src.Architecture.Node.NodeVL.NodeVL;
import icancloud.src.Management.CloudManagement.ResourcesProvisioning.**.*;
import icancloud.src.Management.NetworkManager.NetworkManager;
import icancloud.src.Management.Topology.Topology;
import icancloud.src.Management.NetworkManager.NetworkManager;
import icancloud.src.Users.UserGenerator.UserGenerator;
import icancloud.src.Virtualization.VirtualMachines.SetDefinition.VmSet;
import inet.networklayer.autorouting.ipv4.IPv4NetworkConfigurator;

// ---------------------------------------------------------------
//   Definition of channels
// ---------------------------------------------------------------
channel Channel_0_Container_A_FCFS_big extends DatarateChannel
{
    delay = 1.25E-4us;
    datarate = 10Mbps;
    ber = 0.0;
    per = 0.0;
}

channel Channel_1_Container_A_FCFS_big extends DatarateChannel
{
    delay = 1.25E-4us;
    datarate = 10Mbps;
    ber = 0.0;
    per = 0.0;
}

channel Channel_2_Container_A_FCFS_big extends DatarateChannel
{
    delay = 1.25E-4us;
    datarate = 10Mbps;
    ber = 0.0;
    per = 0.0;
}

channel Channel_3_Container_A_FCFS_big extends DatarateChannel
{
    delay = 1.25E-4us;
    datarate = 10Mbps;
    ber = 0.0;
    per = 0.0;
}

channel Channel_4_Container_A_FCFS_big extends DatarateChannel
{
    delay = 1.25E-4us;
    datarate = 10Mbps;
    ber = 0.0;
    per = 0.0;
}

channel Channel_5_Container_A_FCFS_big extends DatarateChannel
{
    delay = 1.25E-4us;
    datarate = 10Mbps;
    ber = 0.0;
    per = 0.0;
}

channel Channel_6_Container_A_FCFS_big extends DatarateChannel
{
    delay = 1.25E-4us;
    datarate = 10Mbps;
    ber = 0.0;
    per = 0.0;
}

channel Channel_7_Container_A_FCFS_big extends DatarateChannel
{
    delay = 1.25E-4us;
    datarate = 10Mbps;
    ber = 0.0;
    per = 0.0;
}

channel Channel_8_Container_A_FCFS_big extends DatarateChannel
{
    delay = 1.25E-4us;
    datarate = 10Mbps;
    ber = 0.0;
    per = 0.0;
}

channel Channel_9_Container_A_FCFS_big extends DatarateChannel
{
    delay = 1.25E-4us;
    datarate = 10Mbps;
    ber = 0.0;
    per = 0.0;
}

channel Channel_10_Container_A_FCFS_big extends DatarateChannel
{
    delay = 1.25E-4us;
    datarate = 10Mbps;
    ber = 0.0;
    per = 0.0;
}

channel Channel_11_Container_A_FCFS_big extends DatarateChannel
{
    delay = 1.25E-4us;
    datarate = 10Mbps;
    ber = 0.0;
    per = 0.0;
}

channel RackChannel_0_Container_A_FCFS_big extends DatarateChannel
{
    delay = 1.25E-4us;
    datarate = 10Gbps;// RackChannelBandwidth
    ber = 0.0;
    per = 0.0;
}

channel RackChannel_1_Container_A_FCFS_big extends DatarateChannel
{
    delay = 1.25E-4us;
    datarate = 10Gbps;// RackChannelBandwidth
    ber = 0.0;
    per = 0.0;
}

channel RackChannel_2_Container_A_FCFS_big extends DatarateChannel
{
    delay = 1.25E-4us;
    datarate = 10Gbps;// RackChannelBandwidth
    ber = 0.0;
    per = 0.0;
}

channel RackChannel_3_Container_A_FCFS_big extends DatarateChannel
{
    delay = 1.25E-4us;
    datarate = 10Mbps;// RackChannelBandwidth
    ber = 0.0;
    per = 0.0;
}

channel RackChannel_4_Container_A_FCFS_big extends DatarateChannel
{
    delay = 1.25E-4us;
    datarate = 10Mbps;// RackChannelBandwidth
    ber = 0.0;
    per = 0.0;
}

channel RackChannel_5_Container_A_FCFS_big extends DatarateChannel
{
    delay = 1.25E-4us;
    datarate = 10Mbps;// RackChannelBandwidth
    ber = 0.0;
    per = 0.0;
}

// ---------------------------------------------------------------
//   Definition of the cloud
// ---------------------------------------------------------------
network Container_A_FCFS_big
{
    // -------------------------------------------------------
    //   Definition of main modules
    // -------------------------------------------------------
    submodules:


        // ---------------------------------------------------------------
        //   Network configurator
        // ---------------------------------------------------------------
        configurator: IPv4NetworkConfigurator {
            parameters:
                config = xml("<config><interface hosts='**' address='192.168.x.x' netmask='255.x.x.x'/></config>");
        }

        // ---------------------------------------------------------------
        //    Network Manager
        // ---------------------------------------------------------------
        networkManager: NetworkManager;

        // ---------------------------------------------------------------
        //   Definition of cloud scheduler
        // ---------------------------------------------------------------
        manager: CloudSchedulerFCFS {
            parameters:
                numberOfPFSRemoteServers = 2;
                remote_storage_VMs_per_node = 2;
                minimum_percent_storageNode_free = 0.15;
                virtualIPsBasis = "10.0.0.1";
        }

        // ---------------------------------------------------------------
        //   Definition of user generator
        // ---------------------------------------------------------------
        userGenerator: UserGenerator {
            parameters:
                numCells = 1;
        }

        // ---------------------------------------------------------------
        //   Definition of switches
        // ---------------------------------------------------------------
        switch_0: EtherSwitch;

        switch_1: EtherSwitch;

        switch_2: EtherSwitch;

        switch_3: EtherSwitch;

        rackSwitch_0: EtherSwitch;

        rackSwitch_1: EtherSwitch;

        rackSwitch_2: EtherSwitch;

        rackSwitch_3: EtherSwitch;

        rackSwitch_4: EtherSwitch;

        rackSwitch_5: EtherSwitch;

        // ---------------------------------------------------------------
        //   Definition of Computing Nodes
        // ---------------------------------------------------------------
        // ---------------------------------------------------------------
        //   Definition of Storage Nodes
        // ---------------------------------------------------------------
        ns_0_NodeStorage_C: NodeVL;
        ns_1_NodeStorage_C: NodeVL;
        ns_2_NodeStorage_C: NodeVL;
        //------------------------------------------------------
        // Modules that defines the topology and the set of VMs 
        //------------------------------------------------------
        vmSet: VmSet;
        topology: Topology;


        // ---------------------------------------------------------------
        //   Definition of Racks
        // ---------------------------------------------------------------
        rc_0_Rack_B_32[32]: NodeVL {

            @display("p=376,168");
        }

        rc_1_Rack_B_32[32]: NodeVL {

            @display("p=95,161");
        }


        rc_2_Rack_B_32[32]: NodeVL {

            @display("p=237,141");
        }


        rc_3_Rack_C_48[48]: NodeVL {

            @display("p=413,474");
        }


        rc_4_Rack_C_48[48]: NodeVL {

            @display("p=339,592");
        }


        rc_5_Rack_C_48[48]: NodeVL {

            @display("p=191,592");
        }


    // ---------------------------------------------------------------
    //   Connections section
    // ---------------------------------------------------------------
    connections allowunconnected:

        // Connections for rack rc_0_Rack_B_32
        for i=0..31 {
            rc_0_Rack_B_32[i].ethg++ <--> RackChannel_0_Container_A_FCFS_big <--> rackSwitch_0.ethg++;
        }

        // Connections for rack rc_1_Rack_B_32
        for i=0..31 {
            rc_1_Rack_B_32[i].ethg++ <--> RackChannel_0_Container_A_FCFS_big <--> rackSwitch_1.ethg++;
        }

        // Connections for rack rc_2_Rack_B_32
        for i=0..31 {
            rc_2_Rack_B_32[i].ethg++ <--> RackChannel_0_Container_A_FCFS_big <--> rackSwitch_2.ethg++;
        }

        // Connections for rack rc_3_Rack_C_48
        for i=0..47 {
            rc_3_Rack_C_48[i].ethg++ <--> RackChannel_0_Container_A_FCFS_big <--> rackSwitch_3.ethg++;
        }

        // Connections for rack rc_4_Rack_C_48
        for i=0..47 {
            rc_4_Rack_C_48[i].ethg++ <--> RackChannel_0_Container_A_FCFS_big <--> rackSwitch_4.ethg++;
        }

        // Connections for rack rc_5_Rack_C_48
        for i=0..47 {
            rc_5_Rack_C_48[i].ethg++ <--> RackChannel_0_Container_A_FCFS_big <--> rackSwitch_5.ethg++;
        }

        switch_0.ethg++ <--> Channel_0_Container_A_FCFS_big <--> rackSwitch_0.ethg++;
        switch_0.ethg++ <--> Channel_1_Container_A_FCFS_big <--> rackSwitch_1.ethg++;
        switch_0.ethg++ <--> Channel_2_Container_A_FCFS_big <--> rackSwitch_2.ethg++;
        switch_1.ethg++ <--> Channel_3_Container_A_FCFS_big <--> switch_0.ethg++;
        switch_1.ethg++ <--> Channel_4_Container_A_FCFS_big <--> switch_2.ethg++;
        switch_1.ethg++ <--> Channel_5_Container_A_FCFS_big <--> switch_3.ethg++;
        switch_2.ethg++ <--> Channel_6_Container_A_FCFS_big <--> rackSwitch_3.ethg++;
        switch_2.ethg++ <--> Channel_7_Container_A_FCFS_big <--> rackSwitch_4.ethg++;
        switch_2.ethg++ <--> Channel_8_Container_A_FCFS_big <--> rackSwitch_5.ethg++;
        switch_3.ethg++ <--> Channel_9_Container_A_FCFS_big <--> ns_0_NodeStorage_C.ethg++;
        switch_3.ethg++ <--> Channel_10_Container_A_FCFS_big <--> ns_1_NodeStorage_C.ethg++;
        switch_3.ethg++ <--> Channel_11_Container_A_FCFS_big <--> ns_2_NodeStorage_C.ethg++;
}
