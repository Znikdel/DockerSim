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
//g
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#include "AbstractCloudManager.h"

AbstractCloudManager::~AbstractCloudManager() {

    requestsQueue.clear();
    migrationVector.clear();
    pendingStorageRequests.clear();
    pendingRemoteStorageDeletion.clear();
    connectionsDeletion.clear();


}

void AbstractCloudManager::initialize(){
 //   if (DEBUG_CLOUD_SCHED) printf("\n Method[AbstractCloudManager]: -------> initialize\n");

    // Define ..
        string initialVIP;
        cModule* networkManagerMod;

    // Initialize the migration parameters
        dirtyingRate = par ("dirtyingRate");
        migrationVector.clear();

        initialVIP = par("virtualIPsBasis").stringValue();

        networkManagerMod = getParentModule()->getSubmodule("networkManager");
        networkManager = check_and_cast <NetworkManager*> (networkManagerMod);
        networkManager->setIPBasis(initialVIP.c_str());

    // Create the cfgCloud object!
        CfgCloud* cfg;
        cfg = new CfgCloud();

        cModule* vmSet = getParentModule()->getSubmodule("vmSet");
        if (vmSet == NULL) throw cRuntimeError ("AbstractCloudManager::initialize() -> Error during initialization. There is no vmSet\n");

        int vmSetSize = vmSet->par("vmImageQuantity").longValue();
        for (int i = 0; i < vmSetSize; i++){
            cModule* vmImage = vmSet->getSubmodule("vmImage",i);

            cfg->setVMType(vmImage->par("id").stringValue(),
                           vmImage->par("numCores").longValue(),
                           vmImage->par("memorySize_MB").doubleValue(),
                           vmImage->par("storageSize_GB").doubleValue()
                           );

        }

        dataCenterConfig = check_and_cast<CfgDataCenter*>(cfg);

        cModule* topology = getParentModule()->getSubmodule("topology");
        if (topology == NULL) throw cRuntimeError ("AbstractCloudManager::initialize() -> Error during initialization. There is no topology\n");

        int computeSize = topology->par("computeNodeQuantity").longValue();
    //    cout << "computeSize"<<computeSize<<endl;
        for (int i = 0; i < computeSize; i++){
            cModule* computeNodeMod = topology->getSubmodule("computeNode",i);
            dataCenterConfig->setNodeType(computeNodeMod->par("id").stringValue(), computeNodeMod->par("quantity").longValue());
        }

        int storageSize = topology->par("storageNodeQuantity").longValue();

        for (int i = 0; i < storageSize; i++){
            cModule* storageNodeMod = topology->getSubmodule("storageNode",i);
            dataCenterConfig->setStorageNodeType(storageNodeMod->par("id").stringValue(),storageNodeMod->par("quantity").longValue());
        }

        configDone = true;

        reqPendingToDelete.clear();

        AbstractDCManager::initialize();

        configDone = false;
   //     if (DEBUG_CLOUD_SCHED) printf("\n Method[AbstractCloudManager]: -------> initialize-----------------FIN------\n");

}

