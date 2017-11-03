cplusplus {{

#include <list>
#include <string>
#include "icancloud_types.h"
	
	
   /**
	* Structure that represents a trace piece.
	*/
	struct TraceComponent{
		int moduleID;						/**< Module ID */
		int gateID;							/**< Gate ID to moduleID */
		std::list <reqNum_t> reqSequence;	/**< Requests sequence */
		
		
	   /**
		* Assignment operator
		* @param other Trace component
		*/
		TraceComponent& operator=(const TraceComponent& other);	
		
	   /**
		* Constructor
		* @param newModule Module ID
		* @param newGate Gate ID		
		*/
		TraceComponent (int newModule, int newGate); 
		
	   /**
		* Constructor
		* @param newModule Module ID
		* @param newGate Gate ID
		* @param newReq Request number		
		*/
		TraceComponent (int newModule, int newGate, reqNum_t newReq);		
	};
}}



struct TraceComponent;



///
/// icancloud Message Base definition
///
message icancloud_Message extends cPacket
{
    @customize(true);

    unsigned int operation = -1;						// Operation type
    bool isResponse = false;							// Is a response message?
    bool remoteOperation = false;						// Is a remote operation?		

    int connectionId = -1;								// Connection ID (Used in remote operation between nodes)
    int commId = -1;									// Communication ID (Used to locate a communication with server)
    unsigned int sourceId = NULL_PROCESS_ID;			// ID from the source process

    unsigned int nextModuleIndex = -1;					// Array index destination module.				
    int result = 0;										// Operation result.

    abstract TraceComponent trace[];					// Message trace		
    
    int uid = -1;										// user id
    int pid = -1;										// processId id	
}


