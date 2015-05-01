/*----------------------
   Copyright (C): OpenGATE Collaboration

This software is distributed under the terms
of the GNU Lesser General  Public Licence (LGPL)
See GATE/LICENSE.txt for further details
----------------------*/



/*! \file
 
  \brief Class GateMessageManager and Macros for outputing messages in Gate

  There are 4 kinds of messages :
  - Messages (normal messages)
  - Debug messages (not compiled in release)
  - Warnings 
  - Errors

  There are also "types" of messages which are strings which identify
  the nature of the message (for example : "Core" messages are
  generated by the core classes of the library, there can be a type of
  message for each type of Node, and so on...)

  A type of message must be declared by registering it into the
  MessageManager. This is done by a line like :
  Gate::MessageManager::RegisterMessageType("Core","Messages generated
  by the core classes of the library",5); where :

  -The first string is the type of the message (the keyword which will
   be used to generate a message of this type)
  -The second string is help string
  -The integer is the initial level for the messages of this type (see
   below).
  
  To generate a message of a known type then use one of the macros :
  GateMessage, GateDebugMessage, GateWarning, GateError or their variants.

  example :

  GateMessage("Core",4,"problem with "<<GetName()<<Gateendl);

  will push the 3rd argument in std::cout if the message level of
  "Core" messages is greater or equal to 4.  which means that it
  generates a message of level 4 (0 : very important/always displayed
  ... 9 : deep debug message).

  At run time, one is able to change the level of the messages
  displayed by using a command like :
 
  Gate::MessageManager::SetMessageLevel("Core",5); 
  
  which tells the manager to display all Core messages of level up to 5.

  Variants :
  Gate*Cont : continues a previous GateMessage on the same line (without rewriting the type and level)
  Gate*Inc / Dec : displays the message and then increments/decrement the messages tabulation 
  [Debug]IncTab / [Debug]DecTab : increment/decrement tabulation without dispalying message
*/

//-----------------------------------------------------------
/**
   \class GateMessageManager
   \brief Manages the messages displayed by Gate
   \author thibault.frisson@creatis.insa-lyon.fr
          laurent.guigues@creatis.insa-lyon.fr
	  david.sarrut@creatis.insa-lyon.fr
*/

#ifndef __GateMessageManager_h_INCLUDED__
#define __GateMessageManager_h_INCLUDED__

//-----------------------------------------------------------
// Remark : 
// The do { } while(0) statement in macros is made to "swallow the semicolon" 
// see http://gcc.gnu.org/onlinedocs/cpp/Swallowing-the-Semicolon.html#Swallowing-the-Semicolon

#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include "G4ExceptionHandler.hh"
#include "GateConfiguration.h"

// Uncomment out these symbols to force messages compilation 
#define GATE_COMPILE_MESSAGES
//#define GATE_COMPILE_DEBUG_MESSAGES
#define GATE_COMPILE_WARNING_MESSAGES
#define GATE_COMPILE_ERROR_MESSAGES

// Uncomment next line if you want messages to be preprended 
// by as many spaces as the message level
#define GATE_PREPEND_MESSAGE_WITH_SPACE
#define GATE_PREPEND_MESSAGE_WITH_TAB

///-----------------------------------------------------------
// Uncomment next line if you want messages to be preprended 
// by the code of the type of message 
#define GATE_PREPEND_MESSAGE_WITH_CODE
#define GATE_PREPEND_DEBUGMESSAGE_WITH_CODE

// Uncomment next line if you want messages to be preprended 
// by the current tabulation stored in MessageManager
// Tabulation are increased/decreased by 
// the macros which have 'Dec' or 'Inc' in their names
// Example :
// In a method X:
// X() 
// {
//   GateDebugMessageInc("Physics",3,"Begining of method X\n"); 
//   ..
//   // This message will be tabulated with one more space 
//   GateDebugMessage("Physics",5,"comment...");
//   ..
//   GateDebugMessageDec("Physics",3,"End of method X\n"); 
// }
// Gives :
// Begining of method X
//  comment
// End of method X
//
//-----------------------------------------------------------

//-----------------------------------------------------------
#define GateOnMessageLevel(key,value)			\
  int __GateOnMessageLevelVariable =			\
    GateMessageManager::GetMessageLevel(key);		\
  if ( __GateOnMessageLevelVariable<0)			\
    {							\
      GateWarning("message type '"<<key<<"' unknown");	\
    }							\
  else if (value<= __GateOnMessageLevelVariable) 

//-----------------------------------------------------------
#ifdef GATE_PREPEND_MESSAGE_WITH_CODE
#define GateMessageCode(key, value)			\
  "[" << key << "-" << value << "] " 
#else 
#define GateMessageCode(key, value) ""
#endif 

//-----------------------------------------------------------
#ifdef GATE_PREPEND_DEBUGMESSAGE_WITH_CODE
#define GateDebugMessageCode(key, value)			\
  "[Debug-" << key << "-" << value << "] " 
