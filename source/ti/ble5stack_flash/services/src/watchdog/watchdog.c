/******************************************************************************

 @file  watchdog.c

 @brief Watchdog driver interface

 Group: WCS, LPC, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2015 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/
/*
 *  ======== Watchdog.c ========
 */

#include <stdint.h>
#include <stdlib.h>

#include <ti/drivers/Watchdog.h>

/* Externs */
extern const Watchdog_Config Watchdog_config[];

/* Also used to check status for initialization */
static int Watchdog_count = -1;

/* Default Watchdog parameters structure */
const Watchdog_Params Watchdog_defaultParams = {
    NULL,                    /* callbackFxn */
    Watchdog_RESET_ON,       /* resetMode */
    Watchdog_DEBUG_STALL_ON, /* debugStallMode */
    NULL                     /* custom */
};

/*
 *  ======== Watchdog_clear ========
 */
void Watchdog_clear(Watchdog_Handle handle)
{
    handle->fxnTablePtr->watchdogClear(handle);
}

/*
 *  ======== Watchdog_close ========
 */
void Watchdog_close(Watchdog_Handle handle)
{
    handle->fxnTablePtr->watchdogClose(handle);
}

/*
 *  ======== Watchdog_control ========
 */
int Watchdog_control(Watchdog_Handle handle, unsigned int cmd, void *arg)
{
    return (handle->fxnTablePtr->watchdogControl(handle, cmd, arg));
}

/*
 *  ======== Watchdog_init ========
 */
void Watchdog_init(void)
{
    /* Prevent multiple calls for Watchdog_init */
    if (Watchdog_count >= 0) {
        return;
    }

    /* Call each driver's init function */
    for (Watchdog_count = 0;
            Watchdog_config[Watchdog_count].fxnTablePtr != NULL;
            Watchdog_count++) {
        Watchdog_config[Watchdog_count].fxnTablePtr->watchdogInit(
                (Watchdog_Handle)&(Watchdog_config[Watchdog_count]));
    }
}

/*
 *  ======== Watchdog_open ========
 */
Watchdog_Handle Watchdog_open(unsigned int index, Watchdog_Params *params)
{
    Watchdog_Handle handle;

    handle = (Watchdog_Handle)&(Watchdog_config[index]);
    return (handle->fxnTablePtr->watchdogOpen(handle, params));
}

/*
 *  ======== Watchdog_Params_init ========
 */
void Watchdog_Params_init(Watchdog_Params *params)
{
    *params = Watchdog_defaultParams;
}


/*
 *  ======== Watchdog_setReload ========
 */
void Watchdog_setReload(Watchdog_Handle handle, uint32_t value)
{
    handle->fxnTablePtr->watchdogSetReload(handle, value);
}
