_default: _app

include ../../scripts/front_matter.mak

APP_NAME=example_app

COMMON_HOME=../../components/common

CFLAGS += -I${COMMON_HOME}/inc

C_SOURCES = 
C_SOURCES += linux_main.c
C_SOURCES += app_main.c

APP_LIBS    = libcommon.a
APP_LIBDIRS += ${COMMON_HOME}/${OBJDIR}


include ../../scripts/app.mak

#  ========================================
#  Texas Instruments Micro Controller Style
#  ========================================
#  Local Variables:
#  mode: makefile-gmake
#  End:
#  vim:set  filetype=make


