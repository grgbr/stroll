################################################################################
# SPDX-License-Identifier: LGPL-3.0-only
#
# This file is part of Stroll.
# Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
################################################################################

test-cflags  := -Wall \
                -Wextra \
                -Wformat=2 \
                -Wconversion \
                -Wundef \
                -Wshadow \
                -Wcast-qual \
                -Wcast-align \
                -Wmissing-declarations \
                -D_GNU_SOURCE \
                -DSTROLL_VERSION_STRING="\"$(VERSION)\"" \
                -I../include \
                $(EXTRA_CFLAGS)

# Use -whole-archive to enforce the linker to scan builtin.a static library
# entirely so that symbols in utest.o may override existing strong symbols
# defined into other compilation units.
# This is required since we want stroll_assert_fail() defined into utest.c to
# override stroll_assert_fail() defined into libstroll.so for assertions testing
# purposes.
utest-ldflags := \
	$(test-cflags) \
	-L$(BUILDDIR)/../src \
	$(EXTRA_LDFLAGS) \
	-Wl,-z,start-stop-visibility=hidden \
	-Wl,-whole-archive $(BUILDDIR)/builtin_utest.a -Wl,-no-whole-archive \
	-lstroll

ptest-ldflags := \
	$(test-cflags) \
	-L$(BUILDDIR)/../src \
	$(EXTRA_LDFLAGS) \
	-Wl,-whole-archive $(BUILDDIR)/builtin_ptest.a -Wl,-no-whole-archive \
	-Wl,-z,start-stop-visibility=hidden \
	-lstroll

ifneq ($(filter y,$(CONFIG_STROLL_ASSERT_API) $(CONFIG_STROLL_ASSERT_INTERN)),)
test-cflags   := $(filter-out -DNDEBUG,$(test-cflags))
utest-ldflags := $(filter-out -DNDEBUG,$(utest-ldflags))
ptest-ldflags := $(filter-out -DNDEBUG,$(ptest-ldflags))
endif # ($(filter y,$(CONFIG_STROLL_ASSERT_API) $(CONFIG_STROLL_ASSERT_INTERN)),)

list_kconf    := $(CONFIG_STROLL_SLIST) \
                 $(CONFIG_STROLL_DLIST)
prheap_kconf  := $(CONFIG_STROLL_HPRHEAP) \
                 $(CONFIG_STROLL_DRPHEAP) \
                 $(CONFIG_STROLL_PPRHEAP) \
                 $(CONFIG_STROLL_DPRHEAP) \
                 $(CONFIG_STROLL_DBNHEAP)

builtins               := builtin_utest.a
builtin_utest.a-objs   := utest.o $(config-obj)
builtin_utest.a-cflags := $(test-cflags)

checkbins            := stroll-utest
stroll-utest-objs    := cdefs.o
ifneq ($(filter y,$(CONFIG_STROLL_ARRAY) $(list_kconf)),)
stroll-utest-objs    += array_data.o
endif # ($(filter y,$(CONFIG_STROLL_ARRAY) $(list_kconf)),)
stroll-utest-objs    += $(call kconf_enabled,STROLL_BOPS,bops.o)
stroll-utest-objs    += $(call kconf_enabled,STROLL_POW2,pow2.o)
stroll-utest-objs    += $(call kconf_enabled,STROLL_BMAP,bmap.o)
stroll-utest-objs    += $(call kconf_enabled,STROLL_FBMAP,fbmap.o)
stroll-utest-objs    += $(call kconf_enabled,STROLL_LVSTR,lvstr.o)
stroll-utest-objs    += $(call kconf_enabled,STROLL_ARRAY,array.o)
stroll-utest-objs    += $(call kconf_enabled,STROLL_HEAP,heap.o)
stroll-utest-objs    += $(call kconf_enabled,STROLL_SLIST,slist.o)
stroll-utest-objs    += $(call kconf_enabled,STROLL_DLIST,dlist.o)
ifneq ($(filter y,$(prheap_kconf)),)
stroll-utest-objs    += theap.o
endif # ($(filter y,$(prheap_kconf)),)
stroll-utest-cflags  := $(test-cflags)
stroll-utest-ldflags := $(utest-ldflags)
stroll-utest-pkgconf := libcute

