/*
  The RealityGrid Steering Library

  Copyright (c) 2002-2010, University of Manchester, United Kingdom.
  All rights reserved.

  This software is produced by Research Computing Services, University
  of Manchester as part of the RealityGrid project and associated
  follow on projects, funded by the EPSRC under grants GR/R67699/01,
  GR/R67699/02, GR/T27488/01, EP/C536452/1, EP/D500028/1,
  EP/F00561X/1.

  LICENCE TERMS

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

    * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.

    * Neither the name of The University of Manchester nor the names
      of its contributors may be used to endorse or promote products
      derived from this software without specific prior written
      permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
  COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.

  Author: Andrew Porter
          Robert Haines
 */

/** @internal
    @file ReG_Steer_XML.h
    @brief Header file defining methods and structures for handling XML
    @author Andrew Porter
    @author Robert Haines
    This header file contains defines routines and data structures for
    parsing the XML steering-communication messages
*/

#ifndef __REG_STEER_XML_H__
#define __REG_STEER_XML_H__

#include "ReG_Steer_Config.h"
#include "ReG_Steer_Browser.h"

#ifdef __cplusplus
  #define PREFIX "C"
#else
  #define PREFIX
#endif

#define REG_SUCCESS 0
#define REG_FAILURE 1

/*-----------------------------------------------------------------*/

/** @internal
    Stucture for holding parsed elements of a parameter or param_def */
struct param_struct{

  /** The handle of the parameter */
  xmlChar             *handle;
  /** The label of the parameter */
  xmlChar             *label;
  /** The value of the parameter */
  xmlChar             *value;
  /** The type of the parameter (coded according to ReG_Steer_types.h) */
  xmlChar             *type;
  /** Whether or not the parameter is steerable */
  xmlChar             *steerable;
  /** Whether or not the parameter has been generated by the steering lib*/
  xmlChar             *is_internal;
  /** Minimum permitted value (if any) */
  xmlChar             *min_val;
  /** Maximum permitted value (if any).  If parameter is a string then
      this holds its maximum length. */
  xmlChar             *max_val;
  /** Pointer to next parameter in list (if any) */
  struct param_struct *next;
};

/** @internal Structure for holding parsed elements of a command */
struct cmd_struct{
  /** The identifier of the command (if any) */
  xmlChar             *id;
  /** The name of the command (if any) */
  xmlChar             *name;
  /** Pointer to first parameter associated with this command */
  struct param_struct *first_param;
  /** Linked list of parameters associated with this command */
  struct param_struct *param;
  /* Pointer to next command in list (if any) */
  struct cmd_struct   *next;
};

/** @internal Structure for holding parsed elements of a status msg */
struct status_struct{
  /** Pointer to first parameter (if any) */
  struct param_struct *first_param;
  /** Linked list of parameters in the message */
  struct param_struct *param;
  /** Pointer to first command (if any) */
  struct cmd_struct   *first_cmd;
  /** Linked list of commands in the message */
  struct cmd_struct   *cmd;
};

/** @internal Structure for holding parsed elements of IO- or Chk-Type */
struct io_struct{
  /** Label of the IO- or ChkType (as specified at registration) */
  xmlChar  	   *label;
  /** Handle of the IO- or ChkType (as generated by lib) */
  xmlChar  	   *handle;
  /** Whether REG_IN, REG_OUT or REG_INOUT (ChkType only) */
  xmlChar  	   *direction;
  /** Handle of the steerable parameter controlling the interval
      between IO/Chk activity for this Type */
  xmlChar  	   *freq_handle;
  /** Pointer to next IO/Chk-Type in list (if any) */
  struct io_struct *next;
};

/** @internal Structure for holding parsed elements of a control msg
    @bug  is almost the same as status_struct so could re-use?? */
struct control_struct{
  /** The simulated time after which this message becomes valid
      (if any) */
  xmlChar             *valid_after;
  /**  Pointer to first parameter (if any) */
  struct param_struct *first_param;
  /** Linked list of parameters in the message */
  struct param_struct *param;
  /** Pointer to first command (if any) */
  struct cmd_struct   *first_cmd;
  /** Linked list of commands in the message */
  struct cmd_struct   *cmd;
};

