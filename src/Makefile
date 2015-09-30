# Makefile for my basic library
#

NAME                 =  base
VERS_MAJOR           =  1
VERS_MINOR           =  1

OPERATING_SYSTEM        ?=  __unknown_opsys__

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

export SRCDIR        =  . ../opsys/$(OPERATING_SYSTEM)

.PHONY: all
all: _everything

include $(SCRIPTDIR)/makelib

export LFLAGS       +=  $(MY_PROJECT_LIBS)

YACC_PREFIX          =  yy

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

.PHONY: clean
clean:
	@echo " - Cleaning basic library..."
	@rm -rf $(BINDIR)
	@rm -f $(MY_BASELIB)/include/public $(OBJECTS) $(Y_CPPFILES) $(Y_HEADERS) $(DEPENDS)

