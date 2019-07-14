/**********************************************************************
Copyright (C) Siemens AG 1997-2004 All rights reserved. Confidential

Permission is hereby granted, without written agreement and without
license or royalty fees, to use and modify this software and its
documentation for the use with SIEMENS PC104 cards.

IN NO EVENT SHALL SIEMENS AG BE LIABLE TO ANY PARTY FOR DIRECT,
INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF SIEMENS AG
HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

SIEMENS AG SPECIFICALLY DISCLAIMS ANY WARRANTIES,INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN
"AS IS" BASIS, AND SIEMENS AG HAS NO OBLIGATION TO PROVIDE MAINTENANCE,
SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
**=====================================================================
** PROJECT::   TCN
** MODULE::    PC104 CLASS1.1-1.3
** WORKFILE::  config.h
**---------------------------------------------------------------------
** TASK::
   Application Programmers Interface (API) for PC104 access.
   Configuration parameters.

**---------------------------------------------------------------------
** AUTHOR::    e09renu0
** CREATED::   30.04.1997
** REVISION::  2.10.00
**---------------------------------------------------------------------
** CONTENTS::
      functions:
          none
**---------------------------------------------------------------------
** NOTES::     -
**---------------------------------------------------------------------
  
**********************************************************************/

#ifndef CONFIG_H_
#define CONFIG_H_

/*------------------List of compiler switches to be used-------------*/ 

/*---------- processor little/big endian ----------------------------*/
/* #define BIGENDIAN */ /* not defined for little endian */


/*---------- processor uses segmentation ----------------------------*/
/* #define O_SEG */  /* Special Case: Intel Processors, segmentation */


/*----- Support of BIG NSDBs for segmented memory architectures------*/
/*#define BIG_NSDB_SUPPORT  */


/*---------- class mode  of software --------------------------------*/
/* always defined: Class 1.1
   Only one of the following may be defined */
 /*#define CLASS13 */   /* Class 1.3 */
 /*#define CLASS12 */   /* Class 1.2 */
 /*#define CLASS2  */   /* Class 2 */

#ifdef CLASS2 
# define CLASS13
# define CLASS2_NSDB
#endif

/*--------------- Definition of MVBM --------------------------------*/
/* May only be defined in Class 1.2 or Class 1.3 */
#if defined(CLASS12) || defined(CLASS13)
/* #define MVB_M  */
#endif  

/*--------------- shall CRC be supported ? --------------------------*/
/* #define USE_CRC */


/*-------- not compiler switch, but other definitions----------------*/

/*---------- basic definitions --------------------------------------*/
#ifndef NULL
# define NULL 0L
#endif

/*---------- check for correct memory-model on INTEL-Machines -------*/
#ifdef M_I86
# if !defined(M_I86LM)
#  error  Please change memory-model to LARGE
# endif
#endif  /* ifdef M_I86 */     

/*----------- for Keil C166 Compiler --------------------------------*/
#ifdef O_166
# pragma mod167        /* C167 mode with 16MB address space */
# pragma fix167        /* fix 167 processor bugs */
# pragma ot(5,speed)   /* optimize level 5 and speed */
# pragma wl(3)         /* 2 = display all warnings, except C166 spec. */
# pragma large         /* memory model */
# pragma debug         /* enables debug info in obj */
#endif

/*--------------------- if using a LINUX-like system ----------------*/
#ifdef O_LINUX

# define MVB_NODE  "/dev/mvb"

#endif

/*-------------------- MVB specific definitions ---------------------*/


/*---------- value for supervision register STSR --------------------*/
/* Value defines supervision time interval in milliseconds.
   The user is responsible to avoid TM access overloading due to
   supervising too many docks within a too small time interval.

   Possible values:
   TM_STSR_INTERV_OFF
   TM_STSR_INTERV_1MS
   TM_STSR_INTERV_2MS
   TM_STSR_INTERV_4MS
   TM_STSR_INTERV_8MS
   TM_STSR_INTERV_16MS
   TM_STSR_INTERV_32MS
   TM_STSR_INTERV_64MS
   TM_STSR_INTERV_128MS
   TM_STSR_INTERV_256MS */
#define TM_STSR_INTERV          TM_STSR_INTERV_OFF

/* Number of docks to be supervised.
   User must assure that this value does not exceed the number of
   available docks in the data area. Bus traffic overflow can occur
   by wrong combination of intervall and number of docks ! */
#define TM_STSR_DOCKS           0

/*---------- MVBC in Intel or Motorola mode -------------------------*/
/* In this application the MVBC will always work in Intel Mode.
   The value can be 0 (Motorola) or 1 (Intel). */
#define LCX_INTEL_MODE          1 /* Intel Mode activated */


/*---------- number of managed traffic stores -----------------------*/
#define LCX_TM_COUNT            1


/*---------- traffic memory access waitstates -----------------------*/
/* Fill values for all traffic stores (LCX_TM_COUNT) between brackets.
   Example: { TM_SCR_WS_0, TM_SCR_WS_1, TM_SCR_WS_1 }
   Possible values:
   TM_SCR_WS_3
   TM_SCR_WS_2
   TM_SCR_WS_1
   TM_SCR_WS_0 */
#define LCX_WAITSTATES          { TM_SCR_WS_3 }

/*---------- traffic memory arbitration mode -----------------------*/
/* Fill values for all traffic stores (LCX_TM_COUNT) between brackets.
   Example: { TM_SCR_WS_0, TM_SCR_WS_1, TM_SCR_WS_1 }
   Possible values:
   TM_SCR_ARB_3
   TM_SCR_ARB_2
   TM_SCR_ARB_1
   TM_SCR_ARB_0 */
#define LCX_ARB_MODE          { TM_SCR_ARB_0 }


/*---------- Memory Control Mode (MCM) -----------------------*/
/* PC/104 usually works with 64K
   Possible values:
   64K: 2 
   32K: 1
   16K: 0 */
#define MEM_MODE  2

/*---------- maximal port index for Class 1.1 -----------------------*/
/* The value depends on the Memory Control Mode (MCM) of the MVBC
   Possible values:
   TM_MCM_64K: 1023 
   TM_MCM_32K:  255
   TM_MCM_16K:  255 */
#if MEM_MODE >= 2
#define LC_TS_MAX_LA_PORT_INDX  1023
#else
#define LC_TS_MAX_LA_PORT_INDX  255
#endif



#endif