void AbstractCloudManager::initManager (int totalNodes){
    if (DEBUG_CLOUD_SCHED) printf("\n Method[AbstractCloudManager]: -------> initManager\n");

    migrationActive = false;

       // Define.
           std::ofstream line;
           string file;
           int i;
           int computeNodeSetSize = 0;
           int storageSetSize = 0;
           if (totalNodes == -1) totalNodes = 0;
           CfgCloud* config = check_and_cast<CfgCloud*>(dataCenterConfig);
       // Definition for parse
           // Nodes
               vector<string> nodeNames;
               string nodeTypeName;
               HeterogeneousSet* hetNodeSet;

       // Define auxiliar variables to link the module to the object
            cModule* nodeMod;
            string nodeName;
            Node* nodeChecked;
            Machine* machine;

       // Create the cfgCloud object!

            if (!isCfgLoaded()){

                // Initialize structures and parameters
                    nodesMap = new MachinesMap();
                    storage_nodesMap = new MachinesMap();

                // Get the Nodes

                    computeNodeSetSize = config->getNumberOfNodeTypes();

                // Memorization
                   MemoSupport* cpu;
                   MemoSupport* memory;
                   MemoSupport* storage;
                   MemoSupport* network;
                   string component;

                   bool componentsLoaded;

                   // Memorization init
                       cpus.clear();
                       memories.clear();
                       storages.clear();
                       networks.clear();
                       cpu = NULL;
                       memory = NULL;
                       storage = NULL;
                       network = NULL;

                    for (int j = 0; j < computeNodeSetSize; j++){

                        nodeNames = config->generateStructureOfNodes(j);

                        hetNodeSet = new HeterogeneousSet();

                        componentsLoaded = false;

                        // link all the created nodes by omnet in the vector nodeSet.
                          for (i = 0 ; i < (int)nodeNames.size(); i++){

                              nodeName = (*(nodeNames.begin() + i));

                              nodeMod = getParentModule()->getParentModule()->getModuleByPath(nodeName.c_str());

                              nodeChecked = check_and_cast<Node*>(nodeMod);
                              nodeChecked->initNode();

                              if ((memorization) && (!componentsLoaded)){
                                   componentsLoaded = true;
                                   bool found = false;
                                   // CPU
                                       component = nodeChecked->getCPUName();

                                       for (int k = 0; k < (int)cpus.size() && (!found); k++){
                                           if (strcmp ((*(cpus.begin() + k))->getComponentName().c_str(), component.c_str()) == 0){
                                               found = true;
                                               cpu = (*(cpus.begin() + k));
                                           }
                                       }

                                       if (!found){
                                           cpu = new MemoSupport( component );
                                           printf ("AbstractMeterUnit::loadMemo()-> %s\n", component.c_str());
                                           cpus.push_back(cpu);
                                       }


                                   // MEMORY
                                       component = nodeChecked->getMemoryName();
                                       found = false;

                                       for (int k = 0; k < (int)memories.size() && (!found); k++){
                                           if (strcmp ((*(memories.begin() + k))->getComponentName().c_str(), component.c_str()) == 0){
                                               found = true;
                                               memory = (*(memories.begin() + k));
                                           }
                                       }

                                       if (!found){
                                           memory = new MemoSupport( component );
                                           printf ("AbstractMeterUnit::loadMemo()-> %s\n", component.c_str());
                                           memories.push_back(memory);
                                       }

                                   // STORAGE
                                       component = nodeChecked->getStorageName();
                                       found = false;

                                       for (int k = 0; k < (int)storages.size() && (!found); k++){
                                           if (strcmp ((*(storages.begin() + k))->getComponentName().c_str(), component.c_str()) == 0){
                                               found = true;
                                               storage = (*(storages.begin() + k));
                                           }
                                       }

                                       if (!found){
                                           storage = new MemoSupport( component );
                                           printf ("AbstractMeterUnit::loadMemo()-> %s\n", component.c_str());
                                           storages.push_back(storage);
                                       }
                                   // NETWORK
                                       component = nodeChecked->getNetworkName();
                                       found = false;

                                       for (int k = 0; k < (int)networks.size() && (!found); k++){
                                           if (strcmp ((*(networks.begin() + k))->getComponentName().c_str(), component.c_str()) == 0){
                                               found = true;
                                               network = (*(networks.begin() + k));
                                           }
                                       }

                                       if (!found){
                                           network = new MemoSupport( component );
                                           printf ("AbstractMeterUnit::loadMemo()-> %s\n", component.c_str());
                                           networks.push_back(network);
                                       }
                               }
                              nodeChecked->loadMemo(cpu,memory,storage,network);

                              string state = (nodeMod->par("initialState").stringValue());
                              if (strcmp(state.c_str(), MACHINE_STATE_OFF) == 0) nodeChecked->turnOff();
                              else nodeChecked->turnOn();

                              if (i == 0){
                                  hetNodeSet->setElementType(nodeChecked->getElementType());
                              }

                              machine = check_and_cast<Machine*>(nodeChecked);
                              hetNodeSet->initMachine(machine);
                              totalNodes++;
                          }

                    nodesMap->setInstances(hetNodeSet);

                    }

                // Get the Storage Nodes
                    storageSetSize = config->getNumberOfStorageNodeTypes();

                    for (int j = 0; j < storageSetSize; j++){

                        nodeNames = config->generateStructureOfStorageNodes(j);

                        hetNodeSet = new HeterogeneousSet();

                        componentsLoaded = false;

                        // link all the created nodes by omnet in the vector nodeSet.
                          for (i = 0 ; i < (int)nodeNames.size(); i++){

                              nodeName = (*(nodeNames.begin() + i));
                              nodeMod = getParentModule()->getParentModule()->getModuleByPath(nodeName.c_str());

                              nodeChecked = check_and_cast<Node*>(nodeMod);
                              nodeChecked->initNode();

                              if ((memorization) && (!componentsLoaded)){
                                   componentsLoaded = true;
                                   bool found = false;
                                   // CPU
                                       component = nodeChecked->getCPUName();

                                       for (int k = 0; k < (int)cpus.size() && (!found); k++){
                                           if (strcmp ((*(cpus.begin() + k))->getComponentName().c_str(), component.c_str()) == 0){
                                               found = true;
                                               cpu = (*(cpus.begin() + k));
                                           }
                                       }

                                       if (!found){
                                           cpu = new MemoSupport( component );
                                           printf ("AbstractMeterUnit::loadMemo()-> %s\n", component.c_str());
                                           cpus.push_back(cpu);
                                       }

                                   // MEMORY
                                       component = nodeChecked->getMemoryName();
                                       found = false;

                                       for (int k = 0; k < (int)memories.size() && (!found); k++){
                                           if (strcmp ((*(memories.begin() + k))->getComponentName().c_str(), component.c_str()) == 0){
                                               found = true;
                                               memory = (*(memories.begin() + k));
                                           }
                                       }

                                       if (!found){
                                           memory = new MemoSupport( component );
                                           printf ("AbstractMeterUnit::loadMemo()-> %s\n", component.c_str());
                                           memories.push_back(memory);
                                       }


                                   // STORAGE
                                       component = nodeChecked->getStorageName();
                                       found = false;

                                       for (int k = 0; k < (int)storages.size() && (!found); k++){
                                           if (strcmp ((*(storages.begin() + k))->getComponentName().c_str(), component.c_str()) == 0){
                                               found = true;
                                               storage = (*(storages.begin() + k));
                                           }
                                       }

                                       if (!found){
                                           storage = new MemoSupport( component );
                                           printf ("AbstractMeterUnit::loadMemo()-> %s\n", component.c_str());
                                           storages.push_back(storage);
                                       }

                                   // NETWORK
                                       component = nodeChecked->getNetworkName();
                                       found = false;

                                       for (int k = 0; k < (int)networks.size() && (!found); k++){
                                           if (strcmp ((*(networks.begin() + k))->getComponentName().c_str(), component.c_str()) == 0){
                                               found = true;
                                               network = (*(networks.begin() + k));
                                           }
                                       }

                                       if (!found){
                                           network = new MemoSupport( component );
                                           printf ("AbstractMeterUnit::loadMemo()-> %s\n", component.c_str());
                                           networks.push_back(network);
                                       }
                              }

                              nodeChecked->loadMemo(cpu,memory,storage,network);

                              string state = (nodeMod->par("initialState").stringValue());
                              if (strcmp(state.c_str(), MACHINE_STATE_OFF) == 0) nodeChecked->turnOff();
                              else nodeChecked->turnOn();

                              if (i == 0){
                                  hetNodeSet->setElementType(nodeChecked->getElementType());
                              }

                              machine = check_and_cast<Machine*>(nodeChecked);
                              hetNodeSet->initMachine(machine);
                              totalNodes++;
                          }

                        storage_nodesMap->setInstances(hetNodeSet);
                    }
                    cfgLoaded();
                }

       AbstractDCManager::initManager(totalNodes);
       if (DEBUG_CLOUD_SCHED) printf("\n Method[AbstractCloudManager]: -------> initManager--------FIN-----\n");

}

