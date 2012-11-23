# Makefile for my basic library
#

NAME                 =  Basic
VERS_MAJOR           =  0
VERS_MINOR           =  1

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

SRCDIR               =  .
BINDIR               =  bin

export SUBPROJECT_PATH = $(PWD)/..

.PHONY: all
all: _everything

include $(SCRIPTDIR)/makelib

export LFLAGS       +=  $(MY_CLUTTER_LIBS)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

.PHONY: clean
clean:
	@echo " - Cleaning basic library..."
	@rm -f $(BINDIR)/* $(OBJECTS) $(Y_CPPFILES) $(Y_HEADERS) $(DEPENDS) $(DEPFILES)