/** @internal Structure for holding supported commands */
struct supp_cmd_struct{
  /** Pointer to first command in list */
  struct cmd_struct   *first_cmd;
  /** Linked list of supported commands */
  struct cmd_struct   *cmd;
};

/** @internal Structure for holding IOType defs */
struct io_def_struct{
  /** Pointer to first IOType in list */
  struct io_struct *first_io;
  /** Linked list of IOTypes */
  struct io_struct *io;
};

/** @internal Strucutre for holding a single checkpoint log entry */
struct chk_log_entry_struct{
  /** Handle of the ChkType */
  xmlChar                 *chk_handle;
  /** Tag acting as UID for this checkpoint */
  xmlChar                 *chk_tag;
  /** Pointer to first param stored with this checkpoint */
  struct param_struct     *first_param;
  /** Linked list of parameters stored with this checkpoint */
  struct param_struct     *param;
  /** Pointer to next entry in the log */
  struct chk_log_entry_struct *next;
};

/** @internal Structure for single log entry */
struct log_entry_struct{
  /** The key (UID) of this entry in the log */
  xmlChar                 *key;
  /** Pointer to first entry in the parameter log */
  struct param_struct     *first_param_log;
  /** Linked list of parameter entries */
  struct param_struct     *param_log;
  /** Pointer to first entry in the checkpoint log */
  struct chk_log_entry_struct *first_chk_log;
  /** Linked list of checkpoint log entries */
  struct chk_log_entry_struct *chk_log;
  /** Pointer to next entry in the log */
  struct log_entry_struct *next;
};

/** @internal Structure for holding whole log */
struct log_struct{
  /** Pointer to first entry in log */
  struct log_entry_struct *first_entry;
  /** Linked list of log entries */
  struct log_entry_struct *entry;
};

/** @internal Structure for holding a steering message */
struct msg_struct{
  /** Type of the message (encoded according to ReG_Steer_types.h) */
  int   	           msg_type;
  /** The UID of the message */
  xmlChar                 *msg_uid;
  /** Pointer to details of status message */
  struct status_struct    *status;
  /** Pointer to details of control message */
  struct control_struct   *control;
  /** Pointer to supported commands message */
  struct supp_cmd_struct  *supp_cmd;
  /** Pointer to IO definitions message */
  struct io_def_struct    *io_def;
  /** Pointer to Chk definitions message */
  struct io_def_struct    *chk_def;
  /** Pointer to details of log message */
  struct log_struct       *log;
};

/** @internal
    Structure for storing multiple steering messages generated
    by parsing @e e.g. a ResourceProperties document */
struct msg_store_struct {
  /** @internal Pointer to details of a single message */
  struct msg_struct *msg;
  /** @internal Pointer to next entry in message store list (if any) */
  struct msg_store_struct *next;
};

/** @internal
    Structure for storing the UIDs of previous
    messages so we don't act upon them again */
struct msg_uid_history_struct {
  /** Array holding previous UIDs */
  unsigned int  uidStore[REG_UID_HISTORY_BUFFER_SIZE];
  /** Pointer to next free entry in @p uidStore */
  unsigned int *uidStorePtr;
  /** The max possible value of @p uidStorePtr */
  unsigned int *maxPtr;
};


/** @internal
    Definition of entry in main table holding data for connected simulations.
    Contains five sub-tables - the first holding the commands that the
    simulation supports, the second its registered parameters (both
    steerable and monitored), the third its registered IO types, the fourth
    its registered Chk types and the fifth a log of checkpoints taken. */
