# Makefile for my basic library
#

NAME                 =  Unix Sources
SRCDIR               =  .

.PHONY: all
all: _everything

include $(SCRIPTDIR)/makesource
include $(SCRIPTDIR)/make.states

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

.PHONY: clean
clean:
	@echo " - Cleaning Unix objects..."
	@rm -f $(OBJECTS) $(Y_CPPFILES) $(Y_HEADERS) $(DEPENDS) $(DEPFILES)

