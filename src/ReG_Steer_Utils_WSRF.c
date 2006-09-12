/*----------------------------------------------------------------------------
  This file contains utility routines and data structures related to
  WSRF(SOAP)-based steering.

  (C) Copyright 2006, University of Manchester, United Kingdom,
  all rights reserved.

  This software was developed by the RealityGrid project
  (http://www.realitygrid.org), funded by the EPSRC under grants
  GR/R67699/01 and GR/R67699/02.

  LICENCE TERMS

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.

  THIS MATERIAL IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. THE ENTIRE RISK AS TO THE QUALITY
  AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE PROGRAM PROVE
  DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR
  CORRECTION.
---------------------------------------------------------------------------*/

/** @file ReG_Steer_Utils_WSRF.c
    @brief Source file for routines to do registry look-up
    @author Andrew Porter
  */

#include "ReG_Steer_types.h"
#include "ReG_Steer_Browser.h"
#include "ReG_Steer_Utils.h"
#include "ReG_Steer_Steerside_WSRF.h"
#include "ReG_Steer_Common.h"
#include "ReG_Steer_XML.h"
#include "soapH.h"

/** Global scratch buffer - declared in ReG_Steer_Appside.c */
extern char Global_scratch_buffer[];

/*-------------------------------------------------------------------------*/

int Get_registry_entries_wsrf(const char *registryEPR, 
			      const struct reg_security_info *sec,
			      struct registry_contents *contents){

  struct wsrp__GetMultipleResourcePropertiesRequest in;
  struct soap soap;
  int    status = REG_SUCCESS;
  char  *out;
  int    i;
#ifdef USE_REG_TIMING
  double time0, time1;
#endif
  contents->numEntries = 0;
  contents->entries = NULL;

  soap_init(&soap);

  /* regServiceGroup can use SSL for authentication */
  /* If address of SWS begins with 'https' then initialize SSL context */
  if( (sec->use_ssl == REG_TRUE) && 
      (strstr(registryEPR, "https") == registryEPR) ){
    if( REG_Init_ssl_context(&soap,
			     REG_TRUE, /* Authenticate SWS */
			     sec->myKeyCertFile,
			     sec->passphrase,
			     sec->caCertsPath) == REG_FAILURE){

      fprintf(stderr, "STEERUtils: Get_registry_entries_wsrf: call to initialize "
	      "soap SSL context failed\n");
      return REG_FAILURE;
    }
    status = Create_WSRF_header(&soap, registryEPR,
				NULL, NULL);
  }
  else{
    /* Otherwise we just use WSSE */
    status = Create_WSRF_header(&soap, registryEPR,
				sec->userDN, sec->passphrase);
  }
  if(status != REG_SUCCESS){
    return REG_FAILURE;
  }

  in.__size = 1;
  in.__ptr = (struct wsrp__ResourcePropertyStruct *)malloc(in.__size*
							  sizeof(struct wsrp__ResourcePropertyStruct));
  if(!in.__ptr){
    fprintf(stderr, "STEERUtils: ERROR: Get_registry_entries_wsrf: "
	    "malloc failed\n");
    return REG_MEM_FAIL;
  }

  for(i=0; i<in.__size;i++){
    in.__ptr[i].ResourceProperty = (char *)malloc(16*sizeof(char));
    if(!(in.__ptr[i].ResourceProperty))return REG_MEM_FAIL;
  }
  /* Entries in a ServiceGroup are held in the 'Entry' ResourceProperty */
  sprintf(in.__ptr[0].ResourceProperty, "Entry");

#ifdef USE_REG_TIMING
  Get_current_time_seconds(&time0);
#endif

  if(soap_call_wsrp__GetMultipleResourceProperties(&soap, 
						   registryEPR, 
						   "", in,
						   &out) != SOAP_OK){
    soap_print_fault(&soap, stderr);
    free(in.__ptr[0].ResourceProperty);
    soap_end(&soap);
    soap_done(&soap);
    return REG_FAILURE;
  }

  free(in.__ptr[0].ResourceProperty);
  free(in.__ptr);

#ifdef USE_REG_TIMING
  Get_current_time_seconds(&time1);
  fprintf(stderr, "STEERUtils: TIMING: soap_call_wsrp__GetMultipleResourceProperties "
	  "took %f seconds\n", (time1-time0));
#endif

#if REG_DEBUG_FULL
  fprintf(stderr, "STEERUtils: Get_registry_entries_wsrf: "
	  "Get_resource_property for Entry returned >>%s<<\n\n", out);
#endif
  if(strlen(out) > 0){
    status = Parse_registry_entries(out, 
				    strlen(out),
				    contents);
  }

  soap_end(&soap);
  soap_done(&soap);

  return status;
}

