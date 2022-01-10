/* include/openthread-config-generic.h.  Generated from openthread-config-generic.h.in by configure.  */
/* include/openthread-config-generic.h.in.  Generated from configure.ac by autoheader.  */

#if OPENTHREAD_MTD
#error Wrong Include file, expected FTD
#endif

#if OPENTHREAD_FTD
/* all is well */
#endif

#define OPENTHREAD_CONFIG_COAP_API_ENABLE 1
#define OPENTHREAD_CONFIG_BORDER_AGENT_ENABLE 0
#define OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE 1
#define OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE 0
#define OPENTHREAD_CONFIG_CHANNEL_MANAGER_ENABLE 0
#define OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE 0
#define OPENTHREAD_CONFIG_CHILD_SUPERVISION_ENABLE 0
#define OPENTHREAD_CONFIG_COMMISSIONER_ENABLE 1
#define OPENTHREAD_CONFIG_IP6_SLAAC_ENABLE 1
#define OPENTHREAD_ENABLE_DHCP6_MULTICAST_SOLICIT 1
#define OPENTHREAD_CONFIG_DHCP6_CLIENT_ENABLE 1
#define OPENTHREAD_CONFIG_DHCP6_SERVER_ENABLE 0
#define OPENTHREAD_CONFIG_DIAG_ENABLE 1
#define OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE 1
#define OPENTHREAD_CONFIG_DTLS_ENABLE 1
#define OPENTHREAD_CONFIG_JAM_DETECTION_ENABLE 0
#define OPENTHREAD_CONFIG_JOINER_ENABLE 1
#define OPENTHREAD_CONFIG_LEGACY_ENABLE 0
#define OPENTHREAD_CONFIG_MAC_FILTER_ENABLE 0
#define OPENTHREAD_CONFIG_TMF_NETWORK_DIAG_MTD_ENABLE 0
#define OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE 0
#define OPENTHREAD_CONFIG_NCP_SPI_ENABLE 0
#define OPENTHREAD_ENABLE_NCP_SPINEL_ENCRYPTER 0
#define OPENTHREAD_CONFIG_NCP_UART_ENABLE 1
#define OPENTHREAD_ENABLE_NCP_VENDOR_HOOK 0
#define OPENTHREAD_CONFIG_LINK_RAW_ENABLE 1
#define OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE 0
#define OPENTHREAD_CONFIG_UDP_FORWARD_ENABLE 0

#define PACKAGE_NAME "TI-OPENTHREAD"
#define PACKAGE_VERSION "1.2.4.0"

#define OPENTHREAD_CONFIG_CLI_MAX_LINE_LENGTH     256

#define ENABLE_ALL_LOGS_TO_UART 0
#if     ENABLE_ALL_LOGS_TO_UART

#define OPENTHREAD_CONFIG_LOG_OUTPUT              OPENTHREAD_CONFIG_LOG_OUTPUT_DEBUG_UART

#define OPENTHREAD_CONFIG_ENABLE_DEBUG_UART       1

#define OPENTHREAD_CONFIG_LOG_LEVEL               OT_LOG_LEVEL_DEBG

#define OPENTHREAD_CONFIG_LOG_API                 1
#define OPENTHREAD_CONFIG_LOG_ARP                 1
#define OPENTHREAD_CONFIG_LOG_CLI                 1
#define OPENTHREAD_CONFIG_LOG_COAP                1
#define OPENTHREAD_CONFIG_LOG_ICMP                1
#define OPENTHREAD_CONFIG_LOG_IP6                 1
#define OPENTHREAD_CONFIG_LOG_MAC                 1
#define OPENTHREAD_CONFIG_LOG_MEM                 1
#define OPENTHREAD_CONFIG_LOG_MLE                 1
#define OPENTHREAD_CONFIG_LOG_NETDATA             1
#define OPENTHREAD_CONFIG_LOG_NETDIAG             1
#define OPENTHREAD_CONFIG_LOG_PKT_DUMP            1
#define OPENTHREAD_CONFIG_LOG_PLATFORM            1
#define OPENTHREAD_CONFIG_LOG_PREPEND_LEVEL       1
#define OPENTHREAD_CONFIG_LOG_PREPEND_REGION      1

#endif