typedef struct {
  /** The handle assigned to the connected simulation */
  int                  handle;
  /** For connection to applications using local file system - contains
      the location of the directory used to communicate with the sim. */
  char                 file_root[REG_MAX_STRING_LENGTH];
  /** Set to REG_TRUE once detach has been called - prevents us
      calling detach more than once on the SWS */
  int                  detached;
  /** Last status message received from this simulation - filled in
      Get_next_message() and used by whichever Consume_... routine
      is called in response to the message type */
  struct msg_struct   *msg;
  /** Structure for holding multiple messages obtained by parsing
      SWS' ResourceProperties document */
  struct msg_store_struct  Msg_store;
  /** Pointer to the end of the store of multiple messages */
  struct msg_store_struct *Msg_store_tail;
  /** Structure for holding the uid's of messages that we have
      previously consumed */
  struct msg_uid_history_struct Msg_uid_store;
  /** Table of registered commands for this sim */
  Supp_cmd_table_type  Cmds_table;
  /** Table of registered params for this sim */
  Param_table_type     Params_table;
  /** Table of registered IOTypes for this sim */
  IOdef_table_type     IOdef_table;
  /** Table of registered ChkTypes for this sim */
  IOdef_table_type     Chkdef_table;
  /** Table for logging checkpoint activity */
  Chk_log_type         Chk_log;

} Sim_entry_type;

/*-----------------------------------------------------------------*/

/** @internal
    @param filename Name of the file to read and parse
    @param msg Pointer to message struct to hold results
    @param sim Pointer to Sim_entry struct or NULL

    Parse the xml in the specified file.
    If @p sim is NULL then any message UIDs are stored in
    the global Msg_uid_store.  If @p sim is not NULL (@e i.e. this
    routine has been called by a steering client) then they are
    stored in a table within that simulation's table entry.
*/
int Parse_xml_file(char*              filename,
		   struct msg_struct *msg,
		   Sim_entry_type    *sim);

/** @internal
    Parse the xml in the supplied buffer
    @param buf Pointer to buffer to parse
    @param size Size *bytes) of the buffer to parse
    @param msg  Pointer to message struct to hold results
    @param sim Pointer to Sim_entry struct or NULL (if not called by
    a steering client)
    @see Parse_xml_file() */
extern PREFIX int Parse_xml_buf(char*              buf,
				int                size,
				struct msg_struct *msg,
				Sim_entry_type    *sim);

/** @internal
    Parse the DOM document and put the results in msg_struct
    @param doc The DOM document to parse
    @param msg Pointer to message struct to hold results
    @param sim Pointer to Sim_entry struct or NULL (if not called by
    a steering client) */
int Parse_xml(xmlDocPtr          doc,
	      struct msg_struct *msg,
	      Sim_entry_type    *sim);

/** @internal
    Parse a steering message (ReG_steer_message)
    @param doc The DOM document to parse
    @param ns The namespace to use
    @param cur Current XML node
    @param msg Struct to fill with message details
    @param sim Pointer to Sim_entry struct or NULL (if not called by
    a steering client) */
int parseSteerMessage(xmlDocPtr          doc,
		      xmlNsPtr           ns,
		      xmlNodePtr         cur,
		      struct msg_struct *msg,
		      Sim_entry_type    *sim);

/** @internal
    Parse a Resource Properties document from a WSRF service
    @param doc The DOM document to parse
    @param ns The namespace to use
    @param cur Current XML node
    @param sim Pointer to Sim_entry struct or NULL (if not called by
    a steering client) */
int parseResourceProperties(xmlDocPtr       doc,
			    xmlNsPtr        ns,
			    xmlNodePtr      cur,
			    Sim_entry_type *sim);

/** @internal
    Parse a Status message
    @param doc The DOM document to parse
    @param ns The namespace to use
    @param cur Current XML node
    @param status Pointer to struct to fill with msg details */
int parseStatus(xmlDocPtr             doc,
		xmlNsPtr              ns,
		xmlNodePtr            cur,
	        struct status_struct *status);

/** @internal
    Parse a Control message.
    @param doc The DOM document to parse
    @param ns The namespace to use
    @param cur Current XML node
    @param ctrl Pointer to struct to fill with msg details */
int parseControl(xmlDocPtr              doc,
		 xmlNsPtr               ns,
		 xmlNodePtr             cur,
	         struct control_struct *ctrl);

