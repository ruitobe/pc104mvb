/* ==========================================================================
 *
 *  File      : MUE_CAL.C
 *
 *  Purpose   : MVB UART Emulation - MUE Calibration Application
 *
 *  Project   : General TCN Driver Software
 *              - MVB UART Emulation Protocol (d-000206-nnnnnn)
 *              - TCN Software Architecture   (d-000487-nnnnnn)
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
 *  - MUE_CALIBRATE   - enable MUE calibration application
 *
 * ==========================================================================
 */


/* ==========================================================================
 *
 *  Project specific Definitions used for Conditional Compiling
 *
 * ==========================================================================
 */
 
 #include "tcn_common.h"
#ifdef TCN_PRJ
#   include <tcn_prj.h>
#endif




/* ==========================================================================
 *
 *  Conditional Compiling
 *
 * ==========================================================================
 */
#ifdef MUE_CALIBRATE

#define MUE_OSL_PRINTF   printf
/* ==========================================================================
 *
 *  Include Files
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  TCN Software Architecture
 * --------------------------------------------------------------------------
 */
#include <tcn_def.h>

/* --------------------------------------------------------------------------
 *  MVB UART Emulation
 * --------------------------------------------------------------------------
 */
#include <mue_def.h>
#include <mue_bc.h>
#include <mue_osl.h>
#include <mue_acc.h>


/* ==========================================================================
 *
 *  Local Variables
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Bus Controller Structure
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC MUE_BUS_CTRL    mue_bus_ctrl;
TCN_DECL_LOCAL  void            *p_bus_ctrl = (void*)&mue_bus_ctrl;


/* ==========================================================================
 *
 *  Procedures
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : mue_calibrate
 *
 *  Remarks   : How to calculate actual value for 'acc_reg_timeout'?
 *
 *              NOTE: This is an interative process.
 *
 *              (1) set value for 'cnt_for_1sec_timeout' (see examples)
 *
 *              (2) run application 'mue_cal'
 *                  It will repetitive make the following printout:
 *                  ...
 *                  cnt_for_1sec_timeout=670000
 *                  0123456789
 *                  **********
 *                  ...
 *
 *              (3) measure time required for a printout
 *
 *              (4) calculate actual value for 'cnt_for_1sec_timeout':
 *                  x = ((cnt_for_1sec_timeout / measured_time) * 10)
 *
 *                  Example:
 *                  x = ((650000 / 9.7sec) * 10sec) = 670103 => ~670000
 *
 *              (5) calculate value for 'acc_reg_timeout':
 *                  NOTE:
 *                  - ~10ms  for MD16/MDFULL
 *                  - ~100us for PD16
 *
 *                  Example for D113L-MDFULL:
 *                  acc_reg_timeout = (670000 / 100)   => 6700
 *
 *                  Example for D114L-PD16:
 *                  acc_reg_timeout = (670000 / 10000) => 67
 *
 * --------------------------------------------------------------------------
 */
UNSIGNED32 mue_calibrate (void)
{
    UNSIGNED16  loop;
    UNSIGNED16  counter;
    UNSIGNED32  cnt_for_1sec_timeout;


    MUE_OSL_PRINTF("\n");
    MUE_OSL_PRINTF("Calibration of MVB UART Emulation (MUE)\n");
    MUE_OSL_PRINTF("=======================================\n");


    MUE_OSL_PRINTF("- mue_acc_init()\n\n");
    mue_acc_init(p_bus_ctrl);


    /* ----------------------------------------------------------------------
     *  Example 1:
     *  ----------
     *  - DUT : AT91-CONTROLLER with MDFULL
     *  - HOST: AT91-CONTROLLER (ARM7 with 24MHz)
     *  - OS  : eCos (ext. SRAM), NOTE: GNU GCC option '-O2'
     *
     *  cnt_for_1sec_timeout: ~281250
     * ----------------------------------------------------------------------
     *  Example 2:
     *  ----------
     *  - DUT : AT91-CONTROLLER with MDFULL
     *  - HOST: AT91-CONTROLLER (ARM7 with 24MHz)
     *  - OS  : eCos (int. SRAM), NOTE: GNU GCC option '-O2'
     *
     *  cnt_for_1sec_timeout: ~1004464
     * ----------------------------------------------------------------------
     *  Example 3:
     *  ----------
     *  - DUT : MDFULL
     *  - HOST: PC AMD K6-2 500MHz
     *  - OS  : eCos, NOTE: GNU GCC option '-O2'
     *
     *  cnt_for_1sec_timeout: ~670000
     * ----------------------------------------------------------------------
     */
    cnt_for_1sec_timeout = 1000000;


    for(;;)
    {
        /* alternative:                                                */
        /* scanf("cnt_for_1sec_timeout=%ld\n", &cnt_for_1sec_timeout); */

        MUE_OSL_PRINTF("cnt_for_1sec_timeout=%ld\n", cnt_for_1sec_timeout);

        for (loop=0; loop<3; loop++)
        {
            for (counter=0; counter<10; counter++)
            {
                MUE_OSL_PRINTF("%1d", counter);
            } /* for (counter=0; counter<10; counter++) */
            MUE_OSL_PRINTF("\n");

            for (counter=0; counter<10; counter++)
            {
                MUE_OSL_PRINTF("*");
                mue_acc_timeout_calibrate(p_bus_ctrl, cnt_for_1sec_timeout);
            } /* for (counter=0; counter<10; counter++) */
            MUE_OSL_PRINTF("\n");

        } /* for (loop=0; loop<3; loop++) */

        MUE_OSL_PRINTF("\n");

    } /* for(;;) */


    return(0);

} /* mue_calibrate */

/* --------------------------------------------------------------------------
 *  Procedure : main
 * --------------------------------------------------------------------------
 */
//int
//main (void)
//{
 //   UNSIGNED32  result;

 //   result = mue_calibrate();
 //   if (result != 0)
 //   {
 //       MUE_OSL_PRINTF("ERROR: result=%ld", result);
 //       MUE_OSL_PRINTF("\n");
 //   } /* if (result != 0) */

 //   TCN_MACRO_TERMINATE_MAIN(result);

 //   return((int)result);

//} /* main */


#endif /* #ifdef MUE_CALIBRATE */
