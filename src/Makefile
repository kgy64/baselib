# Makefile for my basic library
#

NAME                 =  Basic
VERS_MAJOR           =  0
VERS_MINOR           =  1

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

export SRCDIR        =  . ../opsys/unix

.PHONY: all
all: _everything

include $(SCRIPTDIR)/makelib

export LFLAGS       +=  $(MY_PROJECT_LIBS)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

.PHONY: clean
clean:
	@echo " - Cleaning basic library..."
	@rm -f $(BINDIR)/* $(OBJECTS) $(Y_CPPFILES) $(Y_HEADERS) $(DEPENDS)

