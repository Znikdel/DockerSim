#ifndef _icancloud_MIGRATION_MESSAGE_H_
#define _icancloud_MIGRATION_MESSAGE_H_

#include "icancloud_Migration_Message_m.h"
#include "TCPSocket.h"
using std::ostringstream;


/**
 * @class icancloud_Migration_Message icancloud_Migration_Message.h "icancloud_Migration_Message.h"
 *   
 * Class that represents a icancloud_Migration_Message.
 * 
 * @author Gabriel GonzN&aacute;lez Casta&ntilde;&eacute;
 */
class icancloud_Migration_Message: public icancloud_Migration_Message_Base{
	
			
	public:

	   /**
		* Destructor.
		*/
		virtual ~icancloud_Migration_Message();

	   /**
		* Constructor of icancloud_Message
		* @param name Message name
		* @param kind Message kind		* 
		*/
		icancloud_Migration_Message (const char *name=NULL, int kind=0);
		
		
	   /**
		* Constructor of icancloud_Message
		* @param other Message		 
		*/
		icancloud_Migration_Message(const icancloud_Migration_Message& other);
		
		
	   /**
		* = operator
		* @param other Message		 
		*/
		icancloud_Migration_Message& operator=(const icancloud_Migration_Message& other);
		
		
	   /**
		* Method that makes a copy of a icancloud_Message
		*/
		virtual icancloud_Migration_Message *dup() const;
						
};

#endif
