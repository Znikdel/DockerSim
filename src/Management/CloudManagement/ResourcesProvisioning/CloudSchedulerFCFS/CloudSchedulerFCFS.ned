// Module that implements a cloud scheduler
//
// This scheduler is a First Come First Serve
//
// @authors Gabriel Gonz&aacute;lez Casta&ntilde;&eacute
// @date 2014-12-11

       
package icancloud.src.Management.CloudManagement.ResourcesProvisioning.CloudSchedulerFCFS;

import icancloud.src.Management.CloudManagement.ResourcesProvisioning.CloudManagerScheduler;

module CloudSchedulerFCFS extends CloudManagerScheduler 
{
    parameters:
        
        @class(CloudSchedulerFCFS);
        @display("i=msg/job");
        
}
