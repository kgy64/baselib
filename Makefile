# Makefile for my basic library
#

NAME                 =  basic
VERS_MAJOR           =  0
VERS_MINOR           =  1

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

export BASE_LIBRARIES    =  glibmm-2.4
export PROJECT_ROOT     ?=  $(PWD)
export SCRIPTDIR        ?=  $(PROJECT_ROOT)/scripts
export CXXFLAGS         += -I$(PROJECT_ROOT)/tests -I$(PROJECT_ROOT)/opsys/unix

include $(SCRIPTDIR)/make.flags

.PHONY: all
all:
	@(cd opsys/unix && $(MAKE) -s)
	@(cd src && $(MAKE) -s)

.PHONY: test
test: all
	@cd tests && $(MAKE) -s test

.PHONY: clean
clean:
	@(cd src && $(MAKE) -s clean)
	@(cd opsys/unix && $(MAKE) -s clean)
	@(cd tests && $(MAKE) -s clean)

.PHONY: doc
doc:
	@(cd src && $(MAKE) -s doc)

