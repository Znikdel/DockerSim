//
// Module that defines a cell that makes users appear at system.
//
// This module represents 24 hours creating users at system. 
//
// @author Gabriel Gonzalez Casta;&ntilde;e
// @date 2012-11-30

package icancloud.src.Users.UserGenerator.core.CustomGeneratorCells.UserGeneratorDay;

import icancloud.src.Users.UserGenerator.VmDefinition.VmDefinition;
import icancloud.src.Users.UserGenerator.DistributionDefinition.DistributionDefinition;
import icancloud.src.Users.UserGenerator.AppDefinition.AppDefinition;
import icancloud.src.Users.UserGenerator.ContainerDefinition.ContainerDefinition;

import icancloud.src.Users.UserGenerator.core.CustomGeneratorCells.IUserGenerator;
import icancloud.src.Users.UserGenerator.core.UserGenerator_cell;

simple UserGeneratorDay like IUserGenerator {
    parameters:
        // User parameters
        string name;
        string behavior;
        int quantity_user_preloaded;							// Users preloaded at the system before the distribution begins
        int total_users;										// Total users to be created per total time creation.
        double time_creation;									// Time creation in hours of users in the system in hours
        int repetitions; 										// # Repetitions of the behavior. -1 infinite.        
        string remoteServers = default("LOCAL");   
        
        int vmsToRentTypesQuantity;
        
        bool printResults = default(true);
        @display("i=abstract/person");

}