ifeq ($(CONFIG_STROLL_PTEST),y)

builtins                  += builtin_ptest.a
builtin_ptest.a-objs      := ptest.o
builtin_ptest.a-cflags    := $(test-cflags)

ifneq ($(filter y,$(CONFIG_STROLL_ARRAY) $(list_kconf)),)

checkbins                 += stroll-sort-ptest
stroll-sort-ptest-objs    := sort_ptest.o
stroll-sort-ptest-cflags  := $(test-cflags)
stroll-sort-ptest-ldflags := $(ptest-ldflags) -lm

endif # ($(filter y,$(CONFIG_STROLL_ARRAY) $(list_kconf)),)

checkbins                 += $(call kconf_enabled,STROLL_HEAP,stroll-heap-ptest)
stroll-heap-ptest-objs    := heap_ptest.o
stroll-heap-ptest-cflags  := $(test-cflags)
stroll-heap-ptest-ldflags := $(ptest-ldflags) -lm

define ptest_data_files_cmds
for n in $(1); do
	for s in $(2); do
		for o in $(3); do
			if [ $$s -lt 100 ] && [ $$o -lt 50 ]; then
				continue;
			fi;
			if [ $$s -eq 0 ] && [ $$o -ne 100 ]; then
				continue;
			fi;
			echo stroll_ptest_n$${n}_s$${s}_o$${o}.dat;
		done;
	done;
done
endef

ptest-data-nr      := 8 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768 \
                      65536 131072
ptest-data-orders  := 0 5 25 45 50 55 75 95 100
ptest-data-singles := 1 3 5 7 10 15 20 40 60 90 100
ptest-data-files   := $(strip $(shell $(call ptest_data_files_cmds, \
                                             $(ptest-data-nr), \
                                             $(ptest-data-singles), \
                                             $(ptest-data-orders))))

ptest_data_builddir := $(BUILDDIR)/data

build-check: $(BUILDDIR)/stroll-sort-ptest-run.sh \
             $(BUILDDIR)/stroll-heap-ptest-run.sh \
             $(BUILDDIR)/ptest-common.sh \
             $(addprefix $(ptest_data_builddir)/,$(ptest-data-files))

$(BUILDDIR)/stroll-sort-ptest-run.sh: $(SRCDIR)/sort-ptest-run.sh \
                                      | $(BUILDDIR)/
	sed -e 's;@@BINDIR@@;$(BINDIR);g' \
	    -e 's;@@DATADIR@@;$(DATADIR);g' \
	    -e 's;@@LIBEXECDIR@@;$(LIBEXECDIR);g' \
	    $(<) > $(@)

$(BUILDDIR)/stroll-heap-ptest-run.sh: $(SRCDIR)/heap-ptest-run.sh | $(BUILDDIR)/
	sed -e 's;@@BINDIR@@;$(BINDIR);g' \
	    -e 's;@@DATADIR@@;$(DATADIR);g' \
	    -e 's;@@LIBEXECDIR@@;$(LIBEXECDIR);g' \
	    $(<) > $(@)

$(BUILDDIR)/ptest-common.sh: $(SRCDIR)/ptest-common.sh | $(BUILDDIR)/
	sed -e 's;@@BINDIR@@;$(BINDIR);g' \
	    -e 's;@@DATADIR@@;$(DATADIR);g' \
	    -e 's;@@LIBEXECDIR@@;$(LIBEXECDIR);g' \
	    $(<) > $(@)

ptest_data_base := $(ptest_data_builddir)/stroll_ptest_

