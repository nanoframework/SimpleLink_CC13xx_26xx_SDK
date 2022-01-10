_default: _app

CFLAGS += -I../common/inc

include ../../scripts/front_matter.mak

APP_NAME=mt_test

C_SOURCES = src/mt_test.c

APP_LIBS    =  libapimac.a  libcommon.a
APP_LIBDIRS += ${OBJDIR} ../common/${OBJDIR}

# In this case, we want to build the library by typing "make"
PRE_LINK_CMD=${MAKE} 

include ../../scripts/app.mak

#  ========================================
#  Texas Instruments Micro Controller Style
#  ========================================
#  Local Variables:
#  mode: makefile-gmake
#  End:
#  vim:set  filetype=make


