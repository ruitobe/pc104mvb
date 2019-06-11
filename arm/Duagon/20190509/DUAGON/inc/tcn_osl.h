#ifndef TCN_OSL_H
#define TCN_OSL_H

/* ==========================================================================
 *
 *  File      : TCN_OSL.H
 *
 *  Purpose   : Operating System Libraries
 *              NOTE: Standard C and POSIX compatible OS (e.g. eCos)
 *
 *  Project   : General TCN Driver Software
 *              - TCN Software Architecture (d-000487-nnnnnn)
 *
 *  Version   : d-000543-nnnnnn
 *
 *  Licence   : Duagon Software Licence (see file 'licence.txt')
 *
 * --------------------------------------------------------------------------
 *
 *  (C) COPYRIGHT, Duagon GmbH, CH-8953 Dietikon, Switzerland
 *  All Rights Reserved.
 *
 * ==========================================================================
 */


/* ==========================================================================
 *
 *  Pre-processor Definitions:
 *  --------------------------
 *  - __ECOS__            - eCos using GNU GCC
 *  - _MSC_VER            - DOS, Windows using Microsoft Visual C++
 *  - TCN_OSL_INCLUDE_... - value=0 -> disable include
 *                        - value=1 -> Standard C and POSIX Libraries
 *                        - value=2 -> special libraries for MS Visual C++
 *
 * ==========================================================================
 */


/* ==========================================================================
 *
 *  Project specific Definitions used for Conditional Compiling
 *
 * ==========================================================================
 */
#ifdef TCN_PRJ
#   include <tcn_prj.h>
#endif


/* ==========================================================================
 *
 *  Include Files
 *
 * ==========================================================================
 */

/* ==========================================================================
 *  TCN Software Architecture
 * ==========================================================================
 */
#include <tcn_def.h>

/* ==========================================================================
 *  Standard C Library
 *  ------------------
 *  - TCN_OSL_INCLUDE_STANDARDC_ERRNO_H
 *  - TCN_OSL_INCLUDE_STANDARDC_STDIO_H
 *  - TCN_OSL_INCLUDE_STANDARDC_STDLIB_H
 *  - TCN_OSL_INCLUDE_STANDARDC_STRING_H
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  system error numbers
 * --------------------------------------------------------------------------
 */
#ifndef TCN_OSL_INCLUDE_STANDARDC_ERRNO_H
#   define TCN_OSL_INCLUDE_STANDARDC_ERRNO_H    1
#endif
#if (TCN_OSL_INCLUDE_STANDARDC_ERRNO_H == 1)
#   include <errno.h>
#endif

/* --------------------------------------------------------------------------
 *  standard buffered input/output
 *  ------------------------------
 *  - function 'printf()' (test and debugging)
 * --------------------------------------------------------------------------
 */
#ifndef TCN_OSL_INCLUDE_STANDARDC_STDIO_H
#   define TCN_OSL_INCLUDE_STANDARDC_STDIO_H    1
#endif
#if (TCN_OSL_INCLUDE_STANDARDC_STDIO_H == 1)
#   include <stdio.h>
#endif

/* --------------------------------------------------------------------------
 *  standard library definitions
 *  ----------------------------
 *  - function 'malloc()'
 *  - function 'realloc()'
 *  - function 'free()'
 * --------------------------------------------------------------------------
 */
#ifndef TCN_OSL_INCLUDE_STANDARDC_STDLIB_H
#   define TCN_OSL_INCLUDE_STANDARDC_STDLIB_H   1
#endif
#if (TCN_OSL_INCLUDE_STANDARDC_STDLIB_H == 1)
#   include <stdlib.h>
#endif

/* --------------------------------------------------------------------------
 *  string operations
 *  -----------------
 *  - function 'memset()'
 *  - function 'memcpy()'
 *  - function 'strlen()'
 *  - function 'strcpy()'
 * --------------------------------------------------------------------------
 */
#ifndef TCN_OSL_INCLUDE_STANDARDC_STRING_H
#   define TCN_OSL_INCLUDE_STANDARDC_STRING_H   1
#endif
#if (TCN_OSL_INCLUDE_STANDARDC_STRING_H == 1)
#   include <string.h>
#endif

