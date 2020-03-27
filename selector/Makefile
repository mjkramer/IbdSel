# Inspired by https://gist.github.com/maxtruxa/4b3929e118914ccef057f8a05c614b0f

ALL_PRODUCTS := common stage1 stage2

common_DIRS := SelectorFramework/core common
stage1_DIRS := stage1
stage2_DIRS := stage2

BUILD_DIR ?= _build

.PHONY: all
all: $(ALL_PRODUCTS)

.PHONY: common
common: $(BUILD_DIR)/common.so
common: LIBNAME = common

.PHONY: stage1
stage1: $(BUILD_DIR)/stage1.exe $(BUILD_DIR)/stage1.so
stage1: LIBNAME = stage1

.PHONY: stage2
stage2: $(BUILD_DIR)/stage2.exe $(BUILD_DIR)/stage2.so
stage2: LIBNAME = stage2

.PHONY: recalc
recalc: $(BUILD_DIR)/recalc.exe stage2

CC := g++
LD := $(CC)

ifeq ($(IBDSEL_DEBUG),1)
	CFLAGS += -g
else
	CFLAGS += -O3
endif

# so that code can find data files
CFLAGS += -DIBDSEL_BASE=\"$(realpath $(dir $(firstword $(MAKEFILE_LIST)))..)\"

BOOST_HOME ?= /global/homes/m/mkramer/dunescratch/opt-cori/include
EXTRA_INC  := -I$(shell root-config --incdir) -I$(BOOST_HOME)
CFLAGS     += $(shell root-config --cflags) $(EXTRA_INC) -fPIC -fsigned-char -pipe
LDFLAGS    += $(shell root-config --ldflags --libs)

CACHE_DIR := $(BUILD_DIR)/_cache
OBJS_DIR  := $(CACHE_DIR)/objs
DICTS_DIR := $(CACHE_DIR)/dicts
DEPS_DIR  := $(CACHE_DIR)/deps

DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPS_DIR)/$*.dep.tmp

sources_of = $(shell find $($(1)_DIRS) -name '*.cc')
headers_of = $(shell find $($(1)_DIRS) -name '*.hh')

$(foreach p,$(ALL_PRODUCTS),$(eval $(p)_SOURCES := $(call sources_of,$(p))))
$(foreach p,$(ALL_PRODUCTS),$(eval $(p)_HEADERS := $(call headers_of,$(p))))

objs_of  = $(patsubst %.cc,$(OBJS_DIR)/%.o,$($(1)_SOURCES))
deps_of  = $(patsubst %.cc,$(DEPS_DIR)/%.dep,$($(1)_SOURCES))
dicts_of = $(patsubst %.hh,$(DICTS_DIR)/%_hh_ACLiC_dict.o,$($(1)_HEADERS))

$(foreach p,$(ALL_PRODUCTS),$(eval $(p)_OBJS := $(call objs_of,$(p))))
$(foreach p,$(ALL_PRODUCTS),$(eval $(p)_DEPS := $(call deps_of,$(p))))
$(foreach p,$(ALL_PRODUCTS),$(eval $(p)_DICTS := $(call dicts_of,$(p))))

all_cache_of = $($(1)_OBJS) $($(1)_DEPS) $($(1)_DICTS)
all_cache_dirs := $(foreach p,$(ALL_PRODUCTS),$(dir $(call all_cache_of,$(p))))
$(shell mkdir -p $(sort $(all_cache_dirs)))

# We produce/rename .dep.tmp instead of directly producing .dep so that we don't
# end up having Make read in a truncated .dep from an interrupted build
.PRECIOUS: $(OBJS_DIR)/%.o
$(OBJS_DIR)/%.o: %.cc $(DEPS_DIR)/%.dep
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@
	mv -f $(DEPS_DIR)/$*.dep.tmp $(DEPS_DIR)/$*.dep
	@# ensure .o is newer than .dep:
	touch $@

# It seems that dicts don't work unless they follow this exact filename scheme.
.PRECIOUS: $(DICTS_DIR)/%_hh_ACLiC_dict.o
$(DICTS_DIR)/%_hh_ACLiC_dict.o: %.hh $(DICTS_DIR)/%_LinkDef.h
	rootcling -f $(DICTS_DIR)/$*_hh_ACLiC_dict.cxx -multiDict -s $(LIBNAME) $(EXTRA_INC) $^
	$(CC) $(CFLAGS) -c -I. $(DICTS_DIR)/$*_hh_ACLiC_dict.cxx -o $@
	mv $(LIBNAME)_$(notdir $*)_hh_ACLiC_dict_rdict.pcm $(BUILD_DIR)

$(DICTS_DIR)/%_LinkDef.h:
	$(shell sed 's!PLACEHOLDER!$*.hh!' cling/LinkDef.h > $@)

.SECONDEXPANSION:

$(BUILD_DIR)/%.so: $$($$*_OBJS) $$($$*_DICTS)
	$(LD) $(LDFLAGS) $^ -shared -o $@

$(BUILD_DIR)/%.exe: $$*/$$*_exe.cpp $(BUILD_DIR)/$$*.so $(BUILD_DIR)/common.so
	$(CC) $(CFLAGS) $(LDFLAGS) -L$(BUILD_DIR) -l:$*.so -l:common.so $< -o $@

.PHONY: clean
clean:
	rm -rf _build *.pcm

.PRECIOUS: $(DEPS_DIR)/%.dep
$(DEPS_DIR)/%.dep: ;

all_deps := $(foreach p,$(ALL_PRODUCTS),$(call deps_of,$(p)))
-include $(all_deps)
