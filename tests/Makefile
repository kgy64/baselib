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
	@echo "Don't use this make directly, call it from the root of this project."
	@exit 1

-include $(SCRIPTDIR)/makesource
-include $(SCRIPTDIR)/make.states

export CXXFLAGS     +=  -DSYS_DEBUG_ON=1 -I$(PROJECT_ROOT)/tests/include -I$(PROJECT_ROOT)/opsys/unix
export LFLAGS       +=  $(MY_PROJECT_LIBS)

$(MY_BASIC_LIB): _basic

.PHONY: _basic
_basic:
	@(cd ../ && $(MAKE) all)

.PHONY: test
test: _everything
	@echo "Running test:"
	@$(BINDIR)/$(NAME)

.PHONY: clean
clean:
	@echo " - Cleanincs tests..."
	@rm -f $(OBJECTS) $(BINDIR)/$(NAME) $(DEPENDS)

.PHONY: $(BINDIR)
$(BINDIR):
	@test -d "$@" || mkdir "$@"

$(BINDIR)/$(NAME): $(BINDIR) $(OBJECTS_AND_LIBS)
	@echo " o Linking executable '$(NAME)'..."
	@$(CXX) -o "$@" $(OBJECTS_AND_LIBS) $(LFLAGS)

_everything: $(BINDIR)/$(NAME)

