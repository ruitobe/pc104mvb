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
** WORKFILE::  host_dep.h
**---------------------------------------------------------------------
** TASK::
   Application Programmers Interface (API) for PC104 access.
   Definitions of host dependencies.

**---------------------------------------------------------------------
** AUTHOR::    e09renu0
** CREATED::   30.04.1997
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

#ifndef HOST_DEP_H_
#define HOST_DEP_H_

/*---------- basic type definitions for application -----------------*/
typedef short          SWORD16;
typedef unsigned short UNSIGNED16;
typedef unsigned char  UNSIGNED8;
typedef unsigned long  UNSIGNED32;

#define SIZEOFWORD64   (sizeof(UNSIGNED8)*8)

#ifdef CLASS2
typedef int            BOOLEAN;
#endif

/*---------- basic type definitions for dual port ram access --------*/
#define VOL volatile
typedef unsigned char      TM_TYPE_BYTE;
typedef unsigned short     TM_TYPE_WORD;
typedef VOL unsigned short TM_TYPE_RWORD;  /*Always accessed directly*/


/*---------- basic type definitions for NSDB access -----------------*/
typedef unsigned char  OCTET;
typedef unsigned short WORD16;
typedef unsigned long  WORD32;

/*----------------- Support of BIG NSDBs ----------------------------*/
/* Support of BIG NSDBs for segmented memory architectures:
 *
 * If the NSDB is larger than the maximal memory size of one data structure,
 * e.g. on an Infineon C165 target with Keil C166 compiler this is 16 KByte,
 * then you have to set the BIG_NSDB_SUPPORT switch.
 *
 * With the switch BIG_NSDB_SUPPORT set, for ALL accesses to the NSDB,
 * 
 *
 */
#ifdef BIG_NSDB_SUPPORT
# define XNSDBWORD UNSIGNED32  /* is used for the variable gTotalNSDBLength */
# define XHUGE xhuge
# include <string.h>
# define XHMEMMOVE xmemmove 
# define XHMEMCHR  xmemchr
# define XHMEMCMP  xmemcmp
# ifdef MVB_M
#  include <stdlib.h>
#  define XHFREE xfree
#  define XHMALLOC xmalloc
# endif
#else
# define XNSDBWORD UNSIGNED16
# define XHUGE     /* nothing */
# include <string.h>
# define XHMEMMOVE memmove
# define XHMEMCHR  memchr
# define XHMEMCMP  memcmp
# ifdef MVB_M
#  include <stdlib.h>
#  define XHFREE free
#  define XHMALLOC malloc
# endif
#endif              


/* to fake silly compilers */
#define UNREFERENCED(x)  x = x

/*---------- address arithmetics ------------------------------------*/

#if defined (O_SEG)
/*  On Intel-80x86-based systems:
    The addition is restricted to segment part only.  The offset part
    is not affected.  Therefore, granularity is limited to 16 bytes or
    integral multiple of it. */

#define LCX_PTR_ADD_SEG(ptr, ofs)  \
    ((void*) (((char *) (ptr)) +  \
    ((((UNSIGNED32) (ofs)) & 0xFFFF0L) << 12)))

#elif defined O_166

/* Take care of special C166 address calculation: offset is only 14 bit in a pointer */
#define LCX_PTR_ADD_SEG(ptr,ofs)  ( (void *) (((unsigned long) (ptr)) + \
          ((unsigned long) (ofs))) )

#else
/*  Linear addressing: pure address addition takes place. */

#define LCX_PTR_ADD_SEG(ptr, ofs)  \
    ((void *) (((char *) (ptr)) + ((UNSIGNED32) (ofs)) ))

#endif


/*---------- pointer conversion and access --------------------------*/

/* macro makes pointer of type "type" out of UNSIGNED32 content of arg,
   which is also a pointer */
#define P_OF_CONTENT(type,arg)   ((type)*((UNSIGNED32 XHUGE *)(arg)))