#else 
#define GateDebugMessageCode(key, value) ""
#endif 

//-----------------------------------------------------------
#ifdef GATE_PREPEND_MESSAGE_WITH_TAB
#define GateMessageTab				\
  GateMessageManager::GetTab()
#else 
#define GateMessageTab ""
#endif

//-----------------------------------------------------------
#ifdef GATE_PREPEND_MESSAGE_WITH_SPACE
#define GateMessageSpace(value)			\
  GateMessageManager::GetSpace(value)
#else 
#define GateMessageSpace(value) ""
#endif
 
//-----------------------------------------------------------
#ifdef GATE_COMPILE_MESSAGES

//-----------------------------------------------------------
// Macro for messages
#define GateMessage(key,value,MESSAGE)		\
  do {						\
    GateOnMessageLevel(key,value)		\
      {						\
	std::cout << GateMessageCode(key,value)	\
		  << GateMessageSpace(value)	\
		  << MESSAGE;			\
      }						\
  }						\
  while (0)

//-----------------------------------------------------------
// Macro for continuing a message (when one wants to split the macro
// call into multiple lines)
#define GateMessageCont(key,value,MESSAGE)	\
  do						\
    {						\
      GateOnMessageLevel(key,value)		\
	{					\
	  std::cout << MESSAGE;			\
	}					\
    }						\
  while (0)

#define GateMessageInc(key,value,MESSAGE)	\
  do						\
    {						\
      GateOnMessageLevel(key,value)		\
	{					\
	  std::cout << GateMessageCode(key,value)	\
		    << GateMessageSpace(value)	\
		    << MESSAGE;			\
	  GateMessageManager::IncTab();		\
	}					\
    }						\
  while (0)

#define GateMessageDec(key,value,MESSAGE)	\
  do						\
    {						\
      GateOnMessageLevel(key,value)		\
	{					\
	  GateMessageManager::DecTab();		\
	  std::cout << GateMessageCode(key,value)	\
		    << GateMessageSpace(value)	\
		    << MESSAGE;			\
	}					\
    }						\
  while (0)

#define GateDecTab(key,value)			\
  do						\
    {						\
      GateOnMessageLevel(key,value)		\
	{					\
	  GateMessageManager::DecTab();		\
	}					\
    }						\
  while (0)

#define GateIncTab(key,value)			\
  do						\
    {						\
      GateOnMessageLevel(key,value)		\
	{					\
	  GateMessageManager::IncTab();		\
	}					\
    }						\
  while (0)

#define GateResetTab()				\
  do						\
    {						\
      GateMessageManager::ResetTab();		\
    }						\
  while (0)

#else
#define GateMessage(key,value,MESSAGE)
#define GateMessageInc(key,value,MESSAGE)
#define GateMessageDec(key,value,MESSAGE)
#define GateMessageCont(key,value,MESSAGE)
#define GateDecTab(key,value)
#define GateIncTab(key,value)
#define GateResetTab()
#endif
//-----------------------------------------------------------

//-----------------------------------------------------------
#ifdef GATE_COMPILE_DEBUG_MESSAGES

//-----------------------------------------------------------
// Macro for debug messages
#define GateDebugMessage(key,value,MESSAGE)		\
  do							\
    {							\
      GateOnMessageLevel(key,value)			\
	{						\
	  std::cout << GateDebugMessageCode(key,value)	\
		    << GateMessageSpace(value)		\
		    << MESSAGE;				\
	}						\
    }							\
  while (0)

//-----------------------------------------------------------
// Macro for continuing a debug message (when one wants to split the
// macro call into multiple lines)
#define GateDebugMessageCont(key,value,MESSAGE)	\
  do						\
    {						\
      GateOnMessageLevel(key,value)		\
	{					\
	  std::cout << MESSAGE;			\
	}					\
    }						\
  while (0)

#define GateDebugMessageInc(key,value,MESSAGE)	\
  do						\
    {						\
      GateOnMessageLevel(key,value)		\
	{					\
	  std::cout << GateDebugMessageCode(key,value)	\
		    << GateMessageSpace(value)	\
		    << MESSAGE;			\
	  GateMessageManager::IncTab();		\
	}					\
    }						\
  while (0)

#define GateDebugMessageDec(key,value,MESSAGE)	\
  do						\
    {						\
      GateOnMessageLevel(key,value)		\
	{					\
	  GateMessageManager::DecTab();		\
	  std::cout << GateDebugMessageCode(key,value)	\
		    << GateMessageSpace(value)	\
		    << MESSAGE;			\
	}					\
    }						\
  while (0)

#define GateDebugDecTab(key,value)		\
  do						\
    {						\
      GateOnMessageLevel(key,value)		\
	{					\
	  GateMessageManager::DecTab();		\
	}					\
    }						\
  while (0)

#define GateDebugIncTab(key,value)		\
  do						\
    {						\
      GateOnMessageLevel(key,value)		\
	{					\
	  GateMessageManager::IncTab();		\
	}					\
    }						\
  while (0)
    