/*-----------------------------------------------------------------*/

char *Create_SWS(const struct reg_job_details   *job,
		 const char                     *containerAddress,
		 const char                     *registryAddress,
		 const struct reg_security_info *sec){

  struct swsf__createSWSResourceResponse     response;
  struct wsrp__SetResourcePropertiesResponse setRPresponse;
  struct rsg__AddResponse                    addResponse;
  char                                       factoryAddr[256];
  char                                       jobDescription[1024];
  static char                                epr[256];
  struct soap                                soap;
  char                                      *contents;
  char                                      *pchar;
  int                                        numBytes;
  int                                        ssl_initialized = 0;
  /* NOTE that the job struct contains information about the service
     to be created (including username and password) while the sec
     struct contains information to allow us to authenticate to the
     registry */
#if REG_DEBUG_FULL
  fprintf(stderr,"\nSTEERUtils: Create_SWS args:\n");
  fprintf(stderr," - lifetimeMinutes: %d\n", job->lifetimeMinutes);
  fprintf(stderr," - containerAddress: %s\n", containerAddress);
  fprintf(stderr," - registryAddress: %s\n", registryAddress);
  fprintf(stderr," - userName: %s\n", job->userName);
  fprintf(stderr," - group: %s\n", job->group);
  fprintf(stderr," - software: %s\n", job->software);
  fprintf(stderr," - purpose: %s\n", job->purpose);
  fprintf(stderr," - inputFilename: %s\n", job->inputFilename);
  fprintf(stderr," - checkpointAddress: %s\n", job->checkpointAddress);
#endif /* REG_DEBUG_FULL */

  sprintf(factoryAddr, "%sSession/SWSFactory/SWSFactory", 
	  containerAddress);

#if REG_DEBUG_FULL
  fprintf(stderr, "\nSTEERUtils: Create_SWS: using factory >>%s<<\n",
	  factoryAddr);
#endif

  soap_init(&soap);
  /* Something to do with the XML type */
  soap.encodingStyle = NULL;

  if(strstr(factoryAddr, "https") == factoryAddr){

    if( REG_Init_ssl_context(&soap,
			     REG_TRUE, /* Authenticate container */
			     sec->myKeyCertFile,
			     sec->passphrase,
			     sec->caCertsPath) == REG_FAILURE){

      fprintf(stderr, "STEERUtils: ERROR: call to initialize soap SSL context failed\n");
      return NULL;
    }
    ssl_initialized = 1;
  }

#if REG_DEBUG_FULL
  if(sec->passphrase[0]){
    printf("STEERUtils: Create_SWS: userName for call to createSWSResource >>%s<<\n", 
	   sec->userDN);
  }
#endif /* REG_DEBUG_FULL */

  if(Create_WSRF_header(&soap, factoryAddr, NULL, NULL) != REG_SUCCESS){
    return NULL;
  }

  /* 1440 = 24hrs in minutes.  Is the default lifetime of the service
     until its associated job starts up and then the TerminationTime
     is reset using the maxRunTime RP */
  if(soap_call_swsf__createSWSResource(&soap, factoryAddr, NULL, 
				       1440, 
				       (char *)job->checkpointAddress, 
				       (char *)job->passphrase,
				       &response) != SOAP_OK){
    if(soap.fault && soap.fault->detail){

      fprintf(stderr, "STEERUtils: Call to createSWSResource failed: Soap error "
	      "detail any = %s\n", soap.fault->detail->__any);
    }
    soap_print_fault(&soap, stderr);
    return NULL;
  }

  strncpy(epr, response.wsa__EndpointReference.wsa__Address, 256);

  /* Register this SWS */

  snprintf(jobDescription, 1024, 
	   "<MemberEPR><wsa:Address>%s</wsa:Address></MemberEPR>"
	   "<Content><registryEntry>\n"
	   "<serviceType>SWS</serviceType>\n"
	   "<componentContent>\n"
	   "<componentStartDateTime>%s</componentStartDateTime>\n"
	   "<componentCreatorName>%s</componentCreatorName>\n"
	   "<componentCreatorGroup>%s</componentCreatorGroup>\n"
	   "<componentSoftwarePackage>%s</componentSoftwarePackage>\n"
	   "<componentTaskDescription>%s</componentTaskDescription>\n"
	   "</componentContent>"
	   "<regSecurity>"
	   "<passphrase>%s</passphrase>"
	   "<allowedUsers>"
	   "<user>%s</user>"
	   "</allowedUsers>"
	   "</regSecurity>"
	   "</registryEntry>"
	   "</Content>",
	   epr, Get_current_time_string(), job->userName, job->group, 
	   job->software, job->purpose, job->passphrase, job->userName);

  if(!ssl_initialized && 
     (strstr(registryAddress, "https") == registryAddress) ){

    if( REG_Init_ssl_context(&soap,
			     REG_TRUE, /* Authenticate container */
			     sec->myKeyCertFile,
			     sec->passphrase,
			     sec->caCertsPath) == REG_FAILURE){

      fprintf(stderr, "STEERUtils: ERROR: Create_SWS: call to initialize soap SSL"
	      " context for call to regServiceGroup::Add failed\n");
      Destroy_WSRP(epr, sec);
      return NULL;
    }
    ssl_initialized = 1;
  }

  if(Create_WSRF_header(&soap, registryAddress, 
			sec->userDN, sec->passphrase) != REG_SUCCESS){
    return NULL;
  }

  if(soap_call_rsg__Add(&soap, registryAddress, 
			"", jobDescription,
			&addResponse) != SOAP_OK){
    fprintf(stderr, "STEERUtils: ERROR: Create_SWS: call to Add service to "
	    "registry failed:\n");
    soap_print_fault(&soap, stderr);
    Destroy_WSRP(epr, sec);
    return NULL;
  }

  /* Print out address of the ServiceGroupEntry
     that represents our SWS's entry in the registry */
#if REG_DEBUG_FULL
  fprintf(stderr, "STEERUtils: Create_SWS: Address of SGE >>%s<<\n", 
	  addResponse.wsa__EndpointReference.wsa__Address);
#endif /* REG_DEBUG_FULL */

  /* Finally, set it up with information on its
     maximum run-time, address of registry and address of SGE */

  snprintf(jobDescription, 1024, "<maxRunTime>%d</maxRunTime>"
	   "<registryEPR>%s</registryEPR>"
	   "<ServiceGroupEntry>%s</ServiceGroupEntry>",
	   job->lifetimeMinutes, registryAddress, 
	   addResponse.wsa__EndpointReference.wsa__Address);

  if(Create_WSRF_header(&soap, epr, job->userName, job->passphrase) 
     != REG_SUCCESS){
    return NULL;
  }

#if REG_DEBUG_FULL
  fprintf(stderr,
	  "\nSTEERUtils: Create_SWS: Calling SetResourceProperties "
	  "with >>%s<<\n", jobDescription);
#endif
  if(soap_call_wsrp__SetResourceProperties(&soap, epr, "", jobDescription,
					   &setRPresponse) != SOAP_OK){
    soap_print_fault(&soap, stderr);
    return NULL;
  }

  /* If an input deck has been specified, grab it and store on the
     steering service */
  if(strlen(job->inputFilename) &&
     (Read_file(job->inputFilename, &contents, &numBytes, REG_TRUE) 
      == REG_SUCCESS) ){
    /* 49 = 12 + 37 = strlen("<![CDATA[]]>") + 
                             2*strlen("<inputFileContent>") + 1 */
    if((strlen(contents) + 49)< REG_SCRATCH_BUFFER_SIZE){
      pchar = Global_scratch_buffer;
      /* Protect the file content from any XML parsing by wrapping
	 in CDATA tags */
      pchar += sprintf(pchar, "<inputFileContent><![CDATA[");
      pchar += sprintf(pchar, "%s", contents);
      sprintf(pchar, "]]></inputFileContent>");
    }
    else{
      
    }

    if(Create_WSRF_header(&soap, epr, job->userName, 
			  job->passphrase) != REG_SUCCESS){
      soap_end(&soap);
      soap_done(&soap);
      return NULL;
    }
#if REG_DEBUG_FULL
    fprintf(stderr,
	    "\nSTEERUtils: Create_SWS: Calling SetResourceProperties with >>%s<<\n",
	    Global_scratch_buffer);
#endif
    if(soap_call_wsrp__SetResourceProperties(&soap, epr, "", 
					     Global_scratch_buffer,
					     &setRPresponse) != SOAP_OK){
      soap_print_fault(&soap, stderr);
      return NULL;
    }
  }

  soap_end(&soap); /* dealloc deserialized data */
  soap_done(&soap); /* cleanup and detach soap struct */

  return epr;
}

