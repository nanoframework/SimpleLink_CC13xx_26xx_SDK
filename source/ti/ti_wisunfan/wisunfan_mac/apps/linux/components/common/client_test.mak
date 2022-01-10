_default: _app

include ../../scripts/front_matter.mak


APP_NAME=client_test

C_SOURCES = src/client_test.c

APP_LIBS    = libcommon.a
APP_LIBDIRS += ${OBJDIR} 

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