void AbstractCloudManager::finalizeManager(){

    // finalize the scheduler
   finalizeScheduler();

    AbstractDCManager::finalizeDCManager();

}

VM* AbstractCloudManager::getVmFromUser(int uId, int pId){
    AbstractCloudUser* userCl;
    AbstractUser* user;
    user = getUserById(uId);
    userCl = check_and_cast<AbstractCloudUser*>(user);
    VM* vm = userCl->searchVmByPid(pId);
    return vm;
}

AbstractCloudUser* AbstractCloudManager::getUserFromId(int uId){
    AbstractCloudUser* userCl;
    AbstractUser* user;
    user = getUserById(uId);
    userCl = check_and_cast<AbstractCloudUser*>(user);

    return userCl;
}

bool AbstractCloudManager::request_start_vm (RequestVM* req){
    if (DEBUG_CLOUD_SCHED) printf("\n Method[AbstractCloudManager]: -------> request_start_vm\n");

    //Define ...
        AbstractNode* selectedNode;
        vector<VM*> attendedRequest_vms;
        AbstractUser* user;
        NodeVL* nodeVL;

        RequestVM* attendedRequest;

        VM* vm;
        VM* vmNew;
        string uid;

        bool notEnoughResources;
        int operation;
        int vmQuantity = 0;

        cModule  *vmImage;

        // Init ..
        notEnoughResources = false;
        operation = REQUEST_NOP;
        selectedNode = NULL;
        attendedRequest_vms.clear();

    for (int i = 0; ((i < req->getDifferentTypesQuantity()) && (!notEnoughResources));){

        vmQuantity = req->getSelectionQuantity(i);
        if (DEBUG_CLOUD_SCHED) cout<< "vmQuantity--->"<<vmQuantity<<endl;

        for (int j = 0; (j < vmQuantity) && !notEnoughResources; j++ ){

            // Get the original virtual machine image
            vmImage = getSubmodule("vmImage");

            vm = dynamic_cast<VM*>(vmImage);
            if (DEBUG_CLOUD_SCHED) cout<< " Method[AbstractCloudManager]: ------->vmCPUs--->"<<vm->getNumCores()<<endl;
                if (DEBUG_CLOUD_SCHED) cout<< " Method[AbstractCloudManager]: ------->vmMemory--->"<<vm->getMemoryCapacity()<<endl;
                if (DEBUG_CLOUD_SCHED) cout<< " Method[AbstractCloudManager]: ------->vmStorage--->"<<vm->getStorageCapacity()<<endl;
            // Create the request for scheduling selecting node method
            RequestVM* reqSch;
            AbstractRequest* reqA;

            reqSch = req->dup();

            reqSch->cleanSelectionType();
            reqSch->setForSingleRequest(vm->getElementType());

            reqA = check_and_cast<AbstractRequest*>(reqSch);

            //erase the rest of vms less the actual

            selectedNode = selectNode(reqA);

            delete (reqSch);

            operation = req->getOperation();

            if (operation == REQUEST_ERROR){
                //TODO--

            } else if (selectedNode == NULL){ // There are not a node to allocate the request!
                if (DEBUG_CLOUD_SCHED) printf("\n Method[AbstractCloudManager]: -------> Selected Node is NULL\n");

                // Reenqueue to wait until exists enough resources.
                req->incrementTimesEnqueue();
                req->setState(REQUEST_PENDING);
                req->incrementTimesEnqueue();
                notEnoughResources = true;

            } // everything is ok.
            else {
                if (DEBUG_CLOUD_SCHED) printf("\n Method[AbstractCloudManager]: -------> Selected Node is --->%s\n",selectedNode->getFullName());

                std::ostringstream vmName;
                vmName << vm->getName();

                // Start the node (if it is off)
                    if (!(selectedNode->isON())){
                        selectedNode->turnOn();
                    }

                // link the vm to the node
                    nodeVL = check_and_cast<NodeVL*>(selectedNode);

                    // Create the vm as image of vm image.
                    vmNew = create_VM (vm, req->getSelectionType(0).c_str(), nodeVL->getHypervisor());

                    vmNew->setUid(req->getUid());

                    vmName << req->getSelectionType(0).c_str() <<":u" << req->getUid() << ":p" << vmNew->getPid() << "";
                    vmNew->cModule::setName(vmName.str().c_str());
                    vmNew->setName(vmName.str().c_str());

                    nodeVL->testLinkVM (vmNew->getNumCores(), vmNew->getMemoryCapacity(), vmNew->getStorageCapacity(), vmNew->getNumNetworkIF(), vmNew->getTypeName(), vmNew->getUid(), vmNew->getPid());

                    linkVM (nodeVL, vmNew);

                // If the linked is incorrect
                    req->decreaseSelectionQuantity(i);
                    attendedRequest_vms.insert(attendedRequest_vms.end(), vmNew);
             }
        }
        if (!notEnoughResources) req->eraseSelectionType(i);
    }
    // Send all attendeed requests
    attendedRequest = new RequestVM(req -> getUid(), REQUEST_START_VM, attendedRequest_vms);
    if (notEnoughResources){

        // At least one vm has been allocated
        if (attendedRequest_vms.size() > 0){
            attendedRequest->setState (REQUEST_PENDING);
            user = getUserByModuleID(req -> getUid());
            user -> notify_UserRequestAttendeed(attendedRequest);
        }
        else{
            delete(attendedRequest);
        }

    }
    // All the request has been allocated.
    else{
        attendedRequest->setState (REQUEST_SUCCESS);
        user = getUserByModuleID(req -> getUid());
        user -> notify_UserRequestAttendeed(attendedRequest);
    }
    if (DEBUG_CLOUD_SCHED) printf("\n Method[AbstractCloudManager]: -------> request_start_vm-------FIN-------------\n");

    return notEnoughResources;

}

