_default: _app

include ../../scripts/front_matter.mak

APP_NAME=example_test

CFLAGS += -I../../components/common/inc

C_SOURCES = 
#C_SOURCES += cllc.c
#C_SOURCES += collector.c
#C_SOURCES += csf_linux.c
C_SOURCES += linux_main.c
C_SOURCES += app_main.c

APP_LIBS    = libcommon.a
APP_LIBDIRS += ../../components/common/${OBJDIR}


include ../../scripts/app.mak

#  ========================================
#  Texas Instruments Micro Controller Style
#  ========================================
#  Local Variables:
#  mode: makefile-gmake
#  End:
#  vim:set  filetype=make


