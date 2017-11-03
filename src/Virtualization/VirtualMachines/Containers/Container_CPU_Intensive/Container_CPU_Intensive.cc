#include "Container_CPU_Intensive.h"
#define INPUT_FILE "/input.dat"
#define OUTPUT_FILE "/output.dat"
#define MAX_FILE_SIZE 2000000000

Define_Module(Container_CPU_Intensive);

Container_CPU_Intensive::~Container_CPU_Intensive(){
}


void Container_CPU_Intensive::initialize(){

    std::ostringstream osStream;
    timeoutEvent = NULL;
    timeout = 1.0;

    // Set the moduleIdName
    osStream << "Container_CPU_Intensive." << getId();
    moduleIdName = osStream.str();

    // Init the super-class
    Container_UserJob::initialize();

    // App Module parameters
    startDelay = par ("startDelay");
    inputSizeMB  = par ("inputSize");
    outputSizeMB  = par ("outputSize");
    MIs  = par ("MIs");

    iterations  = par ("iterations");
    currentIteration = 1;

    // Service times
    total_service_IO = 0.0;
    total_service_CPU = 0.0;
    startServiceIO = 0.0;
    endServiceIO = 0.0;
    startServiceCPU = 0.0;
    endServiceCPU = 0.0;
    readOffset = writeOffset = 0;

    // Boolean variables
    executeCPU = executeRead = executeWrite = false;

    // Assign names to the results
      Container_JobResults->newJobResultSet("totalIO");
        Container_JobResults->newJobResultSet("totalCPU");
        Container_JobResults->newJobResultSet("Real run-time");
        Container_JobResults->newJobResultSet("Simulation time");
}


void Container_CPU_Intensive::finish(){
    // Finish the super-class
   Container_UserJob::finish();

}

void Container_CPU_Intensive::startExecution (){

    API_OS::startExecution();

    Enter_Method_Silent();
    // Create SM_WAIT_TO_EXECUTE message for delaying the execution of this application
    cMessage *waitToExecuteMsg = new cMessage (SM_WAIT_TO_EXECUTE.c_str());
    scheduleAt (simTime()+startDelay, waitToExecuteMsg);
}

void Container_CPU_Intensive::processSelfMessage (cMessage *msg){

    if (!strcmp (msg->getName(), SM_WAIT_TO_EXECUTE.c_str())){

        // Starting time...
        simStartTime = simTime();
        runStartTime = time (NULL);

        // Init...
        startServiceIO = simTime();

        executeIOrequest (false, true);

    }else

        showErrorMessage ("Unknown self message [%s]", msg->getName());

    delete (msg);
}


void Container_CPU_Intensive::processRequestMessage (icancloud_Message *sm){

}


