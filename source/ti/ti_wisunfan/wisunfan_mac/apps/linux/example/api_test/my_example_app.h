
#include "log.h"

#define LOG_APIMAC_rxmsg  _bitN(LOG_DBG_APP_FLAG_FIRST + 0)
#define LOG_APIMAC_txmsg  _bitN(LOG_DBG_APP_FLAG_FIRST + 1)
#define LOG_APIMAC_misc   _bitN(LOG_DBG_APP_FLAG_FIRST + 2)
#define LOG_APIMAC_wait   _bitN(LOG_DBG_APP_FLAG_FIRST + 3)
#define LOG_APIMAC_

extern int io_device_type; // letter 's' for socket, or 'u' for uart.
extern int apimac_msg_wait_timeout_mSecs;

void APP_Main(void);

/*
 *  ========================================
 *  Texas Instruments Micro Controller Style
 *  ========================================
 *  Local Variables:
 *  mode: c
 *  c-file-style: "bsd"
 *  tab-width: 4
 *  c-basic-offset: 4
 *  indent-tabs-mode: nil
 *  End:
 *  vim:set  filetype=c tabstop=4 shiftwidth=4 expandtab=true
 */

