#  Makefile for RealityGrid steering library.
#
#  (C) Copyright 2002, 2004, University of Manchester, United Kingdom,
#  all rights reserved.
#
#  This software is produced by the Supercomputing, Visualization and
#  e-Science Group, Manchester Computing, University of Manchester
#  as part of the RealityGrid project (http://www.realitygrid.org),
#  funded by the EPSRC under grants GR/R67699/01 and GR/R67699/02.
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
#  LIMITED  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
#  A PARTICULAR PURPOSE ARE DISCLAIMED. THE ENTIRE RISK AS TO THE QUALITY
#  AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE PROGRAM PROVE
#  DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR
#  CORRECTION.
#
#  Authors........: Andrew Porter, Robert Haines
#
#------------------------------------------------------------------------

include ../Makefile.include
include ../make/Makefile.${ARCH}
include Makefile.shared

INCLUDE_PATH   = ${STEER_INCLUDES}
LIB_PATH       = ${REG_STEER_HOME}/lib$(NBIT)

all:	del ${OBJECTS}
	${CC} ${LD_FLAGS} -o ${LIB_PATH}/libReG_SteerFileSockets.so ${OBJECTS}

del:
	rm -f ${DEL_OBJS}

ReG_Steer_Appside_f.c: ReG_Steer_Appside_f.m4 \
	 ${M4FILE}
	${M4} ${M4FILE} ReG_Steer_Appside_f.m4 > $@

ReG_Steer_Appside_f.o: ReG_Steer_Appside_f.c \
         ../include/ReG_Steer_Appside_internal.h \
         ../include/ReG_Steer_Appside.h \
	 ../include/ReG_Steer_types.h \
	 ../include/ReG_Steer_Common.h \
	 ../include/ReG_Steer_Appside_Sockets.h

ReG_Steering_Initialize.o: ReG_Steering_Initialize.f90
	${F90} ${LIB_C_FLAGS} \
        -DREG_SOCKET_SAMPLES=0 \
        -DREG_SOAP_STEERING=0 \
        -DREG_DEBUG=0 \
        -DREG_DEBUG_FULL=0 \
        -DREG_STEER_LIB_VERSION=\"${REG_STEER_LIB_VERSION}\" \
        ${INCLUDE_PATH} -c $< -o $@

.c.o:
	${CC} ${LIB_C_FLAGS} \
        -DREG_SOCKET_SAMPLES=1 \
        -DREG_SOAP_STEERING=0 \
        -DREG_DEBUG=0 \
        -DREG_DEBUG_FULL=0 \
        -DREG_STEER_LIB_VERSION=\"${REG_STEER_LIB_VERSION}\" \
        ${INCLUDE_PATH} -c $< -o $@
