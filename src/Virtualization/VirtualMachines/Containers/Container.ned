//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package icancloud.src.Virtualization.VirtualMachines.Containers;


import icancloud.src.Virtualization.VirtualMachines.Containers.IContainer;
import icancloud.src.Users.UserGenerator.PreloadFilesDefinition.PreloadFilesDefinition;
import icancloud.src.Users.UserGenerator.FSUserDefinition.FSUserDefinition;

module Container 
{
    parameters:
        string containerType=default("Container_CPU_Intensive");					// The name of ned / folder / class of the application
        string name;					// A name for the application
        int copies = default(1);		// Number of replicas that will be launched at system 
        int numFSRoutes;				// Number of different routes that applications will need  
        int numFiles;					// Number of different files that applications will need 
gates:
    
  //    input fromDockerEngine[] @loose;			// Input gate from OS (Operating System)
  //     output toDockerEngine[] @loose;			
  
      
  submodules:
              
		container: <containerType> like IContainer{
		                @display("p=65,103");
		    
		}
     
        FSConfig[numFSRoutes]: FSUserDefinition {
            @display("p=417,103");
        }

        preloadFiles[numFiles]: PreloadFilesDefinition {
            @display("p=233,103");
        }


}
