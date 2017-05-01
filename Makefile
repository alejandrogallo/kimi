
-include config.mk

VERSION ?= \"0.0.0\"

CXX ?= g++
CC ?= cc

BINDIR ?= bin
SRCDIR ?= src
EXECUTABLE ?= $(BINDIR)/main

MF_DEBUG ?= @

MACROS ?=\
-DVERSION=$(VERSION)

CLEAN_FILES = \
$(wildcard $(OBJFILES)) \
$(wildcard $(DEPFILES)) \
$(wildcard $(EXECUTABLE)) \
$(wildcard $(BINDIR)) \

SOURCES = $(CFILES) $(CXXFILES)

OBJFILES = \
$(shell echo $(SOURCES) | sed "s/\.\S*/.o/g") \

DEPFILES = \
$(shell echo $(SOURCES) | sed "s/\.\S*/.d/g") \

ifneq ($(MAKECMDGOALS), clean)
-include $(DEPFILES)
endif

CXX_EXTENSIONS = cpp C cc cxx CC
define cxx_obj_compile_rule
%.o: %.$(1)
	@echo [CXX] $$(@)
	$$(MF_DEBUG)$$(CXX) $$(CXXFLAGS) -c $$< -o $$*.o
endef
define cxx_dep_compile_rule
%.d: %.$(1)
	@echo [DEP] $$(@)
	$$(MF_DEBUG)$$(CXX) $$(CXXFLAGS) -M -MF $$@ $$<
endef
$(foreach EXT,$(CXX_EXTENSIONS),$(eval $(call cxx_dep_compile_rule,$(EXT))))
$(foreach EXT,$(CXX_EXTENSIONS),$(eval $(call cxx_obj_compile_rule,$(EXT))))

C_EXTENSIONS = c
define c_obj_compile_rule
%.o: %.$(1)
	@echo [CC] $$(@)
	$$(MF_DEBUG)$$(CC) $$(CFLAGS) -c $$< -o $$*.o
endef
define c_dep_compile_rule
%.d: %.$(1)
	@echo [DEP] $$(@)
	$$(MF_DEBUG)$$(CC) $$(CFLAGS) -M -MF $$@ $$<
endef
$(foreach EXT,$(C_EXTENSIONS),$(eval $(call c_dep_compile_rule,$(EXT))))
$(foreach EXT,$(C_EXTENSIONS),$(eval $(call c_obj_compile_rule,$(EXT))))

.DEFAULT_GOAL := $(EXECUTABLE)
all: $(EXECUTABLE)

objs: $(OBJFILES) ## Make object files
deps: $(DEPFILES) ## Make dependencies

$(EXECUTABLE): $(OBJFILES)
	@echo [BIN] $(@)
	$(MF_DEBUG)mkdir -p $(BINDIR)
	$(MF_DEBUG)$(CXX) $(OBJFILES) -o $@ $(CXXFLAGS) $(LD_FLAGS)

clean: ## Clean main files
	@echo Cleaning up...
	$(MF_DEBUG)for i in $(CLEAN_FILES); do echo "[RM] $$i"; rm -rf $$i; done

tags: $(SOURCES) ## Create tags
	ctags -R --sort=yes --c++-kinds=+p --fields=+iatS --extra=+q \
		$(SOURCES) $(subst -I,,$(INCLUDE_FLAGS))

FORCE:
print-%: ; @echo $*=$($*)