void AbstractCloudManager::request_shutdown_vm(RequestVM* req){
    if (DEBUG_CLOUD_SCHED) printf("\n Method[AbstractCloudManager]: -------> request_shutdown_vm--------------------\n");

    // Define ..
        VM* vm;
        vector<AbstractNode*> nodes;
        RequestVM* req_copy;
        AbstractRequest* base;

    // Init ..
        nodes.clear();

    // Get the first VM
        while (req->getVMQuantity() != 0){

            vm = req->getVM(0);

            // Dup the request to storage in the queue waiting for format FS and close connections

                req_copy = req->dup();

            //erase the rest of vms less the actual
                if (req_copy->getVMQuantity() > 1){
                    while (req_copy->getVMQuantity() != 1)
                        req_copy->eraseVM(1);
                }

                reqPendingToDelete.push_back(req_copy);

                base = dynamic_cast<AbstractRequest*>(req_copy);
                if (base == NULL) throw cRuntimeError("AbstractCloudManager::notify_shutdown_vm->Error(req_copy). Casting the request\n");

            nodes = remoteShutdown (base);
            nodes.push_back(getNodeByIndex(vm->getNodeSetName(), vm->getNodeName()));

            // Remote storage control
            formatFSFromNodes (nodes, vm->getUid(), vm->getId(), shutdownNodeIfIDLE());
            // connection control
            closeVMConnections (nodes, vm);

            // Delete the first VM and proceed if exists any one.
            req->eraseVM(0);

        }
        if (DEBUG_CLOUD_SCHED) printf("\n Method[AbstractCloudManager]: -------> request_shutdown_vm-------FIN-------------\n");

}

