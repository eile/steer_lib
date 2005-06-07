!----------------------------------------------------------------------------
!  F90 code to investigate meaning of KIND parameters on current
!  machine architecture.
!
!  (C) Copyright 2005, University of Manchester, United Kingdom,
!  all rights reserved.
!
!  This software was developed by the RealityGrid project
!  (http://www.realitygrid.org), funded by the EPSRC under grants
!  GR/R67699/01 and GR/R67699/02.
!
!  LICENCE TERMS
!
!  Redistribution and use in source and binary forms, with or without
!  modification, are permitted provided that the following conditions
!  are met:
!  1. Redistributions of source code must retain the above copyright
!     notice, this list of conditions and the following disclaimer.
!  2. Redistributions in binary form must reproduce the above copyright
!     notice, this list of conditions and the following disclaimer in the
!     documentation and/or other materials provided with the distribution.
!
!  THIS MATERIAL IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
!  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
!  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
!  A PARTICULAR PURPOSE ARE DISCLAIMED. THE ENTIRE RISK AS TO THE QUALITY
!  AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE PROGRAM PROVE
!  DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR
!  CORRECTION.  
!
!  Authors........: Andrew Porter, Robert Haines    
!---------------------------------------------------------------------------

PROGRAM max_kind
  IMPLICIT none

  INTEGER :: i
  INTEGER :: ikind
  REAL    :: log10_2
  INTEGER :: KIND_FROM_BITS
  INTEGER :: nbits
  CHARACTER (LEN=10) :: type

! Prints the kind parameter of the largest integer the machine
! can represent.  Also finds the KIND parameter that corresponds to each
! C type.
! ARP, 9/8/2002

  log10_2 = LOG10(2.)

  DO i=128,2,-1

    ! n = SELECTED_IND_KIND(R) returns the KIND parameter of an integer 
    ! that covers -10**R < n < 10**R or -1 if none exists.
    ! Max. signed integer, n, represented by nbits is n = 2**(nbits - 1) - 1
    !ikind = SELECTED_INT_KIND(INT((LOG10(2.0**(i-1) - 1.0))))
    ikind = KIND_FROM_BITS(i)

    IF (ikind .ne. -1) EXIT
  END DO

  IF (ikind .ne. -1) THEN

    WRITE(*,*) 'Max. kind param = ', ikind, ' for +/-',(2.0**(i-1) - 1.0), &
               'or ', i, 'bits'
  ELSE
    WRITE(*,*) 'Didn''t find any valid KIND parameters!'
  END IF

  WRITE (*,*) 'KIND of 1     = ', KIND(1)
  WRITE (*,*) 'KIND of 1.0   = ', KIND(1.)
  WRITE (*,*) 'KIND of 1.0d0 = ', KIND(1.0D0)

  type = " "
  type = "short"
  type(6:6) = CHAR(0)

  CALL sizeof_f(type, nbits)
  IF(nbits .gt. 0)THEN
    nbits = 8*nbits
    WRITE (*,*) 'short (', nbits, ' bits) corresponds to KIND = ', KIND_FROM_BITS(nbits)
  END IF

  type = " "
  type = "int"
  type(4:4) = CHAR(0)

  CALL sizeof_f(type, nbits)
  IF(nbits .gt. 0)THEN
    nbits = 8*nbits
    WRITE (*,*) 'int (', nbits, ' bits) corresponds to KIND = ', KIND_FROM_BITS(nbits)
  END IF

  type = " "
  type = "long"
  type(5:5) = CHAR(0)

  CALL sizeof_f(type, nbits)
  IF(nbits .gt. 0)THEN
    nbits = 8*nbits
    WRITE (*,*) 'long (', nbits, ' bits) corresponds to KIND = ', KIND_FROM_BITS(nbits)
  END IF

  type = " "
  type = "float"
  type(6:6) = CHAR(0)

  CALL sizeof_f(type, nbits)
  IF(nbits .gt. 0)THEN
    nbits = 8*nbits
    WRITE (*,*) 'float (', nbits, ' bits) corresponds to KIND = ', KIND_FROM_BITS(nbits)
  END IF

  type = " "
  type = "double"
  type(7:7) = CHAR(0)

  CALL sizeof_f(type, nbits)
  IF(nbits .gt. 0)THEN
    nbits = 8*nbits
    WRITE (*,*) 'double (', nbits, ' bits) corresponds to KIND = ', KIND_FROM_BITS(nbits)
  END IF

END PROGRAM max_kind

FUNCTION KIND_FROM_BITS(nbits)
  IMPLICIT none

  INTEGER :: KIND_FROM_BITS
  INTEGER :: nbits

  ! n = SELECTED_IND_KIND(R) returns the KIND parameter of an integer 
  ! that covers -10**R < n < 10**R or -1 if none exists.
  ! Max. signed integer, n, represented by nbits is n = 2**(nbits - 1) - 1

  KIND_FROM_BITS = SELECTED_INT_KIND(INT((LOG10(2.0**(nbits-1) - 1.0))))

END FUNCTION KIND_FROM_BITS
