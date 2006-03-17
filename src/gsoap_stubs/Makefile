#  Makefile for gSoap bindings for RealityGrid steering lib
#
#  (C) Copyright 2006, University of Manchester, United Kingdom,
#  all rights reserved.
#
#  This software was developed by the RealityGrid project
#  (http://www.realitygrid.org), funded by the EPSRC under grants
#  GR/R67699/01 and GR/R67699/02.
#
#  LICENCE TERMS
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#
#  THIS MATERIAL IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
#  A PARTICULAR PURPOSE ARE DISCLAIMED. THE ENTIRE RISK AS TO THE QUALITY
#  AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE PROGRAM PROVE
#  DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR
#  CORRECTION.
#
#  Authors........: Andrew Porter
#
#------------------------------------------------------------------------

SOAP_CPP2 = ${HOME}/projects/gsoap-2.7/soapcpp2/src/soapcpp2
include ../../Makefile.include
include ../../make/Makefile.${ARCH}

.SUFFIXES: .o .c

all: soap_stubs

soap_stubs: all_hdrs.h SWS.h SGS.h SGSFactory.h SWSFactory.h \
           ServiceGroupRegistration.h RealityGridTreeFactory.h \
           RealityGridTree.h
	${SOAP_CPP2} -c all_hdrs.h

install:
	mv -i soapC.c ../.
	mv -i soapClient.c ../.
	mv -i soapH.h ../../include/.
	mv -i soapStub.h ../../include/.
	mv -i soapSWS.nsmap ../../include/soapRealityGrid.nsmap

clean:
	rm -f soapClient.c soapC.c soapH.h *.nsmap soapStub.h
	rm -f soapClientLib.c soapServer.c soapServerLib.c
	rm -f soapsoap*.h *.req.xml *.res.xml *.wsdl *.xsd