/* macro makes type "type" out of UNSIGNED32 casted arg */
#define P_OF_ULONG(type,arg)     ((type) (UNSIGNED32)(arg))

/* macro returns UNSIGNED32 casted pointer */
#define PTR2ULONG(ptr)           ((UNSIGNED32)(ptr))

/*---------- swap operations ----------------------------------------*/

/* NOTE: The argument to the macro SWAP should be of type UNSIGNED16 */
#define SWAP16(a)  ((((a) & 0x00ff) << 8) | ((UNSIGNED16)((a) & 0xff00) >> 8))

/* NOTE: The arguments to the macro SWAP32_INC should be pointers to
         UNSIGNED16
   NOTE: the construction "do { ... } while(0)" is used to prevent strange
         syntax errors in conjunction with the use of ";" */
#define SWAP32(dest,src)  \
    do {  \
        UNSIGNED16 tmp;  \
        tmp = SWAP16(*(UNSIGNED16*)(src));  \
        *(UNSIGNED16*)(dest) = SWAP16(*(((UNSIGNED16*)(src))+1));  \
        *(((UNSIGNED16*)(dest))+1) = tmp;  \
    } while(0)

#define XHSWAP32(dest,src)  \
    do {  \
        UNSIGNED16 tmp;  \
        tmp = SWAP16(*(UNSIGNED16 XHUGE *)(src));  \
        *(UNSIGNED16 XHUGE *)(dest) = SWAP16(*(((UNSIGNED16 XHUGE *)(src))+1));  \
        *(((UNSIGNED16 XHUGE *)(dest))+1) = tmp;  \
    } while(0)

#ifdef BIGENDIAN
#define SWAPWORD(a)  (((UNSIGNED32)((a) & 0x0000ffff) << 16) | ((UNSIGNED32)((a) & 0xffff0000) >> 16))
#else
#define SWAPWORD(a)  (a)
#endif

/*---------- Transfer data from Net to Host and back ----------------*/

#ifdef BIGENDIAN
# define NtoH16(x)       (x)             /* Net to Host 16 bit */
# define NtoH32(d,s)  \
     do {  \
         *(UNSIGNED32*)(d) = *(UNSIGNED32*)(s);  \
     } while(0)                          /* Net to Host 32 bit */
# define XHNtoH32(d,s)  \
     do {  \
         *(UNSIGNED32 XHUGE *)(d) = *(UNSIGNED32 XHUGE *)(s);  \
     } while(0)                          /* Net to Host 32 bit XHUGE */
# define HtoN16(x)       (x)             /* Host to Net 16 bit */
# define HtoN32(d,s)  \
     do {  \
         *(UNSIGNED32*)(d) = *(UNSIGNED32*)(s);  \
     } while(0)                          /* Host to Net 32 bit */
# define XHHtoN32(d,s)  \
     do {  \
         *(UNSIGNED32 XHUGE *)(d) = *(UNSIGNED32 XHUGE *)(s);  \
     } while(0)                          /* Host to Net 32 bit XHUGE */
#else
# define NtoH16(x)       SWAP16(x)       /* Net to Host 16 bit */
# define NtoH32(d,s)     SWAP32(d,s)     /* Net to Host 32 bit */
# define XHNtoH32(d,s)   XHSWAP32(d,s)   /* Net to Host 32 bit XHUGE */
# define HtoN16(x)       SWAP16(x)       /* Host to Net 16 bit */
# define HtoN32(d,s)     SWAP32(d,s)     /* Host to Net 32 bit */
# define XHHtoN32(d,s)   XHSWAP32(d,s)   /* Host to Net 32 bit XHUGE */
#endif


/*---------- timeout after trying to switch line --------------------*/
/* This is an arbitrary value big enough to allow enough time for
   switching, even for fast CPUs */
#define LCI_LS_LIMIT    10


/*---------- pre- and postfix for access of traffic store -----------*/
#define PREFIX_WRITE_TS
#define POSTFIX_WRITE_TS
#define PREFIX_READ_TS
#define POSTFIX_READ_TS