void AbstractCloudManager::linkVM (AbstractNode* node, VM* vm){

	// link the node place to the base vm

		linkVMInternals (node,vm, false);

        vm->changeState(MACHINE_STATE_OFF);
        vm->setPendingOperation(NOT_PENDING_OPS);

}

void AbstractCloudManager::unlinkVM(AbstractNode* node, VM* vm, bool turnOff){
    if (DEBUG_CLOUD_SCHED) printf("\n Method[AbstractCloudManager]: -------> unlinkVM-------------------\n");

    vm->shutdownVM();

    unlinkVMInternals(node, vm, false);

    for(int k = 0; k < vm->gateCount();k++){
         vm->gateByOrdinal(k)->disconnect();
    }

    vm->callFinish();

    if (turnOff){
        NodeVL* nodeVL = check_and_cast<NodeVL*>(node);
        if ((nodeVL->getNumProcessesRunning() == 0) && (nodeVL->getNumVMAllocated() == 0))
            node->turnOff();
    }
    if (DEBUG_CLOUD_SCHED) printf("\n Method[AbstractCloudManager]: -------> unlinkVM-------FIN-------------\n");

}

void AbstractCloudManager::closeVMConnections (vector<AbstractNode*> nodes, VM* vm){

        // Define ..
        std::ostringstream user_vm;
        unsigned int i;
        string nodeSetName;
        NodeVL* node;
        AbstractCloudManager::PendingConnectionDeletion* pendingConnectionUnit;

        // Init ..
        // Create the pending remote storage deletion element
            pendingConnectionUnit = new AbstractCloudManager::PendingConnectionDeletion();
            pendingConnectionUnit->uId = vm->getUid();
            pendingConnectionUnit->pId = vm->getPid();


        // The remote nodes and the local.
            pendingConnectionUnit->connectionsQuantity = nodes.size();

        // Insert into the pendingRemoteStorageDeletion vector
            connectionsDeletion.push_back(pendingConnectionUnit);

        // The vm has remote storage

            if (nodes.size() != 0){
                for (i = 0; i < nodes.size(); i++){

                   node = dynamic_cast<NodeVL*> ((*(nodes.begin()+i)));

                   if (node == NULL) throw cRuntimeError ("AbstractCloudManager::closeVMConnections->Abstract node cannot be casted to NodeVL\n");
                   node->closeConnections(vm->getUid(), vm->getPid());
                }

            }
}

VM* AbstractCloudManager::create_VM (VM* vmImage, string vmName, cModule* parent){

	//Define ...

		cModule *cloneVm;
		std::ostringstream vmPath;
		string parameter, nedFolder;
		int i, numParameters;
	// Init ..


	//  into vmImage the module to clone
        CfgCloud* cfg;
        cfg = check_and_cast<CfgCloud*>(dataCenterConfig );

	// Here the kind of module is taken to create the module as image..
		vmPath << vmImage->getNedTypeName();

	//create the vm module
		cModuleType *modVMType = cModuleType::get (vmPath.str().c_str());

		//I create the VM out of the module.
		cloneVm = modVMType->create(vmImage->getTypeName().c_str(), parent);

	//configure the main parameters
		numParameters = vmImage->getNumParams();
		for (i = 0; i < numParameters ; i++){
			cloneVm->par(i) = vmImage->par(i);
		}

		int position = cfg->getIndexForVM(vmName.c_str());
		cloneVm->par("numCores") = cfg->getNumCores(position);
        cloneVm->par("memorySize_MB") = cfg->getMemorySize(position);
        cloneVm->par("storageSize_GB") = cfg->getStorageSize(position);

	//finalize and build the module
		cloneVm->finalizeParameters();
		cloneVm->buildInside();

		VM* vm;
		vm = dynamic_cast<VM*>(cloneVm);
		vm->callInitialize();

		return vm;
}