/** @internal
    Parse a Supported Commands message
    @param doc The DOM document to parse
    @param ns The namespace to use
    @param cur Current XML node
    @param supp_cmd Pointer to struct to fill with msg details */
int parseSuppCmd(xmlDocPtr               doc,
		 xmlNsPtr                ns,
		 xmlNodePtr              cur,
		 struct supp_cmd_struct *supp_cmd);

/** @internal
    Parse a Parameter element
    @param doc The DOM document to parse
    @param ns The namespace to use
    @param cur Current XML node
    @param param Pointer to struct to fill with parameter details */
int parseParam(xmlDocPtr            doc,
	       xmlNsPtr             ns,
	       xmlNodePtr           cur,
	       struct param_struct *param);

/** @internal
    Parse a Command element
    @param doc The DOM document to parse
    @param ns The namespace to use
    @param cur Current XML node
    @param cmd Pointer to struct to fill with command details */
int parseCmd(xmlDocPtr          doc,
	     xmlNsPtr           ns,
	     xmlNodePtr         cur,
	     struct cmd_struct *cmd);

/** @internal
    Parse a Checkpoint Type definition element
    @param doc The DOM document to parse
    @param ns The namespace to use
    @param cur Current XML node
    @param chk_def Pointer to struct to fill with ChkType definition */
int parseChkTypeDef(xmlDocPtr             doc,
		    xmlNsPtr              ns,
		    xmlNodePtr            cur,
		    struct io_def_struct *chk_def);

/** @internal
    Parse an IOType definition element
    @param doc The DOM document to parse
    @param ns The namespace to use
    @param cur Current XML node
    @param io_def Pointer to struct to fill with IOType definition */
extern PREFIX int parseIOTypeDef(xmlDocPtr             doc,
				 xmlNsPtr              ns,
				 xmlNodePtr            cur,
				 struct io_def_struct *io_def);

/** @internal
    Parse an IOType/ChkType element
    @param doc The DOM document to parse
    @param ns The namespace to use
    @param cur Current XML node
    @param io Pointer to struct to fill with IOType details */
int parseIOType(xmlDocPtr         doc,
		xmlNsPtr          ns,
		xmlNodePtr        cur,
		struct io_struct *io);

/** @internal
    Parse a Logging message
    @param doc The DOM document to parse
    @param ns The namespace to use
    @param cur Current XML node
    @param log Pointer to struct to fill with log details */
int parseLog(xmlDocPtr          doc,
	     xmlNsPtr           ns,
	     xmlNodePtr         cur,
	     struct log_struct *log);

/** @internal
    Parse a Logging entry
    @param doc The DOM document to parse
    @param ns The namespace to use
    @param cur Current XML node
    @param log_entry Pointer to struct to fill with details of a log entry */
int parseLogEntry(xmlDocPtr                doc,
		  xmlNsPtr                 ns,
		  xmlNodePtr               cur,
		  struct log_entry_struct *log_entry);

/** @internal
    Parse a Checkpoint Log entry
    @param doc The DOM document to parse
    @param ns The namespace to use
    @param cur Current XML node
    @param log_entry Ptr to struct to fill with details of a chkpoint log entry */
int parseChkLogEntry(xmlDocPtr                    doc,
		     xmlNsPtr                     ns,
		     xmlNodePtr                   cur,
		     struct chk_log_entry_struct *log_entry);

/** @internal
    Create a new msg_store_struct and return pointer to it */
struct msg_store_struct            *New_msg_store_struct();
/** Create a new msg_struct and return pointer to it */
extern PREFIX struct msg_struct    *New_msg_struct();
/** @internal
    Create a new status_struct and return a pointer to it */
struct status_struct               *New_status_struct();
/** @internal
    Create a new control_struct and return a pointer to it */
struct control_struct              *New_control_struct();
/** @internal
    Create a new supp_cmd_struct and return a pointer to it */
struct supp_cmd_struct             *New_supp_cmd_struct();
/** Create a new io_def_struct and return a pointer to it */
extern PREFIX struct io_def_struct *New_io_def_struct();
/** @internal
    Create a new io_struct and return a pointer to it */
