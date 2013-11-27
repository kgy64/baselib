# Makefile for my basic library
#

DOXYGEN                 =   $(shell which doxygen)

# The operating system in use. Default value is given here.
OPERATING_SYSTEM       ?=   unix

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

.PHONY: all
all:
	@(cd src && $(MAKE) -s)

.PHONY: test
test:
	@(cd tests && $(MAKE) -s test)

.PHONY: clean
clean:
	(cd src && $(MAKE) -s clean)
	(cd opsys/$(OPERATING_SYSTEM) && $(MAKE) -s clean)
	(cd tests && $(MAKE) -s clean)
	rm -rf doc

.PHONY: doc
doc:
	@$(DOXYGEN) Doxyfile

