# Makefile for my basic library
#

NAME                 =  basic
VERS_MAJOR           =  0
VERS_MINOR           =  1

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

export BASE_LIBRARIES    =  glibmm-2.4

include scripts/make.flags

.PHONY: all
all:
	@(cd src && $(MAKE) -s)

.PHONY: test
test:
	@(cd tests && $(MAKE) -s test)

.PHONY: clean
clean:
	@(cd src && $(MAKE) -s clean)
	@(cd opsys/unix && $(MAKE) -s clean)
	@(cd tests && $(MAKE) -s clean)
	@rm -rf doc

.PHONY: doc
doc:
	@(cd src && $(MAKE) -s doc)

