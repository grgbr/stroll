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
test-ldflags := $(test-cflags) \
                -L$(BUILDDIR)/../src \
                $(EXTRA_LDFLAGS) \
                -Wl,-z,start-stop-visibility=hidden \
                -Wl,-whole-archive $(BUILDDIR)/builtin.a -Wl,-no-whole-archive \
                -lstroll

ifneq ($(filter y,$(CONFIG_STROLL_ASSERT_API) $(CONFIG_STROLL_ASSERT_INTERN)),)
test-cflags  := $(filter-out -DNDEBUG,$(test-cflags))
test-ldflags := $(filter-out -DNDEBUG,$(test-ldflags))
endif # ($(filter y,$(CONFIG_STROLL_ASSERT_API) $(CONFIG_STROLL_ASSERT_INTERN)),)

builtins             := builtin.a
builtin.a-objs       := utest.o $(config-obj)
builtin.a-cflags     := $(test-cflags)

checkbins            := stroll-utest

stroll-utest-objs    := cdefs.o
stroll-utest-objs    += $(call kconf_enabled,STROLL_BOPS,bops.o)
stroll-utest-objs    += $(call kconf_enabled,STROLL_BMAP,bmap.o)
stroll-utest-objs    += $(call kconf_enabled,STROLL_LVSTR,lvstr.o)
stroll-utest-cflags  := $(test-cflags)
stroll-utest-ldflags := $(test-ldflags)
stroll-utest-pkgconf := libcute

# ex: filetype=make :
