/*
 * @class CfgCloud CfgCloud.h "CfgCloud.h"
 *
 * Module to contains all the virtual machine images data that a cloud system offers to tenants
 *
 * @author Gabriel Gonzalez Castane
 * @date 2013-05-04
 */

#ifndef CFGCLOUD_H_
#define CFGCLOUD_H_

#include "cfgDataCenter.h"

class CfgCloud : public CfgDataCenter {

protected:

    // VM Instances structure
		struct vmStructure{
			string vmtype;					   		//Type VM (TypeVMID)
			int numCores;
			double memorySize_MB;
			double storageSize_GB;
		};

		vector <vmStructure*> vms;

public:
	CfgCloud();
	virtual ~CfgCloud();

    void setVMType (string vmName, int numCores, double memorySize_MB, double storageSize_GB);

	// To obtain the size of the main structures
	int getNumberOfVMTypes() {return vms.size();} ;

	// To obtain the parameters of a VM structure
	int getIndexForVM (string vmName);
	string getVMType (int index);
	int getNumCores (int index);
    int getMemorySize (int index);
    int getStorageSize (int index);

protected:
	void printVms();


};

#endif /* CFGCLOUD_H_ */