/*-----------------------------------------------------------------*/

int Destroy_WSRP(const char                     *epr, 
		 const struct reg_security_info *sec)
{
  struct wsrp__DestroyResponse out;
  struct soap soap;
  int    return_status = REG_SUCCESS;
  if(epr){
    soap_init(&soap);
    /* Something to do with the XML type */
    soap.encodingStyle = NULL;

    if(sec->use_ssl != REG_TRUE){
      return_status = Create_WSRF_header(&soap, epr, sec->userDN, 
					 sec->passphrase);
    }
    else{
      return_status = Create_WSRF_header(&soap, epr, NULL, NULL);
    }
    if(return_status != REG_SUCCESS)return REG_FAILURE;

    /* If we're using https then set up the context */
    if((sec->use_ssl == 1) && (strstr(epr, "https") == epr) ){
      if( REG_Init_ssl_context(&soap,
			       REG_TRUE, /* Authenticate SWS */
			       NULL,/*char *certKeyPemFile,*/
			       NULL, /* char *passphrase,*/
			       sec->caCertsPath) == REG_FAILURE){

	fprintf(stderr, "STEERUtils: ERROR: Destroy_WSRP: call to initialize soap "
		"SSL context failed\n");
	return REG_FAILURE;
      }
    }

    if(soap_call_wsrp__Destroy(&soap, epr, NULL, NULL, &out) != SOAP_OK){
      fprintf(stderr, "STEERUtils: Destroy_WSRP: call to Destroy on %s failed:\n   ",
	      epr);
      soap_print_fault(&soap, stderr);
      return_status = REG_FAILURE;
    }

    soap_end(&soap); /* dealloc deserialized data */
    soap_done(&soap); /* cleanup and detach soap struct */
  }
  return return_status;
}