define _ptest_data_path_probe
$(subst $(2),,$(word $(1),$(subst _,$(space),$(patsubst $(ptest_data_base)%.dat,%,$(3)))))
endef

define _ptest_data_nr
$(call _ptest_data_path_probe,1,n,$(1))
endef

define _ptest_data_single
$(call _ptest_data_path_probe,2,s,$(1))
endef

define _ptest_data_order
$(call _ptest_data_path_probe,3,o,$(1))
endef

$(addprefix $(BUILDDIR)/data/,$(ptest-data-files)): $(SRCDIR)/ptest-data.py \
                                                    | $(BUILDDIR)/data
	@echo "  DATAGEN $(@)"
	$(Q)$(PYTHON) $(SRCDIR)/ptest-data.py generate \
	                                      -o $(@) \
	                                      $(call _ptest_data_nr,$(@)) \
	                                      $(call _ptest_data_order,$(@)) \
	                                      $(call _ptest_data_single,$(@)) \
	                                      $(if $(Q),>/dev/null 2>&1)

$(BUILDDIR)/data:
	$(Q)mkdir -p $(@)

clean-check: _clean-check

.PHONY: _clean-check
_clean-check:
	$(call rm_recipe,$(BUILDDIR)/stroll-sort-ptest-run.sh)
	$(call rm_recipe,$(BUILDDIR)/stroll-heap-ptest-run.sh)
	$(call rm_recipe,$(BUILDDIR)/ptest-common.sh)
	$(call rmr_recipe,$(BUILDDIR)/data)

install-check install-strip-check: \
	$(DESTDIR)$(LIBEXECDIR)/stroll/ptest-common.sh \
	$(DESTDIR)$(BINDIR)/stroll-sort-ptest-run.sh \
	$(DESTDIR)$(BINDIR)/stroll-heap-ptest-run.sh \
	$(addprefix $(DESTDIR)$(DATADIR)/stroll/,$(ptest-data-files))

.PHONY: $(DESTDIR)$(BINDIR)/stroll-sort-ptest-run.sh \
        $(DESTDIR)$(BINDIR)/stroll-heap-ptest-run.sh \
        $(DESTDIR)$(LIBEXECDIR)/stroll/ptest-common.sh
$(DESTDIR)$(BINDIR)/stroll-sort-ptest-run.sh: \
	$(BUILDDIR)/stroll-sort-ptest-run.sh
	$(call install_recipe,--mode=755,$(<),$(@))
$(DESTDIR)$(BINDIR)/stroll-heap-ptest-run.sh: \
	$(BUILDDIR)/stroll-heap-ptest-run.sh
	$(call install_recipe,--mode=755,$(<),$(@))
$(DESTDIR)$(LIBEXECDIR)/stroll/ptest-common.sh: \
	$(BUILDDIR)/ptest-common.sh
	$(call install_recipe,--mode=755,$(<),$(@))

.PHONY: $(addprefix $(DESTDIR)$(DATADIR)/stroll/,$(ptest-data-files))
$(addprefix $(DESTDIR)$(DATADIR)/stroll/,$(ptest-data-files)): \
	$(DESTDIR)$(DATADIR)/stroll/%: $(BUILDDIR)/data/%
	$(call install_recipe,-m644,$(<),$(@))

uninstall-check: _uninstall-check

.PHONY: _uninstall-check
_uninstall-check:
	$(call rm_recipe,$(DESTDIR)$(LIBEXECDIR)/stroll/ptest-common.sh)
	$(call rm_recipe,$(DESTDIR)$(BINDIR)/stroll-sort-ptest-run.sh)
	$(call rm_recipe,$(DESTDIR)$(BINDIR)/stroll-heap-ptest-run.sh)
	$(call rmr_recipe,$(DESTDIR)$(DATADIR)/stroll)

endif # ($(CONFIG_STROLL_PTEST),y)

# ex: filetype=make :
