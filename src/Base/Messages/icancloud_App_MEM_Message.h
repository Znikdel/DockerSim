#ifndef _icancloud_APP_MEM_MESSAGE_H_
#define _icancloud_APP_MEM_MESSAGE_H_

#include "icancloud_App_MEM_Message_m.h"


/**
 * @class icancloud_App_MEM_Message icancloud_App_MEM_Message.h "icancloud_App_MEM_Message.h"
 *
 * Class that represents a icancloud_App_MEM_Message.
 *
 * @author Alberto N&uacute;&ntilde;ez Covarrubias
 * @date 02-10-2007
 */
class icancloud_App_MEM_Message: public icancloud_App_MEM_Message_Base{


	public:

	   /**
		* Destructor.
		*/
		virtual ~icancloud_App_MEM_Message();

	   /**
		* Constructor of icancloud_Message
		* @param name Message name
		* @param kind Message kind
		*/
		icancloud_App_MEM_Message (const char *name=NULL, int kind=0);


	   /**
		* Constructor of icancloud_Message
		* @param other Message
		*/
		icancloud_App_MEM_Message(const icancloud_App_MEM_Message& other);


	   /**
		* = operator
		* @param other Message
		*/
		icancloud_App_MEM_Message& operator=(const icancloud_App_MEM_Message& other);


	   /**
		* Method that makes a copy of a icancloud_Message
		*/
		virtual icancloud_App_MEM_Message *dup() const;


		virtual icancloud_Message *dupGeneric() const;


	   /**
		* Update the message length
		*/
		void updateLength ();
   

	   /**
		* Parse all parameters of current message to string.
		* @param printContents Print message contents.
		* @return String with the corresponding contents.
		*/
		virtual string contentsToString (bool printContents);


	   /**
		* 
		* Parse a memory region to string format
		* 
		* @param region Memory region
		* @return Memory region in string format
		*  
		*/
		virtual string regionToString (int region);


	   /**
		* Serializes a icancloud_Message.
		* @param b Communication buffer
		*/
		virtual void parsimPack(cCommBuffer *b);


	   /**
		* Deserializes a icancloud_Message.
		* @param b Communication buffer
		*/
		virtual void parsimUnpack(cCommBuffer *b);
};

#endif
