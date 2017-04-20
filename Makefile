

VERSION ?= \"0.0.0\"

BINDIR ?= bin
executable ?= $(BINDIR)/main

MF_DEBUG ?= @

MACROS ?=\
-DVERSION=$(VERSION)

CFLAGS_CLIBS = -I$(PWD)/clibs/include
LDFLAGS_CLIBS = -L$(PWD)/clibs/lib
CFLAGS_libeigen = -I$(PWD)/clibs/include/eigen3/
LDFLAGS_libint = -lint2
CFLAGS_libint = -I$(PWD)/clibs/include/libint2

INCLUDE_FLAGS= \
-I$(PWD) \
$(CFLAGS_CLIBS) \
$(LDFLAGS_CLIBS) \
$(CFLAGS_libeigen) \
$(CFLAGS_CLIBS) \

CXXFLAGS = \
$(MACROS) \
$(INCLUDE_FLAGS) \

CFLAGS = \
$(CXXFLAGS)

LD_FLAGS = \
$(LDFLAGS_CLIBS) \
$(LDFLAGS_libint) \

CFILES = \
clibs/inih/ini.c \

CXXFILES = \
src/main.cxx \
clibs/inih/cpp/INIReader.cpp \

CLEAN_FILES = \
$(wildcard $(OBJFILES)) \
$(wildcard $(DEPFILES)) \
$(wildcard $(executable)) \
$(wildcard $(BINDIR)) \

SOURCES = $(CFILES) $(CXXFILES)

OBJFILES = \
$(shell echo $(SOURCES) | sed "s/\.\S*/.o/g") \

DEPFILES = \
$(shell echo $(SOURCES) | sed "s/\.\S*/.d/g") \

-include $(DEPFILES)


CXX = g++ -std=c++11
CC = cc

.DEFAULT_GOAL := $(executable)
all: $(executable)

objs: $(OBJFILES) ## Make object files
deps: $(DEPFILES) ## Make dependencies

$(executable): $(OBJFILES)
	@echo [BIN] $(@)
	$(MF_DEBUG)mkdir -p $(BINDIR)
	$(MF_DEBUG)$(CXX) $(OBJFILES) -o $@ $(CXXFLAGS) $(LD_FLAGS)

# DEPENDENCY FILE CREATION
%.d: %.cxx
	@echo [DEP] $(@)
	$(MF_DEBUG)$(CXX) $(CXXFLAGS) -M -MF $@ $<

%.d: %.cpp
	@echo [DEP] $(@)
	$(MF_DEBUG)$(CXX) $(CXXFLAGS) -M -MF $@ $<

%.d: %.c
	@echo [DEP] $(@)
	$(MF_DEBUG)$(CC) $(CFLAGS) -M -MF $@ $<

# OBJECT FILE CREATION
%.o: %.cxx
	@echo [CXX] $(@)
	$(MF_DEBUG)$(CXX) $(CXXFLAGS) -c $< -o $*.o

%.o: %.cpp
	@echo [CXX] $(@)
	$(MF_DEBUG)$(CXX) $(CXXFLAGS) -c $< -o $*.o

%.o: %.c
	@echo "[CC] " $(@)
	$(MF_DEBUG)$(CC) $(CFLAGS) -c $< -o $*.o

clean: ## Clean main files
	@echo Cleaning up...
	$(MF_DEBUG)for i in $(CLEAN_FILES); do echo "[RM] $$i"; rm -rf $$i; done

tags: $(SOURCES) ## Create tags
	ctags -R --c++-kinds=+p --fields=+iaS --extra=+q $(SOURCES) $(subst -I,,$(INCLUDE_FLAGS))

FORCE:
print-%: ; @echo $*=$($*)
