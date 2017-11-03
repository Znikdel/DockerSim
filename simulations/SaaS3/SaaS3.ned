package icancloud.simulations.SaaS3;

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
channel Channel_0_SaaS extends DatarateChannel
{
    delay = 1.25E-4us;
    datarate = 10Gbps;
    ber = 0.0;
    per = 0.0;
}

channel Channel_1_SaaS extends DatarateChannel
{
    delay = 1.25E-4us;
    datarate = 10Gbps;
    ber = 0.0;
    per = 0.0;
}

channel Channel_2_SaaS extends DatarateChannel
{
    delay = 1.25E-4us;
    datarate = 10Gbps;
    ber = 0.0;
    per = 0.0;
}

channel Channel_3_SaaS extends DatarateChannel
{
    delay = 1.25E-4us;
    datarate = 10Gbps;
    ber = 0.0;
    per = 0.0;
}

channel Channel_4_SaaS extends DatarateChannel
{
    delay = 1.25E-4us;
    datarate = 10Gbps;
    ber = 0.0;
    per = 0.0;
}

channel Channel_5_SaaS extends DatarateChannel
{
    delay = 1.25E-4us;
    datarate = 10Gbps;
    ber = 0.0;
    per = 0.0;
}

channel Channel_6_SaaS extends DatarateChannel
{
    delay = 1.25E-4us;
    datarate = 10Gbps;
    ber = 0.0;
    per = 0.0;
}

channel Channel_7_SaaS extends DatarateChannel
{
    delay = 1.25E-4us;
    datarate = 10Gbps;
    ber = 0.0;
    per = 0.0;
}

channel Channel_8_SaaS extends DatarateChannel
{
    delay = 1.25E-4us;
    datarate = 10Gbps;
    ber = 1.25E-4;
    per = 1.25E-4;
}

channel Channel_9_SaaS extends DatarateChannel
{
    delay = 1.25E-4us;
    datarate = 10Gbps;
    ber = 0.0;
    per = 0.0;
}

channel Channel_10_SaaS extends DatarateChannel
{
    delay = 1.25E-4us;
    datarate = 10Gbps;
    ber = 0.0;
    per = 0.0;
}

channel Channel_11_SaaS extends DatarateChannel
{
    delay = 1.25E-4us;
    datarate = 10Gbps;
    ber = 0.0;
    per = 0.0;
}

channel RackChannel_0_SaaS extends DatarateChannel
{
    delay = 1.25E-4us;
    datarate = 10Gbps;// RackChannelBandwidth
    ber = 0.0;
    per = 0.0;
}

channel RackChannel_1_SaaS extends DatarateChannel
{
    delay = 1.25E-4us;
    datarate = 10Gbps;// RackChannelBandwidth
    ber = 0.0;
    per = 0.0;
}

channel RackChannel_2_SaaS extends DatarateChannel
{
    delay = 1.25E-4us;
    datarate = 10Gbps;// RackChannelBandwidth
    ber = 0.0;
    per = 0.0;
}

channel RackChannel_3_SaaS extends DatarateChannel
{
    delay = 1.25E-4us;
    datarate = 10Mbps;// RackChannelBandwidth
    ber = 0.0;
    per = 0.0;
}

channel RackChannel_4_SaaS extends DatarateChannel
{
    delay = 1.25E-4us;
    datarate = 10Mbps;// RackChannelBandwidth
    ber = 0.0;
    per = 0.0;
}

channel RackChannel_5_SaaS extends DatarateChannel
{
    delay = 1.25E-4us;
    datarate = 10Mbps;// RackChannelBandwidth
    ber = 0.0;
    per = 0.0;
}

// ---------------------------------------------------------------
//   Definition of the cloud
// ---------------------------------------------------------------
network SaaS3
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
        networkManager: NetworkManager {
            @display("p=493,345");
        }

        // ---------------------------------------------------------------
        //   Definition of cloud scheduler
        // ---------------------------------------------------------------
      //  manager: CloudSchedulerRR {
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
                @display("p=367,393");
        }

        // ---------------------------------------------------------------
        //   Definition of switches
        // ---------------------------------------------------------------
        switch_0: EtherSwitch;
        switch_1: EtherSwitch {

            @display("p=379,238");
        }
        rackSwitch_0: EtherSwitch;
        rackSwitch_1: EtherSwitch;
        rackSwitch_2: EtherSwitch;
        // ---------------------------------------------------------------
        //   Definition of Computing Nodes
        // ---------------------------------------------------------------
        // ---------------------------------------------------------------
        //   Definition of Storage Nodes
        // ---------------------------------------------------------------
        ns_0_NodeStorage_C: NodeVL;
        //------------------------------------------------------
        // Modules that defines the topology and the set of VMs 
        //------------------------------------------------------
        vmSet: VmSet {
            @display("p=107,302");
        }
        topology: Topology;
        // ---------------------------------------------------------------
        //   Definition of Racks
        // ---------------------------------------------------------------
        rc_0_Rack_B_2[2]: NodeVL;
        rc_1_Rack_B_2[2]: NodeVL;
        rc_2_Rack_B_2[1]: NodeVL;
    // ---------------------------------------------------------------
    //   Connections section
    // ---------------------------------------------------------------
    connections allowunconnected:

        // Connections for rack rc_0_Rack_B_2
        for i=0..1 {
            rc_0_Rack_B_2[i].ethg++ <--> RackChannel_0_SaaS <--> rackSwitch_0.ethg++;
        }

        // Connections for rack rc_1_Rack_B_2
        for i=0..1 {
            rc_1_Rack_B_2[i].ethg++ <--> RackChannel_0_SaaS <--> rackSwitch_1.ethg++;
        }

        // Connections for rack rc_2_Rack_B_2
        for i=0..0 {
            rc_2_Rack_B_2[i].ethg++ <--> RackChannel_0_SaaS <--> rackSwitch_2.ethg++;
        }

        rackSwitch_1.ethg++ <--> Channel_0_SaaS <--> switch_0.ethg++;
        rackSwitch_0.ethg++ <--> Channel_1_SaaS <--> switch_0.ethg++;
        switch_0.ethg++ <--> Channel_2_SaaS <--> rackSwitch_2.ethg++;
        switch_0.ethg++ <--> Channel_3_SaaS<--> switch_1.ethg++;
        ns_0_NodeStorage_C.ethg++ <--> Channel_5_SaaS <--> switch_1.ethg++;
}