/*-----------------------------------------------------------------*/

int Get_IOTypes_WSRF(const char                     *address,
		     const struct reg_security_info *sec,
		     struct reg_iotype_list         *list){

  struct soap        mySoap;
  struct msg_struct *msg;
  struct io_struct  *ioPtr;
  xmlDocPtr          doc;
  xmlNsPtr           ns;
  xmlNodePtr         cur;
  char              *ioTypes;
  int                i;

  if(!list){
    fprintf(stderr, "STEERUtils: ERROR: Get_IOTypes: pointer to iotype_list is NULL\n");
    return REG_FAILURE;
  }

  list->numEntries = 0;

  soap_init(&mySoap);
  if( Get_resource_property (&mySoap,
			     address,
			     sec->userDN,
			     sec->passphrase,
			     "ioTypeDefinitions",
			     &ioTypes) != REG_SUCCESS ){

    fprintf(stderr, "STEERUtils: ERROR: Get_IOTypes: Call to get ioTypeDefinitions "
	    "ResourceProperty on %s failed\n", address);
    soap_print_fault(&mySoap, stderr);
    soap_end(&mySoap);
    soap_done(&mySoap);
    return REG_FAILURE;
  }

  if( !(doc = xmlParseMemory(ioTypes, strlen(ioTypes))) ||
      !(cur = xmlDocGetRootElement(doc)) ){
    fprintf(stderr, "STEERUtils: ERROR: Get_IOTypes: Hit error parsing buffer\n");
    xmlFreeDoc(doc);
    xmlCleanupParser();
    soap_end(&mySoap);
    soap_done(&mySoap);
    return REG_FAILURE;
  }

  ns = xmlSearchNsByHref(doc, cur,
            (const xmlChar *) "http://www.realitygrid.org/xml/steering");

  if ( xmlStrcmp(cur->name, (const xmlChar *) "ioTypeDefinitions") ){
    fprintf(stderr, "STEERUtils: ERROR: Get_IOTypes: ioTypeDefinitions not the root element\n");
    xmlFreeDoc(doc);
    xmlCleanupParser();
    soap_end(&mySoap);
    soap_done(&mySoap);
    return REG_FAILURE;
  }
  /* Step down to ReG_steer_message and then to IOType_defs */
  cur = cur->xmlChildrenNode->xmlChildrenNode;

  msg = New_msg_struct();
  msg->io_def = New_io_def_struct();
  parseIOTypeDef(doc, ns, cur, msg->io_def);

  if(!(ioPtr = msg->io_def->first_io) ){
    fprintf(stderr, "STEERUtils: ERROR: Get_IOTypes: Got no IOType definitions from %s\n",
	    address);
    xmlFreeDoc(doc);   xmlCleanupParser();
    soap_end(&mySoap); soap_done(&mySoap);
    return REG_FAILURE;
  }

  i = 0;
  fprintf(stdout, "STEERUtils: Available IOTypes:\n");
  while(ioPtr){
    fprintf(stdout, "    %d: %s\n", i++, (char *)ioPtr->label);
    fprintf(stdout, " Dir'n: %s\n", (char *)ioPtr->direction);
    ioPtr = ioPtr->next;
  }
  list->iotype = (struct iotype_detail*)malloc(i * sizeof(struct iotype_detail));
  if(!list->iotype){
    fprintf(stderr, "STEERUtils: ERROR: Get_IOTypes: malloc failed\n");
    xmlFreeDoc(doc);   xmlCleanupParser();
    soap_end(&mySoap); soap_done(&mySoap);
    return REG_FAILURE;
  }
  list->numEntries = i;

  ioPtr = msg->io_def->first_io;
  i = 0;
  while(ioPtr){
    strncpy(list->iotype[i].label, (char *)ioPtr->label, 
	    REG_MAX_STRING_LENGTH);
    if( !xmlStrcmp(ioPtr->direction, (const xmlChar *)"IN") ){
      list->iotype[i].direction = REG_IO_IN;
    }
    else{
      list->iotype[i].direction = REG_IO_OUT;
    }
      /*  list->iotype[i].frequency*/
    ioPtr = ioPtr->next; i++;
  }

  xmlFreeDoc(doc);   xmlCleanupParser();
  soap_end(&mySoap); soap_done(&mySoap);
  return REG_SUCCESS;
}

