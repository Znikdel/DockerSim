// Module that implements a cloud scheduler
//
// This scheduler is a First Come First Serve but it produces the results grouped to import them from gnu plot.
//
// @authors Gabriel Gonz&aacute;lez Casta&ntilde;&eacute
// @date 2014-12-11

       
package icancloud.src.Management.CloudManagement.ResourcesProvisioning.CloudSchedulerFCFS_gnup;

import icancloud.src.Management.CloudManagement.ResourcesProvisioning.CloudManagerScheduler;

module CloudSchedulerFCFS_gnup extends CloudManagerScheduler
{
    parameters:
        
        @class(CloudSchedulerFCFS_gnup);
        @display("i=msg/job");
        
}
