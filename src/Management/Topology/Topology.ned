//
// Module that represents the elements that composes a data center.
// 
// It receive a quantity of compute nodes and storage nodes and has two main blocks, 
//		- The compute nodes
//		- The storage nodes
//
// @authors Gabriel Gonz&aacute;lez Casta&ntilde;&eacute
// @date 2014-12-11

package icancloud.src.Management.Topology;

import icancloud.src.Management.Topology.StorageNode;
import icancloud.src.Management.Topology.ComputeNode;

module Topology
{
    parameters:
        int computeNodeQuantity;
		int storageNodeQuantity;
    submodules:
        computeNode[computeNodeQuantity]: ComputeNode {         
                @display("i=msg/job;p=98,129,row");
        }
        storageNode[storageNodeQuantity]: StorageNode {
            parameters:
                @display("i=msg/job;p=98,129,row");
        }

}
