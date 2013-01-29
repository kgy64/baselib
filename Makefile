# Makefile for my basic library
#

.PHONY: all
all:
	@(cd src && $(MAKE) -s)

.PHONY: test
test:
	@(cd tests && $(MAKE) -s test)

.PHONY: clean
clean:
	(cd src && $(MAKE) -s clean)
	(cd opsys/unix && $(MAKE) -s clean)
	(cd tests && $(MAKE) -s clean)
	rm -rf doc

.PHONY: doc
doc:
	@doxygen Doxyfile