#define GateDebugResetTab()			\
  do						\
    {						\
      GateMessageManager::ResetTab();		\
    }						\
  while (0)

#else
#define GateDebugMessage(key,value,MESSAGE) 
#define GateDebugMessageCont(key,value,MESSAGE) 
#define GateDebugMessageInc(key,value,MESSAGE)
#define GateDebugMessageDec(key,value,MESSAGE) 
#define GateDebugDecTab(key,value)
#define GateDebugIncTab(key,value)
#endif
//-----------------------------------------------------------

//-----------------------------------------------------------
#ifdef GATE_COMPILE_WARNING_MESSAGES

//-----------------------------------------------------------
// Macro for warning messages
#define GateWarning(MESSAGE)						\
  do									\
    {									\
      int lev = GateMessageManager::GetMessageLevel("Warning");		\
      if (lev >0)							\
	{								\
	  std::cout << " <!> *** WARNING *** <!>  " << MESSAGE << Gateendl; \
	  if (lev >1)							\
	    {								\
	      std::cout << " <!> *** WARNING *** <!>  In file '"<<__FILE__ \
			<<"' ; Line "<<__LINE__<<Gateendl;		\
	    }								\
	}								\
    }									\
  while (0) 

#else
#define GateWarning(MESSAGE) 
#endif
//-----------------------------------------------------------

// LG : TO DO : DEFINE GATE_GET_CURRENT_OBJECT_NAME correctly 
#define GATE_GET_CURRENT_OBJECT_NAME "Gate"

//-----------------------------------------------------------
#ifdef GATE_COMPILE_ERROR_MESSAGES
// Macro for errors occuring within an object
#define __SHORT_FILE__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define GateError(MESSAGE)			\
  do						\
    {						\
      std::ostringstream s_;			\
      s_ << MESSAGE;				\
      std::ostringstream f_;			\
      f_ << __SHORT_FILE__ << " (l."<<__LINE__<<"): ";\
      G4String str_f = f_.str();		\
      G4String str_s = s_.str();		\
      std::cout << str_f << str_s << Gateendl; \
      exit(-1);					\
      G4Exception(str_f,str_s,			\
		  FatalException," ");		\
    }						\
  while (0) 

//-----------------------------------------------------------
// Macro for errors occuring at global scope (or in static methods)
#define GateGlobalError(MESSAGE)		\
  do						\
    {						\
      std::ostringstream s;			\
      s << MESSAGE;				\
      std::ostringstream f;			\
      f << __SHORT_FILE__ << " (l."<<__LINE__<<")";	\
      G4String str_f = f.str();			\
      G4String str_s = s.str();			\
      G4Exception(str_f,str_s,			\
		  FatalException," ");		\
    }						\
  while (0) 

#else
#define GateError(MESSAGE)
#endif
//-----------------------------------------------------------

//-----------------------------------------------------------
#define Gateendl std::endl
//-----------------------------------------------------------

//-----------------------------------------------------------
#define DD(a) GateMessage("Core", 0 , #a " = [ " << a << " ]" << Gateendl);
//-----------------------------------------------------------

//-----------------------------------------------------------
#include "GateMessageMessenger.hh"
#include "G4UIsession.hh"

//-----------------------------------------------------------
class GateMessageManager : public G4UIsession
{
public:
  GateMessageManager();
  ~GateMessageManager(){}

  static GateMessageManager* GetInstance();
  static void RegisterMessageType(std::string key, 
				  std::string help,
				  unsigned char default_level = 9);
  static void SetMessageLevel(std::string key, unsigned char level);
  static int GetMessageLevel(std::string key);
  static std::string& GetTab() { static std::string s; return s; }
  static std::string GetSpace(int n);
  static void IncTab() { GetTab() += std::string("   "); }
  static void DecTab() { GetTab() = GetTab().substr(0,GetTab().length()-3); }
  static void ResetTab() { GetTab() = std::string(""); }
  static void PrintInfo();
    
  // the two follwing are overrided from G4UIsession to intercept
  // all G4cout. In the main : 
  // G4UImanager::GetUIpointer()->SetCoutDestination(new GateMessageManager);
  #ifdef Geant496_COMPATIBILITY
    virtual G4int ReceiveG4cout(const G4String& s);
    virtual G4int ReceiveG4cerr(const G4String& s);
  #else
    virtual G4int ReceiveG4cout(G4String s);
    virtual G4int ReceiveG4cerr(G4String s);
  #endif  

  void EnableG4Messages(bool b);
      
protected:
  GateMessageMessenger *pMessenger;
  std::map<std::string,int> mMessageLevel;
  std::map<std::string,std::string> mMessageHelp;  
  unsigned int mMaxMessageLength;
  int mAllLevel;
  int mEnableG4Message;
};
//-----------------------------------------------------------

//#include "GateException.hh"

#endif