/*-----------------------------------------------------------------*/

char *Create_checkpoint_tree_wsrf(const char *factory, 
				  const char *metadata){

  struct soap                        mySoap;
  struct cpt__createNewTreeResponse  out;
  char       *pchar;
  static char epr[REG_MAX_STRING_LENGTH];

  soap_init(&mySoap);

  if(soap_call_cpt__createNewTree(&mySoap, 
				  factory, 
				  "", /* soap Action */
				  (char *)metadata, 
				  &out) != SOAP_OK){
    fprintf(stderr, "STEERUtils: ERROR: Create_checkpoint_tree_wsrf: Call to "
	    "createNewTree on %s failed\n", factory);
    soap_print_fault(&mySoap, stderr);
    soap_end(&mySoap); soap_done(&mySoap);
    return NULL;
  }

  if(strstr(out._createNewTreeReturn, "Address>")){

    pchar = strstr(out._createNewTreeReturn, "</");
    if(pchar)*pchar = '\0';
    pchar = strchr(out._createNewTreeReturn, '>');
    strncpy(epr, ++pchar, REG_MAX_STRING_LENGTH);
  }
  else{
    strncpy(epr, out._createNewTreeReturn, REG_MAX_STRING_LENGTH);
  }
  soap_end(&mySoap); soap_done(&mySoap);
  return epr;
}
