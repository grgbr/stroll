################################################################################
# SPDX-License-Identifier: LGPL-3.0-only
#
# This file is part of Stroll.
# Copyright (C) 2017-2023 Grégor Boirie <gregor.boirie@free.fr>
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
utest-ldflags := $(test-cflags) \
                 -L$(BUILDDIR)/../src \
                 $(EXTRA_LDFLAGS) \
                 -Wl,-z,start-stop-visibility=hidden \
                 -Wl,-whole-archive $(BUILDDIR)/builtin.a -Wl,-no-whole-archive \
                 -lstroll

ptest-ldflags := $(test-cflags) \
                 -L$(BUILDDIR)/../src \
                 $(EXTRA_LDFLAGS) \
                 -Wl,-z,start-stop-visibility=hidden \
                 -lstroll

ifneq ($(filter y,$(CONFIG_STROLL_ASSERT_API) $(CONFIG_STROLL_ASSERT_INTERN)),)
test-cflags   := $(filter-out -DNDEBUG,$(test-cflags))
utest-ldflags := $(filter-out -DNDEBUG,$(utest-ldflags))
ptest-ldflags := $(filter-out -DNDEBUG,$(ptest-ldflags))
endif # ($(filter y,$(CONFIG_STROLL_ASSERT_API) $(CONFIG_STROLL_ASSERT_INTERN)),)

builtins             := builtin.a
builtin.a-objs       := utest.o $(config-obj)
builtin.a-cflags     := $(test-cflags)

checkbins            := stroll-utest
stroll-utest-objs    := cdefs.o
stroll-utest-objs    += $(call kconf_enabled,STROLL_BOPS,bops.o)
stroll-utest-objs    += $(call kconf_enabled,STROLL_POW2,pow2.o)
stroll-utest-objs    += $(call kconf_enabled,STROLL_BMAP,bmap.o)
stroll-utest-objs    += $(call kconf_enabled,STROLL_FBMAP,fbmap.o)
stroll-utest-objs    += $(call kconf_enabled,STROLL_LVSTR,lvstr.o)
stroll-utest-objs    += $(call kconf_enabled,STROLL_ARRAY,array.o array_data.o)
stroll-utest-cflags  := $(test-cflags)
stroll-utest-ldflags := $(utest-ldflags)
stroll-utest-pkgconf := libcute

checkbins                  += stroll-array-ptest
stroll-array-ptest-objs    := ptest.o array_ptest.o
stroll-array-ptest-cflags  := $(test-cflags)
stroll-array-ptest-ldflags := $(ptest-ldflags) -lm

ifeq ($(CONFIG_STROLL_PTEST),y)

ptest-data-nr     := 8 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768 \
                     65536 131072 262144 524288 1048576
ptest-data-orders := 0 5 25 45 50 55 75 95 100
ptest-data-files  := $(foreach n, \
                               $(ptest-data-nr), \
                               $(foreach o, \
                                         $(ptest-data-orders), \
                                         stroll_ptest_$(n)_$(o).dat))

build-check: $(BUILDDIR)/stroll-array-ptest-run.sh \
             $(addprefix $(BUILDDIR)/data/,$(ptest-data-files))

$(BUILDDIR)/stroll-array-ptest-run.sh: $(SRCDIR)/array-ptest-run.sh \
                                       | $(BUILDDIR)/
	sed -e 's;@@BINDIR@@;$(BINDIR);g' \
	    -e 's;@@DATADIR@@;$(DATADIR);g' \
	    $(<) > $(@)

ptest_data_base  := $(BUILDDIR)/data/stroll_ptest_

define ptest_data_nr
$(word 1,$(subst _,$(space),$(patsubst $(ptest_data_base)%.dat,%,$(1))))
endef

define ptest_data_order
$(word 2,$(subst _,$(space),$(patsubst $(ptest_data_base)%.dat,%,$(1))))
endef

$(addprefix $(BUILDDIR)/data/,$(ptest-data-files)): $(SRCDIR)/ptest-data.sh \
                                                    $(SRCDIR)/ptest-data.py \
                                                    | $(BUILDDIR)/data
	@echo "  DATAGEN $(@)"
	$(Q)$${BASH:-bash} $(SRCDIR)/ptest-data.sh \
		--quiet \
		--number $(call ptest_data_nr,$(@)) \
		--order-ratio $(call ptest_data_order,$(@)) \
		$(dir $(@))

$(BUILDDIR)/data:
	$(Q)mkdir -p $(@)

clean-check: _clean-check

.PHONY: _clean-check
_clean-check:
	$(call rm_recipe,$(BUILDDIR)/stroll-array-ptest-run.sh)
	$(call rmr_recipe,$(BUILDDIR)/data)

install-check install-strip-check: \
	$(DESTDIR)$(BINDIR)/stroll-array-ptest-run.sh \
	$(addprefix $(DESTDIR)$(DATADIR)/stroll/,$(ptest-data-files))

.PHONY: $(DESTDIR)$(BINDIR)/stroll-array-ptest-run.sh
$(DESTDIR)$(BINDIR)/stroll-array-ptest-run.sh: \
	$(BUILDDIR)/stroll-array-ptest-run.sh
	$(call install_recipe,--mode=755,$(<),$(@))

$(addprefix $(DESTDIR)$(DATADIR)/stroll/,$(ptest-data-files)): \
	$(addprefix $(BUILDDIR)/data/,$(ptest-data-files))
	$(call install_recipe,-m644,$(<),$(@))

uninstall-check: _uninstall-check

.PHONY: _uninstall-check
_uninstall-check:
	$(call rm_recipe,$(DESTDIR)$(BINDIR)/stroll-array-ptest-run.sh)
	$(call rmr_recipe,$(DESTDIR)$(DATADIR)/stroll)

endif # ($(CONFIG_STROLL_PTEST),y)

# ex: filetype=make :
