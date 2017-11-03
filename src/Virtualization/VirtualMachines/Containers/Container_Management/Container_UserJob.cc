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

#include "Container_UserJob.h"
#include "API_OS.h"

Container_UserJob::~Container_UserJob() {
    appType = "";
    mPtr =NULL;
    fsStructures.clear();
}


 void Container_UserJob::initialize(){

      appType = "";
      mPtr =NULL;
      fsStructures.clear();
      preloadFiles.clear();
      Container_JobResults = new Container_JobResultsSet();

      Container_jobBase::initialize();

 }

 void Container_UserJob::startExecution(){
     API_OS::startExecution();
 }

 void Container_UserJob::finish(){

     fsStructures.clear();
     preloadFiles.clear();

     Container_jobBase::finish();

 }

 Container_UserJob* Container_UserJob::cloneJob (cModule* userMod){

     // Define ...
         cModule *cloneApp;
         AbstractUser* user;
         Container_UserJob* newJob;
         cModuleType *modType;
         std::ostringstream appPath;
         int i, numParameters, size;

     // Init ..
         appPath << this->getNedTypeName();

     // Create the app module
         modType = cModuleType::get (appPath.str().c_str());

     // Create the app into the user module
         cloneApp = modType->create(appPath.str().c_str(), userMod);

     // Configure the main parameters
         numParameters = this->getNumParams();
         for (i = 0; i < numParameters ; i++){
             cloneApp->par(i) = this->par(i);
         }

         cloneApp->setName("app");

     // Finalize and build the module
         cloneApp->finalizeParameters();
         cloneApp->buildInside();

         // Call initiialize
         cloneApp->callInitialize();

         newJob = check_and_cast<Container_UserJob*> (cloneApp);
         newJob->setOriginalName(this->getOriginalName().c_str());
         newJob->setAppType(this->getAppType());

         user =  check_and_cast <AbstractUser*>  (userMod);
         newJob->setUpUser(user);

         size = this->getPreloadSize();
         for (i = 0; i < size; i++){
             newJob->setPreloadFile(this->getPreloadFile(i));
         }

         size = this->getFSSize();
         for (i = 0; i < size; i++){
             newJob->setFSElement(this->getFSElement(i));
         }

         return newJob;
 }


void Container_UserJob::setMachine (Machine* m){
    mPtr = m;
    Container_JobResults->setMachineType(mPtr->getTypeName());
}

void Container_UserJob::setUpUser (AbstractUser *user){

    userPtr = user;
}
