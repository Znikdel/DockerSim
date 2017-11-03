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

#include "Container_jobBase.h"

Container_jobBase::~Container_jobBase() {
    delete(Container_JobResults);
}


 void Container_jobBase::initialize(){

      appType = "";
      fsStructures.clear();
      preloadFiles.clear();
      Container_JobResults = new Container_JobResultsSet();

      API_OS::initialize();

 }

 void Container_jobBase::finish(){

     //delete(Container_JobResults);
     Container_JobResults->finishJobResultsSet();

     API_OS::finish();
     fsStructures.clear();
     preloadFiles.clear();
 }

 double Container_jobBase::getTimeToStart(){
     double timeToStart;

     timeToStart = par("timeToProcessUntilStart").doubleValue();

     return timeToStart;
 }

