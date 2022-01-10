_default: _app

include ../../scripts/front_matter.mak

APP_NAME=rand_data_test

C_SOURCES = src/rand_data_test.c

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