/* ==========================================================================
 *  POSIX Library
 *  -------------
 *  - TCN_OSL_INCLUDE_POSIX_FILE_SYSTEM_H
 *  - TCN_OSL_INCLUDE_POSIX_PTHREAD_H
 *  - TCN_OSL_INCLUDE_POSIX_SCHED_H
 *  - TCN_OSL_INCLUDE_POSIX_SEMAPHORE_H
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  file system
 *  -----------
 *  NOTE: Only used by Configuration Manager (CM) and TNM Agent (MA).
 *  - function 'access()'
 *  - function 'open()'
 *  - function 'close()'
 *  - function 'read()'
 *  - function 'write()'
 *  - function 'lseek()'
 *  - function 'unlink()'
 *  - function 'stat()'
 *  - function 'pathconf()'
 *  - function 'getcwd()'
 *  - function 'chdir()'
 *  - function 'mkdir()'
 *  - function 'rmdir()'
 *  - function 'opendir()'
 *  - function 'closedir()'
 *  - function 'readdir()'
 *  - function 'rewinddir()'
 *
 *  NOTE:
 *  The Configuration Manager uses only the following subset of the above
 *  primitives:
 *  - function 'open()' (only symbolic constant O_RDONLY is required)
 *  - function 'close()'
 *  - function 'read()'
 *  - function 'lseek()'
 * --------------------------------------------------------------------------
 */
#ifdef _MSC_VER
#   define TCN_OSL_INCLUDE_POSIX_FILE_SYSTEM_H  2
#endif

#ifndef TCN_OSL_INCLUDE_POSIX_FILE_SYSTEM_H
#   define TCN_OSL_INCLUDE_POSIX_FILE_SYSTEM_H  1
#endif
#if (TCN_OSL_INCLUDE_POSIX_FILE_SYSTEM_H == 1)
//#   include <sys/types.h>
//#   include <sys/stat.h>
//#   include <unistd.h>      /* standard symbolic constants and types       */
                            /* (e.g. read, close)                          */
//#   include <fcntl.h>       /* file control options (e.g. open)            */
//#   include <dirent.h>      /* format of directory entries                 */
#endif /* #if (TCN_OSL_INCLUDE_POSIX_FILE_SYSTEM_H == 1) */
#if (TCN_OSL_INCLUDE_POSIX_FILE_SYSTEM_H == 2)
    /* ----------------------------------------------------------------------
     *  support for configuration manager
     * ----------------------------------------------------------------------
     */
//#   include <io.h>      /* low-level IO routines (e.g. open, read)         */
//#   include <fcntl.h>   /* file control options                            */

    /* ----------------------------------------------------------------------
     *  additional support for TCN Agent
     *  (only to compile without warnings)
     * ----------------------------------------------------------------------
     */
                        /* File-access permission modes defined in         */
#   define R_OK 4       /* Microsoft Visual C++ Help:                      */
#   define W_OK 2       /* 00 Existence only                               */
#   define X_OK 4       /* 02 Write permission                             */
#   define F_OK 0       /* 04 Read permission                              */

//#   include <sys/stat.h>

    #define _PC_NAME_MAX 1
    long pathconf(const char *path, int name);

    char *getcwd(char *buf, unsigned long size);
    int chdir(const char *path);
    int mkdir(const char *path, long mode);
    int rmdir(const char *path);

    typedef void* DIR;
    DIR *opendir(const char *dirname);
    int closedir(DIR *dirp);
    struct dirent
    {
        char d_name[256];
    };
    struct dirent *readdir(DIR *dirp);
    void rewinddir(DIR *dirp);
#endif /* #if (TCN_OSL_INCLUDE_POSIX_FILE_SYSTEM_H == 2) */

/* --------------------------------------------------------------------------
 *  mutex
 *  -----
 *  function 'pthread_mutex_init()'
 *  function 'pthread_mutex_lock()'
 *  function 'pthread_mutex_unlock()'
 *
 *  threads
 *  -------
 *  NOTE: Only used by TNM Agent (MA).
 *  - function 'pthread_attr_init()'
 *  - function 'pthread_attr_setinheritsched()'
 *  - function 'pthread_attr_setschedparam()'
 *  - function 'pthread_create()'
 * --------------------------------------------------------------------------
 */
#ifndef TCN_OSL_INCLUDE_POSIX_PTHREAD_H
#   define TCN_OSL_INCLUDE_POSIX_PTHREAD_H      1
#endif
#if (TCN_OSL_INCLUDE_POSIX_PTHREAD_H == 1)
//#   include <pthread.h>
#endif

/* --------------------------------------------------------------------------
 *  execution scheduling (REALTIME)
 *  -------------------------------
 *  NOTE: Only used by TNM Agent (MA).
 *  - function 'sched_yield()'
 * --------------------------------------------------------------------------
 */
#ifndef TCN_OSL_INCLUDE_POSIX_SCHED_H
#   define TCN_OSL_INCLUDE_POSIX_SCHED_H        1
#endif
#if (TCN_OSL_INCLUDE_POSIX_SCHED_H == 1)
//#   include <sched.h>
#endif

