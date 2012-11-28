#
#

NAME                 =  basic-tests
VERS_MAJOR           =  0
VERS_MINOR           =  1

# ---------------------------------------------

OBJECTS_AND_LIBS     =  $(OBJECTS) $(PROJECT_ROOT)/bin/Basic.a

export CXXFLAGS     +=  -DSYS_DEBUG_ON=1 -I$(PROJECT_ROOT)/tests/include -I$(PROJECT_ROOT)/opsys/unix
export LFLAGS       +=  $(MY_PROJECT_LIBS)

.PHONY: all
all:
	@echo "Don't use this make directly, call it from the root of this project."
	@exit 1

include $(SCRIPTDIR)/makesource
include $(SCRIPTDIR)/make.states

.PHONY: _baselib
_baselib:
	@(cd ../ && make all)

.PHONY: test
test: _baselib _everything
	@echo "Running test:"
	@$(BINDIR)/$(NAME)

.PHONY: clean
clean:
	@echo " - Cleanincs tests..."
	@rm -f $(OBJECTS) $(BINDIR)/$(NAME) $(DEPENDS) $(DEPFILES)

.PHONY: $(BINDIR)
$(BINDIR):
	@test -d "$@" || mkdir "$@"

do_link: $(BINDIR) $(BINDIR)/$(NAME)

$(BINDIR)/$(NAME): $(OBJECTS_AND_LIBS)
	@echo " o Linking executable '$(NAME)'..."
	@$(CXX) -o "$@" $(OBJECTS_AND_LIBS) $(LFLAGS)

