#
#

NAME                 =  basic-tests
VERS_MAJOR           =  0
VERS_MINOR           =  1

# ---------------------------------------------

BINDIR               =  $(PWD)/bin

OBJECTS_AND_LIBS     =  $(OBJECTS) ../bin/Basic.a

.PHONY: all
all:
	@echo "Don't use this make directly, call it from the root of this project."
	@exit 1

include $(SCRIPTDIR)/makesource
include $(SCRIPTDIR)/make.states

export CXXFLAGS     +=  -DSYS_DEBUG_ON=1
export LFLAGS       +=  $(MY_PROJECT_LIBS)

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

do_link: $(BINDIR) $(BINDIR)/$(NAME)

$(BINDIR)/$(NAME): $(OBJECTS_AND_LIBS)
	@echo " o Linking executable '$(NAME)'..."
	@$(CXX) -o "$@" $(OBJECTS_AND_LIBS) $(LFLAGS)

