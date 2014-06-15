#
#

NAME                 =  basic-tests
VERS_MAJOR           =  0
VERS_MINOR           =  1

# ---------------------------------------------

MY_BASIC_LIB         =  $(PROJECT_ROOT)/bin/Basic.a
OBJECTS_AND_LIBS     =  $(OBJECTS) $(MY_BASIC_LIB) $(MY_PROJECT_LIBS)

export BASE_LIBRARIES    =

.PHONY: all
all:
	$(SILENT_MODE)echo "Don't use this make directly, call it from the root of this project."
	$(SILENT_MODE)exit 1

-include $(SCRIPTDIR)/makesource

export CXXFLAGS     +=  -DSYS_DEBUG_ON=1 -I$(PROJECT_ROOT)/tests/include -I$(PROJECT_ROOT)/opsys/unix
export LFLAGS       +=  $(MY_PROJECT_LIBS)

$(MY_BASIC_LIB): _basic

.PHONY: _basic
_basic:
	$(SILENT_MODE)(cd ../ && $(MAKE) all)

.PHONY: test
test: _everything
	$(SILENT_MODE)echo "Running test:"
	$(SILENT_MODE)$(BINDIR)/$(NAME)

.PHONY: clean
clean:
	$(SILENT_MODE)echo " - Cleaning $(NAME)..."
	$(SILENT_MODE)rm -f $(OBJECTS) $(BINDIR)/$(NAME) $(DEPENDS)

.PHONY: $(BINDIR)
$(BINDIR):
	$(SILENT_MODE)test -d "$@" || mkdir "$@"

$(BINDIR)/$(NAME): $(BINDIR) $(OBJECTS_AND_LIBS)
	$(SILENT_MODE)echo " o Linking executable '$(NAME)'..."
	$(SILENT_MODE)$(CXX) -o "$@" $(OBJECTS_AND_LIBS) $(LFLAGS)

_everything: $(BINDIR)/$(NAME)

