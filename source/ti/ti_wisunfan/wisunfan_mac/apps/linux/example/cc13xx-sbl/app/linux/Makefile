#############################################################
# @file Makefile
#
# @brief TIMAC 2.0 Collector Example Application Makefile
#
# Group: WCS LPC
# $Target Device: DEVICES $
#
#############################################################
# $License: BSD3 2016 $
#############################################################
# $Release Name: PACKAGE NAME $
# $Release Date: PACKAGE RELEASE DATE $
#############################################################

_default: _app

COMPONENTS_HOME=../../components
SDK_HOME=../../../

HERE=$(shell pwd)
CFLAGS += -std=gnu99
# Unconnemt to enable two wire UART interface
#CFLAGS += -DSBL_TWO_WIRE
CFLAGS += -I.
CFLAGS += -I$(PROJ_DIR)../../platform/linux 
CFLAGS += -I$(PROJ_DIR)../../cc13xxdnld

include ../../../../scripts/front_matter.mak

APP_NAME = cc13xx-sbl

C_SOURCES += main.c
C_SOURCES += cc13xxdnld.c
C_SOURCES += sblUart.c

include ../../../../scripts/app.mak

#  ========================================
#  Texas Instruments Micro Controller Style
#  ========================================
#  Local Variables:
#  mode: makefile-gmake
#  End:
#  vim:set  filetype=make
