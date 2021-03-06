#
#  The RealityGrid Steering Library
#
#  Copyright (c) 2002-2010, University of Manchester, United Kingdom.
#  All rights reserved.
#
#  This software is produced by Research Computing Services, University
#  of Manchester as part of the RealityGrid project and associated
#  follow on projects, funded by the EPSRC under grants GR/R67699/01,
#  GR/R67699/02, GR/T27488/01, EP/C536452/1, EP/D500028/1,
#  EP/F00561X/1.
#
#  LICENCE TERMS
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#
#    * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#
#    * Redistributions in binary form must reproduce the above
#      copyright notice, this list of conditions and the following
#      disclaimer in the documentation and/or other materials provided
#      with the distribution.
#
#    * Neither the name of The University of Manchester nor the names
#      of its contributors may be used to endorse or promote products
#      derived from this software without specific prior written
#      permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
#  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
#  COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
#  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
#  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
#  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
#  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
#  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
#  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.
#
#  Author: Robert Haines

# configure the header file
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/ReG_Examples_Config.h.in"
  "${PROJECT_BINARY_DIR}/ReG_Examples_Config.h"
  @ONLY
)

if(NOT REG_DYNAMIC_MOD_LOADING)
  if(REG_BUILD_MODULAR_LIBS)
    foreach(type ${REG_MODULES_TYPES})
      set(REG_EXAMPLES_MODULES_LINK ${REG_EXAMPLES_MODULES_LINK} "ReG_Steer_${type}_${REG_USE_MODULE_${type}}")
    endforeach(type ${REG_MODULES_TYPES})
  endif(REG_BUILD_MODULAR_LIBS)
endif(NOT REG_DYNAMIC_MOD_LOADING)

add_subdirectory(mini_app)
add_subdirectory(mini_steerer)
add_subdirectory(sink)
add_subdirectory(simple)

if(REG_BUILD_FORTRAN_WRAPPERS)
  add_subdirectory(mini_app_f90)
  if(REG_BUILD_MPI_EXAMPLE)
    add_subdirectory(mini_app_f90_parallel)
  endif(REG_BUILD_MPI_EXAMPLE)
endif(REG_BUILD_FORTRAN_WRAPPERS)