/* --------------------------------------------------------------------------
 *  semaphores (REALTIME)
 *  ---------------------
 *  NOTE: Only used by TNM Agent (MA).
 *  - function 'sem_init()'
 *  - function 'sem_wait()'
 *  - function 'sem_post()'
 * --------------------------------------------------------------------------
 */
#ifndef TCN_OSL_INCLUDE_POSIX_SEMAPHORE_H
#   define TCN_OSL_INCLUDE_POSIX_SEMAPHORE_H    1
#endif
#if (TCN_OSL_INCLUDE_POSIX_SEMAPHORE_H == 1)
//#   include <semaphore.h>
#endif

/* ==========================================================================
 *  Miscellaneous
 * ==========================================================================
 */
#ifdef __ECOS__
//#   include <cyg/infra/diag.h> /* diag_printf */
#endif


/* ==========================================================================
 *
 *  Standard C Library (stdio.h)
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Macro     : TCN_OSL_PRINTF
 *
 *  Purpose   : Print formatted output (used for diagnose messages).
 *
 *  Syntax    : #define TCN_OSL_PRINTF
 * --------------------------------------------------------------------------
 */
#ifdef __ECOS__
#   define TCN_OSL_PRINTF   diag_printf
#endif
#ifdef _MSC_VER
#   define TCN_OSL_PRINTF   printf
#endif
#ifndef TCN_OSL_PRINTF
#   define TCN_OSL_PRINTF   printf
#endif

/* ==========================================================================
 *
 *  General Constants and Types
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Enumerated Type   : OSL_RESULT
 *
 *  Purpose           : Result of a procedure.
 * --------------------------------------------------------------------------
 */
typedef enum
{
    OSL_OK      = 0,    /* correct termination  */
    OSL_ERROR   = 1     /* unspecified error    */
}   OSL_RESULT;


/* ==========================================================================
 *
 *  Interrupt Manager
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure Type: OSL_IM_ISR
 *
 *  Purpose       : When an interrupt event occurs, an interrupt service
 *                  routine (ISR) of this type will be called.
 *
 *  Syntax        : typedef OSL_RESULT
 *                  (*OSL_IM_ISR)
 *                  (
 *                      void        *arg
 *                  );
 *
 *  Input         : arg - argument
 *
 *  Return        : any OSL_RESULT
 *
 *  Remarks:      : - The ISR must be connected by procedure
 *                    'osl_im_connect_isr'.
 * --------------------------------------------------------------------------
 */
typedef OSL_RESULT
(*OSL_IM_ISR)
(
    void    *arg
);


/* --------------------------------------------------------------------------
 *  Structured Type   : OSL_IM_ISR_LIST_ENTRY
 *
 *  Purpose           : Type of a list entry describing a connected
 *                      interrupt service routine (ISR).
 *
 *  Syntax            : typedef struct
 *                      {
 *                          OSL_IM_ISR  isr;
 *                          void        *arg;
 *                      }   OSL_IM_ISR_LIST_ENTRY;
 *
 *  Element           : isr - interrupt service routine (ISR)
 *                            of type 'OSL_IM_ISR'
 *                      arg - argument given to ISR as input parameter
 *
 *  Remarks           : - Each hardware interrupt has its own list of this
 *                        type.
 * --------------------------------------------------------------------------
 */
typedef struct
{
    OSL_IM_ISR  isr;
    void        *arg;
}   OSL_IM_ISR_LIST_ENTRY;


/* --------------------------------------------------------------------------
 *  Procedure : osl_om_connect_isr
 *
 *  Purpose   : Connects an interrupt service routine (ISR) to a hardware
 *              interrupt.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              OSL_RESULT
 *              osl_im_connect_isr
 *              (
 *                  ENUM16      hw_irq,
 *                  OSL_IM_ISR  isr,
 *                  void        *arg
 *              );
 *
 *  Input     : hw_irq - hardware interrupt identifier
 *              isr    - interrupt service routine (ISR) of type 'OSL_IM_ISR'
 *              arg    - argument given to ISR as input parameter
 *                       'osl_im_connect_isr'
 *
 *  Return    : any OSL_RESULT
 *
 *  Remarks   : - Each connected interrupt service routine has a list entry
 *                of structured type 'OSL_IM_ISR_LIST_ENTRY'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
OSL_RESULT
osl_im_connect_isr
(
    ENUM16      hw_irq,
    OSL_IM_ISR  isr,
    void        *arg
);


#endif /* #ifndef TCN_OSL_H */
