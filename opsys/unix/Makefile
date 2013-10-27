# Makefile for my basic library
#

export NAME                 =  Unix-specific
export SRCDIR              ?=  .

.PHONY: all
all: _everything

include $(SCRIPTDIR)/makesource
include $(SCRIPTDIR)/make.states

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

.PHONY: clean
clean:
	@echo " - Cleaning $(NAME) objects..."
	@rm -f $(OBJECTS) $(Y_CPPFILES) $(Y_HEADERS) $(DEPENDS)

