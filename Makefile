# Makefile for my basic library
#

NAME                 =  basic
VERS_MAJOR           =  0
VERS_MINOR           =  1

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

.PHONY: all
all:
	@cd src && $(MAKE) -s

.PHONY: test
test:
	@cd tests && $(MAKE) -s test

.PHONY: clean
clean:
	@(cd src && $(MAKE) -s clean)
	@(cd tests && $(MAKE) -s clean)
