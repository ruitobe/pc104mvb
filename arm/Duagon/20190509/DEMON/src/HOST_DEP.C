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
** WORKFILE::  host_dep.c 
**---------------------------------------------------------------------
** TASK::
   Application Programmers Interface (API) for PC104 access.
   Functions which depend on the host-system

**---------------------------------------------------------------------
** AUTHOR::    e09renu0
** CREATED::   13.01.1998
** REVISION::  2.10.00
**---------------------------------------------------------------------
** CONTENTS::
      functions:
        writeWordToTS()
        readWordFromTS()
        writeWordToMVBC()
        readWordFromMVBC()
        MVB_INT_ENABLE()
        MVB_INT_DISABLE()
        Wait2MS()
        Wait100US()

    #if defined (MVB_M)
         ReadMVBMConfig()
         SaveMVBMConfig()
    #endif

**---------------------------------------------------------------------
** NOTES::     -
**---------------------------------------------------------------------
  
**********************************************************************/

#include "config.h"
#include "host_dep.h"
#include "dpr_dep.h"
#include "cl1_ret.h"
#include "cl1_1.h"
#include "localbus.h"

uint16_t  test_data4 = 0x0;

#if defined (MVB_M)
#include <stdio.h>
#endif

#ifdef WIN32
# include "windows.h"
#endif
/*---------- access to traffic-store --------------------------------*/

void writeWordToTS(void* addr, TM_TYPE_WORD val)
{
    PREFIX_WRITE_TS
	  Localbus_WriteData((uint32_t)addr, &val);
    //*(TM_TYPE_WORD*) addr = val;
    POSTFIX_WRITE_TS
    return;
} /* writeWordToTS */


TM_TYPE_WORD readWordFromTS(void* addr)
{
    TM_TYPE_WORD val;
    PREFIX_READ_TS
	 	/*W/A for HW issue*/
		//Localbus_WriteData(0xEd600, &test_data4);
	  val = (TM_TYPE_WORD)Localbus_ReadData((uint32_t)addr);
    //val = (*(TM_TYPE_WORD*) addr);
    POSTFIX_READ_TS
    return (val);
} /* readWordFromTS */


void writeWordToMVBC(void* addr, TM_TYPE_WORD val)
{
    PREFIX_WRITE_MVBC
	  Localbus_WriteData((uint32_t)addr, &val);
    //*(TM_TYPE_WORD*) addr = val;
    POSTFIX_WRITE_MVBC
    return;
} /* writeWordToMVBC */


TM_TYPE_WORD readWordFromMVBC(void* addr)
{
    TM_TYPE_WORD val;
    PREFIX_READ_MVBC
	  Localbus_WriteData(0xEd600, &test_data4);
	  val = (TM_TYPE_WORD)Localbus_ReadData((uint32_t)addr);
    //val = (*(TM_TYPE_WORD*) addr);
    POSTFIX_READ_MVBC
    return (val);
} /* readWordFromMVBC */


/*---------- interrupt-handling -------------------------------------*/
void MVB_INT_ENABLE(void)
{
#ifdef O_LINUX
  /* not necessary, all critical sections have been moved to the driver part */
#elif defined O_166
# error  Please insert function to enable interrupts on hostsystem here 
#elif defined WIN32
# error  Please insert function to enable interrupts on hostsystem here 
#else  
//# error  Please insert function to enable interrupts on hostsystem here 
#endif
}


void MVB_INT_DISABLE(void)
{
#ifdef O_LINUX
  /* not necessary, all critical sections have been moved to the driver part */
#elif defined O_166
# error  Please insert function to disable interrupts on hostsystem here 
#elif defined WIN32
# error  Please insert function to disable interrupts on hostsystem here 
#else  
//# error  Please insert function to disable interrupts on hostsystem here 
#endif
}

    
/*---------- waiting-functions --------------------------------------*/
void Wait2MS(void)
{
#ifdef O_LINUX
  /* not necessary, all critical sections have been moved to the driver part */
#elif defined O_166
# error  Please insert wait function for about 2 ms here to clear MVBC rx buffer 
#elif defined WIN32
   Sleep(2);
#else  
//# error  Please insert wait function for about 2 ms here to clear MVBC rx buffer 
#endif
}


void Wait100US(void)
{
#ifdef O_LINUX
  /* not necessary, all critical sections have been moved to the driver part */
#elif defined O_166
# error  Please insert wait function for about 100 us here to allow MVBC access to traffic store 
#elif defined WIN32
  Sleep(1);
#else  
//# error  Please insert wait function for about 100 us here to allow MVBC access to traffic store 
#endif
}
    

/*---------- functions for MVB-M ---------*/ 
#if defined (MVB_M)
UNSIGNED8 ReadMVBMConfig(
    UNSIGNED8          *buffer,
    UNSIGNED16         bytes
)
{
#ifdef O_LINUX
    UNREFERENCED(buffer);
    UNREFERENCED(bytes);
    return 0;
#elif defined O_166
    UNREFERENCED(buffer);
    UNREFERENCED(bytes);
    return 0;
#elif defined WIN32
    UNREFERENCED(buffer);
    UNREFERENCED(bytes);
    return 0;
#else  
# error  Please insert function to read the MVBM-Configuration 
#endif
/* Return values:
                 MVBM_OK
                 MVBM_CONFIG_NOT_AVAIL */
}

UNSIGNED8 SaveMVBMConfig(
    UNSIGNED8          *buffer,
    UNSIGNED16         bytes
)
{
#ifdef O_LINUX
    UNREFERENCED(buffer);
    UNREFERENCED(bytes);
    return 0;
#elif defined O_166
    UNREFERENCED(buffer);
    UNREFERENCED(bytes);
    return 0;
#elif defined WIN32
    UNREFERENCED(buffer);
    UNREFERENCED(bytes);
    return 0;
#else  
# error  Please insert function to save the MVBM-Configuration 
#endif
/* Return values:
                 MVBM_OK
                 MVBM_CONFIG_NOT_SAVED */



}
#endif

