
#include "log.h"

#define LOG_DBG_APP_SOCKET  _bitN(LOG_DBG_APP_FLAG_FIRST + 0)
#define LOG_DBG_APP_UART    _bitN(LOG_DBG_APP_FLAG_FIRST + 1)

extern struct uart_cfg my_uart_cfg;
extern struct socket_cfg my_socket_cfg;

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

