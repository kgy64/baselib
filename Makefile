# Makefile for my basic library
#

NAME                =   basic
VERS_MAJOR          =   0
VERS_MINOR          =   1

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

BINDIR              =   ./bin
BASE_LIBRARIES      =   clutter-1.0 glibmm-2.4

SRCDIR              =   ./src ./opsys/unix

.PHONY: all
all: _everything

export SCRIPTDIR    =   $(PWD)/scripts

include $(SCRIPTDIR)/makelib

CXXFLAGS            +=  $(EXTRA_CXXFLAGS)
CXXFLAGS            +=  -fPIC -I$(PWD)/opsys/unix -I$(PWD)/include $(MY_CLUTTER_FLAGS)
LFLAGS              +=  $(MY_CLUTTER_LIBS)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

.PHONY: clean
clean:
	@echo " - Cleaning basic library..."
	@rm -f $(BINDIR)/* $(OBJECTS) $(Y_CPPFILES) $(Y_HEADERS) $(DEPENDS) $(DEPFILES)
	@cd tests && $(MAKE) -s clean

.PHONY: test
test:
	@cd tests && $(MAKE) -s test

.PHONY: doc
doc:
	@doxygen doxyfile

