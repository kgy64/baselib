# Makefile for my basic library
#

NAME                 =  base
VERS_MAJOR           =  1
VERS_MINOR           =  1

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

SRCDIR               =  . ../generic

ifneq ($(OPERATING_SYSTEM), )
SRCDIR              +=  ../opsys/$(OPERATING_SYSTEM)
endif

include $(SCRIPTDIR)/makelib

export LFLAGS       +=  $(MY_PROJECT_LIBS)