/*---------- pre- and postfix for access of MVBC registers ----------*/
#define PREFIX_WRITE_MVBC
#define POSTFIX_WRITE_MVBC
#define PREFIX_READ_MVBC
#define POSTFIX_READ_MVBC

/*---------- function prototypes for access on traffic store --------*/

extern void writeWordToTS(
    void *addr,             /* Address */
    TM_TYPE_WORD val        /* Value to write */
);
/*{
    The function writes the value to the address which is located in TS.
    Swapping is performed for different endianess.
}*/


extern TM_TYPE_WORD readWordFromTS(
    void* addr              /* Address */
);
/*{
    The function reads a value from the address which is located in TS.
    Swapping is performed for different endianess.
}*/


/*---------- function prototypes for access on MVBC registers -------*/
/* The next two functions are added for the case, in which it is necessary
 * to have different access modes to 1) the "ordinary" traffic memory and
 *                                   2) the MVBC internal registers.
 * E.g. using the ASIC-Kit with a dual-ported RAM traffic memory and 
 * an access to the MVBC registers at a different base address. 
 */
extern void writeWordToMVBC(
    void *addr,             /* Address */
    TM_TYPE_WORD val        /* Value to write */
);
/*{
    The function writes the value to the address which is located in MVBC
    intenal registers.
    Swapping is performed for different endianess.
}*/


extern TM_TYPE_WORD readWordFromMVBC(
    void* addr              /* Address */
);
/*{
    The function reads a value from the address which is located in MVBC
    internal registers.
    Swapping is performed for different endianess.
}*/


/*---------- function prototypes for interrupt-manipulation ---------*/ 
extern void MVB_INT_ENABLE(void); 
extern void MVB_INT_DISABLE(void);


/*---------- function prototypes for wait-functions ---------*/ 
extern void Wait2MS(void); 
extern void Wait100US(void);

/*---------- macro for the calling of the watchdog-retrigger-function ---------*/ 
#define SRVWDT()  

/*---------- function prototypes for MVB-M ---------*/ 
#if defined (MVB_M)
extern UNSIGNED8 ReadMVBMConfig(
    UNSIGNED8          *buffer,
    UNSIGNED16         bytes
);
/*{
    The function reads the MVBM-Configuration out of the non-volatile memory.
    The function is host-specific. Because of this it has to be implemented by the user.
 
    Return values:  MVBM_OK
                    MVBM_CONFIG_NOT_AVAIL : configuration could not be read because of any reason
}*/

extern UNSIGNED8 SaveMVBMConfig(
    UNSIGNED8          *buffer,
    UNSIGNED16         bytes
);
/*{
    The function saves the MVBM-Configuration to the non-volatile memory.
    The function is host-specific. Because of this it has to be implemented by the user.
 
    Return values:  MVBM_OK
                    MVBM_CONFIG_NOT_SAVED : saving was not possible because of any reason
}*/
#endif

#ifdef CLASS2

extern char *nse_printf(char *fmt, ...);  /* see pil.c */

#ifdef O_LINUX
# include <syslog.h>

/* openlog("mdd", LOG_CONS, LOG_DAEMON); */

# define RECORD_M_REBOOT(p_msg)      syslog(LOG_ERR, p_msg) 
# define RECORD_M_EVENT(p_msg)       syslog(LOG_NOTICE, p_msg)
#elif defined O_166
# define RECORD_M_REBOOT(p_msg)      {}/* stays empty */
# define RECORD_M_EVENT(p_msg)       {}/* stays empty */

#else
# define RECORD_M_REBOOT(p_msg)      {}/* stays empty */
# define RECORD_M_EVENT(p_msg)       {}/* stays empty */

#endif
/* RECORD_M_REBOOT() is called after critical errors. After that, the messenger shall be restarted.
   RECORD_M_EVENT is only a notice. The system can recover after that and continue. */

#endif

#endif
