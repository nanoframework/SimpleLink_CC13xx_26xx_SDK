###############################################################################
# @file Makefile
#
# @brief TIMAC 2.0 Multi-page NV (nonvolatile) Library Unit-test Makefile
#
# Group: CMCU LPC
# $Target Device: DEVICES $
#
###############################################################################
# $License: BSD3 2019 $
###############################################################################
# $Release Name: PACKAGE NAME $
# $Release Date: PACKAGE RELEASE DATE $
###############################################################################

_default: _app

CFLAGS += -I../common/inc
CFLAGS += -DNV_LINUX
CFLAGS += -DNVOCMP_POSIX_MUTEX
CFLAGS += -DNVOCMP_NVPAGES=4

include ../../scripts/front_matter.mak

APP_NAME = nv_test

C_SOURCES = src/nv_test.c

# Build the local library here before we link
PRE_LINK_CMD=${MAKE} 

APP_LIBS    += libnv.a
APP_LIBS    += libcommon.a

APP_LIBDIRS += ../common/${OBJDIR}
APP_LIBDIRS += ${OBJDIR}

include ../../scripts/app.mak

#  ========================================
#  Texas Instruments Micro Controller Style
#  ========================================
#  Local Variables:
#  mode: makefile-gmake
#  End:
#  vim:set  filetype=make


