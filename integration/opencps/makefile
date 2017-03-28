LEVEL=../../
include ../../compiler.cfg

OPENCPS_HOME:=/apps/opencps/opencps-time-3.3-2017-01-23
OCPS_INC=-I$(OPENCPS_HOME)/thirdparty/include -I$(OPENCPS_HOME)/src/bluefin -I$(OPENCPS_HOME)/src/engine

BIN=$(LIB_DIR)/libmytools.so
OBJDIR = obj
DEP=depend
SOURCES:=$(wildcard *.cc)
OBJECTS:=$(patsubst %.cc, $(OBJDIR)/%.o, $(SOURCES))
DEPENDS:=$(patsubst %.cc, %.dep, $(SOURCES))

CURR_OBJECTS:=$(filter $(wildcard $(OBJDIR)/*.o), $(OBJECTS))
CURR_DEP:=$(wildcard $(DEP))

%.dep: %.cc
	g++ $(CXXSTANDARD) -MM $< -MT $(patsubst %.cc, $(OBJDIR)/%.o, $<) $(CXXINC_FOLDER) $(OCPS_INC) -o $@ 

$(OBJDIR)/%.o: %.cc
	$(CXX) $(LIB_FLAG) $(CXXFLAGS) $(OCPS_INC) -c $< -o $@

.PHONY: all
all: $(DEP) $(OBJECTS) $(BIN)

$(BIN): $(OBJECTS)
	$(CXX) -o $@ $(IMELD_FLAG) -shared $(CXXFLAGS) $(LDFLAGS) $(OBJECTS)

$(DEP): $(DEPENDS)
	cat *.dep > $(DEP)

ifeq "$(CURR_DEP)" "$(DEP)"
-include $(DEP)
endif

.PHONY: clean
clean:
	rm -f $(CURR_OBJECTS) $(OBJDIR)/*.optrpt *.dep $(CURR_DEP) $(BIN)