void AbstractCloudManager::linkVMInternals (AbstractNode* node, VM* vm, bool migration){

        // Define ...
            unsigned int i;

            string ipNode;
            string virtualIP;
            string virtualIPAux;
            cGate** fromNodeCPU;
            cGate** toNodeCPU;
            cGate* fromNodeMemoryI;
            cGate* fromNodeMemoryO;
            cGate* toNodeMemoryI;
            cGate* toNodeMemoryO;
            cGate* toNodeNet;
            cGate* fromNodeNet;
            cGate* fromNodeStorageSystem;
            cGate* toNodeStorageSystem;
            NodeVL* nodeVL;

        // Init ..
            virtualIPAux = "";

            nodeVL = dynamic_cast<NodeVL*>(node);
            if (nodeVL == NULL) throw cRuntimeError("AbstractCloudManager::unlinkVMInternals->can not dynamic casted to NodeVL\n");

            //link the VM to the networkManager
                ipNode = nodeVL->par("ip").stringValue();

            // Build the vmName

            if (!migration){

                    if (!ipNode.empty()){

                        virtualIPAux = vm->par("ip").stdstringValue();

                        if (virtualIPAux.empty()){
                            virtualIP = networkManager->allocateVirtualIP(ipNode,vm->getUid(),vm->getPid());
                        } else {
                            virtualIP = networkManager->changeNodeIP(virtualIPAux,vm->getUid(),ipNode);
                        }

                        vm->par("ip").setStringValue(virtualIP.c_str());
                    }
                    vm->setIP(virtualIP);

            } else {
                virtualIP = vm->par("virtualIP").stringValue();
                virtualIPAux= networkManager->changeNodeIP(virtualIP, vm->getUid(), ipNode);
            }

        // Connect the VM to the Hypervisor

            fromNodeMemoryI = NULL;
            fromNodeMemoryO = NULL;
            toNodeMemoryI = NULL;
            toNodeMemoryO = NULL;
            toNodeNet = NULL;
            fromNodeNet = NULL;
            fromNodeStorageSystem = NULL;
            toNodeStorageSystem = NULL;
            toNodeCPU = new cGate* [vm->getNumCores()];
            fromNodeCPU = new cGate* [vm->getNumCores()];
            // Get VM the gates..
                // CPU
                for (i = 0 ; i < (unsigned int)vm->getNumCores() ; i++){
                    fromNodeCPU [i] = (vm->gate("fromNodeCPU",i));
                    toNodeCPU [i] = (vm->gate("toNodeCPU",i));
                }

                // Memory
                    fromNodeMemoryI = vm->gate("fromNodeMemoryI");
                    toNodeMemoryI = vm->gate("toNodeMemoryI");
                    fromNodeMemoryO = (vm->gate("fromNodeMemoryO"));
                    toNodeMemoryO = (vm->gate("toNodeMemoryO"));

                // Net
                    fromNodeNet = vm->gate("fromNodeNet");
                    toNodeNet = vm->gate("toNodeNet");

                // Storage
                    fromNodeStorageSystem = vm->gate("fromNodeStorageSystem",0);
                    toNodeStorageSystem = vm->gate("toNodeStorageSystem",0);

                    nodeVL -> NodeVL::linkVM(fromNodeCPU, toNodeCPU,
                                     fromNodeMemoryI, toNodeMemoryI, fromNodeMemoryO, toNodeMemoryO,
                                     fromNodeNet, toNodeNet,
                                     fromNodeStorageSystem,toNodeStorageSystem,
                                     vm->getNumCores(), vm->getIP(),vm->getMemoryCapacity(),
                                     vm->getStorageCapacity(),vm->getUid(), vm->getPid());

            // link the node place to the base vm
                    vm->setNodeName(node->getIndex());
                    vm->setNodeSetName(node->getName());

            nodeVL->setVMInstance(vm);


}

void  AbstractCloudManager::unlinkVMInternals (AbstractNode* node, VM* vm, bool migration){
    if (DEBUG_CLOUD_SCHED) printf("\n Method[AbstractCloudManager]: -------> unlinkVMInternals-------------------\n");

    // Define ..
        std::ostringstream msgLine;
        string virtualIP;
        vector<int> gates;
        vector<int>::iterator gatesIt;
        NodeVL* nodeVL;

    // Init ..

        nodeVL = dynamic_cast<NodeVL*>(node);
        if (nodeVL == NULL) throw cRuntimeError("AbstractCloudManager::unlinkVMInternals->can not dynamic casted to NodeVL\n");

        if (!migration){
            // Unlink from network manager
            virtualIP = vm->par("ip").stringValue();
            networkManager->deleteVirtualIP_by_VIP(virtualIP, vm->getUid());
        }

        // Disconnect the VM to the Hypervisor

        if (vm->getNumProcessesRunning() != 0){
            throw cRuntimeError ("AbstractCloudManager::unlinkVMInternals -> the VM has jobs that are not finalized when the order of unlink it has been given\n");
        }

        else {
            nodeVL->unlinkVM(vm->getMemoryCapacity(), vm->getNumCores(), vm->getStorageCapacity(), virtualIP, vm->getUid(), vm->getId());
        }
        if (DEBUG_CLOUD_SCHED) printf("\n Method[AbstractCloudManager]: -------> unlinkVMInternals--------------FIN  -----\n");

}

void AbstractCloudManager::notifyManager(icancloud_Message* msg){

    int operation = msg->getOperation();

    if (operation == SM_NOTIFY_USER_FS_DELETED){
        notifyFSFormatted(msg->getUid(), msg->getPid(), shutdownNodeIfIDLE());

    } else if (operation == SM_NOTIFY_PRELOAD_FINALIZATION ){
        notifyStorageConnectionSuccesful(msg->getUid(), msg->getPid(), msg->getCommId());

    } else if (operation == SM_NOTIFY_USER_CONNECTIONS_CLOSED ){
        notifyVMConnectionsClosed(msg->getUid(), msg->getPid(), shutdownNodeIfIDLE() );

    } else {
        showErrorMessage("AbstractCloudManager::notifyManager-->operation unknown: %i", msg->getOperation());
    }
}

