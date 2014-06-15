# Makefile for my basic library
#

export NAME                 =  Unix-specific
export SRCDIR              ?=  .

.PHONY: all
all: _everything

include $(SCRIPTDIR)/makesource

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

.PHONY: clean
clean:
	$(SILENT_MODE)echo " - Cleaning $(NAME) objects..."
	$(SILENT_MODE)rm -f $(OBJECTS) $(Y_CPPFILES) $(Y_HEADERS) $(DEPENDS)

