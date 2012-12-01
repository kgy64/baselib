# Makefile for my basic library
#

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

