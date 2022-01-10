_default: _app

include ../../scripts/front_matter.mak

APP_NAME=fifo_test

C_SOURCES = src/fifo_test.c

APP_LIBS    = libcommon.a
APP_LIBDIRS += ${OBJDIR} 

# Build the local library here before we link
PRE_LINK_CMD=${MAKE} 

include ../../scripts/app.mak

#  ========================================
#  Texas Instruments Micro Controller Style
#  ========================================
#  Local Variables:
#  mode: makefile-gmake
#  End:
#  vim:set  filetype=make