void AbstractCloudManager::notifyStorageConnectionSuccesful (int uId, int pId, int spId){

    // Define ..
        vector<PendingStorageRequest*>::iterator requestsIt;
        vector <processOperations*>::iterator processOp;
        vector <subprocessOperations*>::iterator subprocessOp;
        vector <connection_T*> connections;
        stringstream id;
        bool found;
        bool notify;
    // if the connection is the last
        StorageRequest* request;

    // Init ..
        requestsIt = pendingStorageRequests.begin();
        found = false;
        notify = false;

        // User
        for (requestsIt = pendingStorageRequests.begin(); (requestsIt < pendingStorageRequests.end()) && (!found);requestsIt++){

            if ((uId == (*requestsIt)->uId))

                // VM
                for (processOp = (*requestsIt)->processOperation.begin(); (processOp < (*requestsIt)->processOperation.end()) && (!found); processOp++){
                    if ((*(processOp))->pId == pId){

                        // App
                        for (subprocessOp = (*(processOp))->pendingOperation.begin(); (subprocessOp < (*(processOp))->pendingOperation.end()) && (!found); subprocessOp++){
                            if ((*subprocessOp)->spId == spId){


                                (*subprocessOp)->numberOfConnections = (*subprocessOp)->numberOfConnections-1;

                                found = true;

                                if ((*subprocessOp)->numberOfConnections == 0){

                                    notify = true;

                                    // Create the request to be used by the scheduler
                                    request = new StorageRequest();

                                    request->setOperation((*subprocessOp)->operation);
                                    request->setPid(pId);
                                    request->setUid(uId);
                                    request->setSPid(spId);
                                    // set the vm and the connection vector
                                    for (int j = 0; j < (int)(*subprocessOp)->pendingOperation.size(); j++){
                                        connection_t* connection = (*((*subprocessOp)->pendingOperation.begin()+j));
                                        request->setConnection(connection);
                                    }

                                    // delete the data from the pending requests
                                    (*(processOp))->pendingOperation.erase(subprocessOp);

                                    if ((*processOp)->pendingOperation.size() == 0){
                                        (*requestsIt)->processOperation.erase(processOp);

                                        if ((*requestsIt)->processOperation.size() == 0){
                                            pendingStorageRequests.erase(requestsIt);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

    if (!found){
        throw cRuntimeError ("AbstractCloudManager::notifyStorageConnectionSuccesful -> Notify storage connection succesful has been invoked and there is no conection pending \n");
    } else {
        if (notify){
            AbstractUser* user;
            AbstractCloudUser* userCl;
            user = getUserById(uId);
            userCl = check_and_cast<AbstractCloudUser*>(user);
            userCl->notify_UserRequestAttendeed(request);
        }
    }

 }

void AbstractCloudManager::notifyFSFormatted(int uId, int pId, bool turnOffNode){
    if (DEBUG_CLOUD_SCHED) printf("\n Method[AbstractCloudManager]: -------> notifyFSFormatted-------------------\n");

    // Define ..
        vector <PendingRemoteStorageDeletion*>::iterator pendingDelete;
        PendingConnectionDeletion* pendingConnectionUnit;
        AbstractNode* node;
        unsigned int i;
        int pendingConnections;
        int pendingRSD;

        string nodeSetName;

        bool found;
        bool pending_found;

    // Init ..
        found = false;
        pending_found = false;
        pendingConnections = 0;
        pendingRSD = 0;

    // Begin the search of the vm in the pending remote storage deletion vector
        for (pendingDelete = pendingRemoteStorageDeletion.begin(); (pendingDelete < pendingRemoteStorageDeletion.end()) && (!found); pendingDelete++){

            if (((*pendingDelete)->uId == uId) && ((*pendingDelete)->pId == pId)){

                found = true;

                // Decrement the waiting for remote storage filesystem formatting notifications
                (*pendingDelete)->remoteStorageQuantity--;
                pendingRSD = (*pendingDelete)->remoteStorageQuantity;

                // Check if it is the last connection and there are not pending connections..
                if ((*pendingDelete)->remoteStorageQuantity == 0) {

                    // Get if there are pending connections..
                    for (i = 0; (i < connectionsDeletion.size()) && (!pending_found);){
                        pendingConnectionUnit = (*(connectionsDeletion.begin() + i));

                        if ( (pendingConnectionUnit->uId == uId) && (pendingConnectionUnit->pId == pId)  ){
                            pendingConnections = pendingConnectionUnit->connectionsQuantity;
                            pending_found = true;
                        } else {
                            i++;
                        }
                    }
                }
                // It is possible to shutdown the vm without problems
                if (((pendingConnections == 0) || (!pending_found)) && (pendingRSD == 0))  {

                    // If pending_found, the pending connection unit is not needed anymore
                    if (pending_found)
                        connectionsDeletion.erase(connectionsDeletion.begin() + i);

                    // Operate with the remote storage..
                    if (pendingConnections == 0)
                        pendingRemoteStorageDeletion.erase(pendingDelete);


                    // notify the scheduler the shutdown of the vm
                        // Get the vm
                            VM* vm = getVmFromUser(uId, pId);

                            if (vm->getNumProcessesRunning() == 0){
                                // Free the resources of the node!
                                    node = getNodeByIndex (vm->getNodeSetName(), vm->getNodeName());

                                    unlinkVM(node, vm, turnOffNode);

                                // notify the scheduler the shutdown of the vm
                                    notify_shutdown_vm (uId, pId, node);
                            }
                }
            }
        }
        if (DEBUG_CLOUD_SCHED) printf("\n Method[AbstractCloudManager]: -------> notifyFSFormatted--------------FIN-----\n");

}

void AbstractCloudManager::notify_shutdown_vm (int uId, int pId, AbstractNode* node){

// Define ..
    RequestVM* req;
    unsigned int i;
    bool found;
    RequestVM* req_copy;
    AbstractUser* user;

// Init ..
    found = false;

// Begin ..

    for (i = 0; (!found) && (i < (reqPendingToDelete.size())); ){

        req =  (*(reqPendingToDelete.begin() + i));

        if ( (req->getUid() == uId) && (req->getPid() == pId) ){

            // copy the req;
                req_copy = req->dup();

                user = getUserByModuleID(req -> getUid());
                user -> notify_UserRequestAttendeed(req_copy);
//                node = getNodeByIndex(req_copy->getVM(0)->getNodeSetName(), req_copy->getVM(0)->getNodeName());

                reqPendingToDelete.erase(reqPendingToDelete.begin() + i);

            found = true;

        }
        else {
            i++;
        }

    }

    freeResources (uId, pId, node);

}

void AbstractCloudManager::notifyVMConnectionsClosed (int uId, int pId, bool turnOff){
    if (DEBUG_CLOUD_SCHED) printf("\n Method[AbstractCloudManager]: -------> notifyVMConnectionsClosed-------------------\n");

    // Define ..
        PendingConnectionDeletion* pendingConnectionUnit;
        vector<PendingConnectionDeletion*>::iterator it;
        vector <PendingRemoteStorageDeletion*>::iterator pendingDelete;
        bool pending_found;
        bool found;
        int pendingConnections;
        VM* vm;
        AbstractNode* node;
        string nodeSetName;
        int nodeName;
        std::ostringstream user_vm;
        int pendingRSD;

    // Init ..
        found = false;
        pending_found = false;
        pendingConnections = 0;
        pendingRSD = -1;


    // Get if there are pending connections..
        for (it = connectionsDeletion.begin(); (it < connectionsDeletion.end()) && (!found);){
            pendingConnectionUnit = (*(it));

            if ((pendingConnectionUnit->pId) == pId){
                pendingConnectionUnit->connectionsQuantity--;
                found = true;

                pendingConnections = pendingConnectionUnit->connectionsQuantity;
                // Check if it is the last connection and there are not pending connections..
                if (pendingConnections == 0){

                    // erase the pending entry in connections deletion
                    connectionsDeletion.erase(it);
                    pending_found = false;

                    for (pendingDelete = pendingRemoteStorageDeletion.begin(); (pendingDelete < pendingRemoteStorageDeletion.end()) && (!pending_found); ){

                        if (((*pendingDelete)->pId == pId) && ((*pendingDelete)->uId == uId)){

                            // Get the vm
                                vm = getVmFromUser(uId, pId);

                            // Set the pending found to control the case
                                pending_found = true;

                            // Operate with the remote storage..
                                pendingRSD = (*pendingDelete)->remoteStorageQuantity;
                               // It is possible to shutdown the vm without problems
                               if (pendingRSD == 0) {
                                   // Operate with the remote storage..
                                   pendingRemoteStorageDeletion.erase(pendingDelete);
                               }

                        } else {
                            pendingDelete++;
                        }
                    }
                }


                if (((pendingRSD == 0) || (!pending_found)) && (pendingConnections == 0)){

                    // Shutdown the vm
                       nodeSetName = vm->getNodeSetName();
                       nodeName = vm->getNodeName();

                       node = getNodeByIndex (nodeSetName, nodeName);

                       if (vm->getNumProcessesRunning() == 0){
                            // Free the resources of the node!
                               unlinkVM(node, vm, turnOff);

                            // notify the scheduler the shutdown of the vm
                              notify_shutdown_vm(uId, pId, node);
                       }
                }

            }
            else {
                it++;
            }

        }
        if (DEBUG_CLOUD_SCHED) printf("\n Method[AbstractCloudManager]: -------> notifyVMConnectionsClosed-----------FIN--------\n");

}

