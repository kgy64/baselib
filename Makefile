# Makefile for my basic library
#

DOXYGEN                 =   $(shell which doxygen)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

.PHONY: all
all:
	$(SILENT_MODE)cd src && $(MAKE)

.PHONY: test
test:
	$(SILENT_MODE)cd tests && $(MAKE) test)

.PHONY: clean
clean:
	$(SILENT_MODE)(cd src && $(MAKE) clean)
	$(SILENT_MODE)(cd opsys/$(OPERATING_SYSTEM) && $(MAKE) clean)
	$(SILENT_MODE)(cd tests && $(MAKE) clean)
	$(SILENT_MODE)rm -rf doc

.PHONY: doc
doc:
	$(SILENT_MODE)$(DOXYGEN) Doxyfile