void Container_CPU_Intensive::processResponseMessage (icancloud_Message *sm){

    icancloud_App_IO_Message *sm_io;
    icancloud_App_CPU_Message *sm_cpu;
    bool isError;
    std::ostringstream osStream;
    int operation;


        // Init...
        operation = sm->getOperation ();
        sm_io = dynamic_cast<icancloud_App_IO_Message *>(sm);
        sm_cpu = dynamic_cast<icancloud_App_CPU_Message *>(sm);
        isError = false;


        // IO Message?
        if (sm_io != NULL){

            // Get time!
            endServiceIO = simTime();

            // Read response!
            if (operation == SM_READ_FILE){
                // All ok!
                if (sm_io->getResult() == icancloud_OK){
                    executeCPU = true;
                    executeWrite = false;
                    executeRead = false;
                    delete (sm);
                }

                // File not found!
                else if (sm_io->getResult() == icancloud_FILE_NOT_FOUND){
                    osStream << "File not found!";
                    isError = true;
                }

                // File not found!
                else if (sm_io->getResult() == icancloud_DATA_OUT_OF_BOUNDS){
                    executeCPU = true;
                }

                // Unknown result!
                else{
                    osStream << "Unknown result value:" << sm_io->getResult();
                    isError = true;
                }
            }


            // Write response!
            else if (operation == SM_WRITE_FILE){
                // All ok!
                if (sm_io->getResult() == icancloud_OK){
                    executeCPU = false;
                    executeWrite = false;
                    executeRead = true;
                    currentIteration++;
                    delete (sm);
                }

                // File not found!
                else if (sm_io->getResult() == icancloud_FILE_NOT_FOUND){
                    osStream << "File not found!";
                    isError = true;
                }

                // File not found!
                else if (sm_io->getResult() == icancloud_DISK_FULL){
                    osStream << "Disk full!";
                    isError = true;
                }

                // Unknown result!
                else{
                    osStream << "Unknown result value:" << sm_io->getResult();
                    isError = true;
                }
            }

            // Unknown I/O operation
            else{
                osStream << "Unknown received response message";
                isError = true;
            }

            // Increase total time for I/O
            total_service_IO += (endServiceIO - startServiceIO);
        }


        // Response came from CPU system
        else if (sm_cpu != NULL){

            // Get time!
            endServiceCPU = simTime ();

            // CPU!
            if (operation == SM_CPU_EXEC){
                executeCPU = false;
                executeWrite = true;
                executeRead = false;
                delete (sm);
            }

            // Unknown CPU operation
            else{
                osStream << "Unknown received response message";
                isError = true;
            }

            // Increase total time for I/O
            total_service_CPU += (endServiceCPU - startServiceCPU);
        }

        // Wrong response message!
        else{

            osStream << "Unknown received response message";
            isError = true;
        }

            // Error?
            if (isError){

                showErrorMessage ("Error in response message:%s. %s",
                                    osStream.str().c_str(),
                                    sm_io->contentsToString(true).c_str());
            }

            // CPU?
            else if (executeCPU){

                // Execute CPU!
                executeCPUrequest ();
            }

            // IO?
            else if ((executeRead) || (executeWrite)){

                if ((executeRead) && (currentIteration > iterations))
                    printResults();
                else
                    executeIOrequest(executeRead, executeWrite);
            }

            // Inconsistency error!
            else
                showErrorMessage ("Inconsistency error!!!! :%s. %s",
                                    osStream.str().c_str(),
                                    sm->contentsToString(true).c_str());
} 


void Container_CPU_Intensive::changeState(string newState){

}


void Container_CPU_Intensive::executeIOrequest(bool executeRead, bool executeWrite){

    // Reset timer!
    startServiceIO = simTime();

    // Executes I/O operation
    if (executeRead){

        if ((readOffset+(inputSizeMB*MB))>=MAX_FILE_SIZE)
            readOffset = 0;

        if (DEBUG_Application)
            showDebugMessage ("[%d/%d] Executing (Read) Offset:%d; dataSize:%d", currentIteration, iterations, readOffset,  inputSizeMB*MB);

        icancloud_request_read (INPUT_FILE, readOffset, inputSizeMB*MB);
        readOffset += (inputSizeMB*MB);


    }
    else{

        if ((writeOffset+(outputSizeMB*MB))>=MAX_FILE_SIZE)
            writeOffset = 0;

        if (DEBUG_Application)
            showDebugMessage ("[%d/%d] Executing (Write) Offset:%d; dataSize:%d", currentIteration, iterations, writeOffset,  outputSizeMB*MB);

        icancloud_request_write (OUTPUT_FILE, writeOffset, outputSizeMB*MB);
        writeOffset += (outputSizeMB*MB);
    }
}


void Container_CPU_Intensive::executeCPUrequest(){

    // Debug?
    if (DEBUG_Application)
        showDebugMessage ("[%d/%d] Executing (CPU) MIs:%d", currentIteration, iterations, MIs);

    // Reset timer!
    startServiceCPU = simTime ();
    icancloud_request_cpu (MIs);
}


void Container_CPU_Intensive::printResults (){

    std::ostringstream osStream;

    //Init..
        simEndTime = simTime();
        runEndTime = time (NULL);

    //Assign values to the results
        osStream <<  total_service_IO.dbl();
             Container_JobResults->setJobResult(0, osStream.str());
             osStream.str("");

             osStream <<  total_service_CPU.dbl();
             Container_JobResults->setJobResult(1, osStream.str());
             osStream.str("");

             osStream <<  difftime (runEndTime,runStartTime);
             Container_JobResults->setJobResult(2, osStream.str());
             osStream.str("");

             osStream << (simEndTime - simStartTime).dbl();
             Container_JobResults->setJobResult(3, osStream.str());

             addResults(Container_JobResults);

         //Send results list to the cloudManager
             userPtr->notify_UserContainerJobHasFinished(this);
}


