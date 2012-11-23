# Makefile for my basic library
#

NAME                 =  basic
VERS_MAJOR           =  0
VERS_MINOR           =  1

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

.PHONY: all
all:
	@(cd opsys/unix && $(MAKE) -s)
	@(cd src && $(MAKE) -s)

.PHONY: test
test:
	@cd tests && $(MAKE) -s test

.PHONY: clean
clean:
	@(cd src && $(MAKE) -s clean)
	@(cd opsys/unix && $(MAKE) -s clean)
	@(cd tests && $(MAKE) -s clean)