struct io_struct                   *New_io_struct();
/** @internal
    Create a new param_struct and return a pointer to it */
struct param_struct                *New_param_struct();
/** @internal
    Create a new cmd_struct and return a pointer to it */
struct cmd_struct                  *New_cmd_struct();
/** @internal
    Create a new chk_log_entry_struct and return a pointer to it */
struct chk_log_entry_struct        *New_chk_log_entry_struct();
/** @internal
    Create a new log_entry_struct and return a pointer to it */
struct log_entry_struct            *New_log_entry_struct();
/** @internal
    Create a new log_struct and return a pointer to it */
struct log_struct                  *New_log_struct();
/** @internal
    Delete a msg_struct and all its constituents
    @param msgIn Pointer to msg_struct to delete */
extern PREFIX void Delete_msg_struct(struct msg_struct **msgIn);
/** @internal
    Delete a status_struct and all its constituents
    @param status Pointer to status_struct to delete */
void               Delete_status_struct(struct status_struct *status);
/** @internal
    Delete a control_struct and all its constituents
    @param ctrl Pointer to control_struct to delete */
void               Delete_control_struct(struct control_struct *ctrl);
/** @internal
    Delete supp_cmd_struct and all its constituents
    @param supp_cmd Pointer to supp_cmd_struct to delete */
void 		   Delete_supp_cmd_struct(struct supp_cmd_struct *supp_cmd);
/** @internal
    Delete a param_struct and all its constituents
    @param param Pointer to param_struct to delete */
void 		   Delete_param_struct(struct param_struct *param);
/** @internal
    Delete a cmd_struct and all its constituents
    @param cmd Pointer to cmd_struct to delete */
void 		   Delete_cmd_struct(struct cmd_struct *cmd);
/** @internal
    Delete an io_def_struct and all its constituents
    @param io_def Pointer to io_def_struct to delete */
void 		   Delete_io_def_struct(struct io_def_struct *io_def);
/** @internal
    Delete an io_struct and all its constituents
    @param io Pointer to io_struct to delete */
void 		   Delete_io_struct(struct io_struct *io);
/** @internal
    Delete a chk_log_entry_struct and all its constituents
    @param log Pointer to chk_log_entry_struct to delete */
void 		   Delete_chk_log_entry_struct(struct chk_log_entry_struct *log);
/** @internal
    Delete a log_entry_struct and all its constituents
    @param log Pointer to log_entry_struct to delete */
void 		   Delete_log_entry_struct(struct log_entry_struct *log);
/** @internal
    Delete a log_struct and all its constituents
    @param log Pointer to log_struct to delete */
void 		   Delete_log_struct(struct log_struct *log);
/** Print out a message to stderr
    @param msg Pointer to msg to print */
extern PREFIX void Print_msg(struct msg_struct *msg);
/** @internal
    Print out the contents of a status_struct to stderr
    @param status Pointer to the status_struct to print */
void               Print_status_struct(struct status_struct *status);
/** @internal
    Print out the contents of a control_struct to stderr
    @param ctrl Pointer to the control_struct to print */
void               Print_control_struct(struct control_struct *ctrl);
/** @internal
    Print out the contents of a param_struct to stderr
    @param param Pointer to the param_struct to print */
void 		   Print_param_struct(struct param_struct *param);
/** @internal
    Print out the contents of a cmd_struct to stderr
    @param cmd Pointer to the cmd_struct to print */
void 		   Print_cmd_struct(struct cmd_struct *cmd);
/** @internal
    Print out the contents of a supp_cmd_struct to stderr
    @param supp_cmd Pointer to the supp_cmd_struct to print */
void 		   Print_supp_cmd_struct(struct supp_cmd_struct *supp_cmd);
/** @internal
    Print out the contents of an io_def_struct to stderr
    @param io_def Pointer to the io_def_struct to print */
void 		   Print_io_def_struct(struct io_def_struct *io_def);
/** @internal
    Print out the contents of an io_struct to stderr
    @param io Pointer to the io_struct to print */
void 		   Print_io_struct(struct io_struct *io);
/** @internal
    Print out the contents of a log_struct to stderr
    @param log Pointer to the log_struct to print */
void 		   Print_log_struct(struct log_struct *log);
/** @internal
    Print out the contents of a log_entry_struct
    @param entry Pointer to the log_entry_struct to print */
void 		   Print_log_entry_struct(struct log_entry_struct *entry);
/** @internal
    Print out the contents of a chk_log_entry_struct to stderr
    @param entry Pointer to the chk_log_entry_struct to print */
void 		   Print_chk_log_entry_struct(struct chk_log_entry_struct *entry);
/** @internal
    Test to see whether string contains XML chars.
    @param string The string to test (NULL terminated)
    @return REG_TRUE if it does and REG_FALSE if it doesn't */
int  String_contains_xml_chars(const char *string);

/** @internal
    Enumeration of the various possible states of our SAX parser
    for the results of an OGSI findServiceData or WSRF
    GetResourceProperty("entry") - corresponds to the elements of
    the doc we're interested in */
enum doc_state {UNKNOWN, STARTING,
		OGSI_ENTRY, MEMBER_SERVICE_LOCATOR,
		GS_HANDLE, CONTENT, SERVICE_TYPE, COMPONENT_CONTENT,
		COMPONENT_START_DATE_TIME,
		COMPONENT_CREATOR_NAME, COMPONENT_CREATOR_GROUP,
		COMPONENT_SOFTWARE_PACKAGE, COMPONENT_TASK_DESCRIPTION,
		/* The next line has those that are WSRF-specifc states */
		WSRF_ENTRY, MEMBER_SERVICE_EPR, SERVICEGROUP_ENTRY_EPR,
		EPR, WSADDRESS, SERVICEGROUP_EPR, SERVICEGROUP_WSADDRESS,
		FINISHING};

/** @internal
    Parse the document returned by doing a findServiceData
    on a serviceGroupRegistration service (OGSI) or a GetResourceProperty
    on a ServiceGroup (WSRF).
    @param buf Buffer to parse
    @param size Length of @p buf in bytes
    @param contents On return, table of details of registry entries
 */
int Parse_registry_entries(char*                     buf,
			   int                       size,
			   struct registry_contents *contents);

/** @internal
    Parse the supplied resource property document and pull out the
    value of the specified resource property
    @param pRPDoc Buffer containing the ResourceProperty document
    @param size Size of the ResourceProperty doc (bytes)
    @param name Name of the ResourceProperty to extract
    @param resultBuf Buffer to put extracted RP into
    @param resultBufLen Max. length of results buffer (bytes)
    @return REG_SUCCESS, REG_FAILURE */
int Extract_resource_property(char *pRPDoc,
			      int   size,
			      char *name,
			      char *resultBuf,
			      int   resultBufLen);

/** @internal
    Check to see whether or not this message has been seen within the
    last REG_UID_HISTORY_BUFFER_SIZE messages received
    @param msg_uid The UID to check for
    @param hist Pointer to the UID history table to search
*/
int Msg_already_received(char                          *msg_uid,
			 struct msg_uid_history_struct *hist);

/** @internal
    Delete the store of messages
    @param msgStore Pointer to the store to delete
*/
int Delete_msg_store(struct msg_store_struct *msgStore);

/** @internal
    Delete the store of message UIDs
    @param uidHist Pointer to the UID store to delete */
int Delete_msg_uid_store(struct msg_uid_history_struct *uidHist);

/** @internal
    Store the value of the specified node in the string pointed
    to by @p dest, which is a member of the specified @p entry struct
    @param doc The XML document being parsed
    @param cur Pointer to the current node in the DOM tree
    @param dest Pointer to the char* to put string into
    @param entry Pointer to the entry struct that @p dest belongs to */
int Store_xml_string(xmlDocPtr doc,
		     xmlNodePtr cur,
		     char **dest,
		     struct registry_entry *entry);

#endif
