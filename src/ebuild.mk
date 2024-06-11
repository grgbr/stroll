################################################################################
# SPDX-License-Identifier: LGPL-3.0-only
#
# This file is part of Stroll.
# Copyright (C) 2017-2023 Grégor Boirie <gregor.boirie@free.fr>
################################################################################

common-cflags        := -Wall \
                        -Wextra \
                        -Wformat=2 \
                        -Wconversion \
                        -Wundef \
                        -Wshadow \
                        -Wcast-qual \
                        -Wcast-align \
                        -Wmissing-declarations \
                        -D_GNU_SOURCE \
                        -I ../include \
                        $(EXTRA_CFLAGS)

ifeq ($(CONFIG_STROLL_UTEST)$(CONFIG_STROLL_ASSERT),yy)
# When unit testsuite is required to be built, make sure to enable ELF semantic
# interposition.
# This allows unit test programs to override the stroll_assert_fail() using
# their own definitions based on cmocka's mock_assert() to validate assertions.
#
# See http://maskray.me/blog/2021-05-09-fno-semantic-interposition for more
# informations about semantic interposition.
common-cflags        := $(common-cflags) -fsemantic-interposition
endif # ($(CONFIG_STROLL_UTEST)$(CONFIG_STROLL_ASSERT),yy)

common-ldflags       := $(common-cflags) $(EXTRA_LDFLAGS) \
                        -Wl,-z,start-stop-visibility=hidden

ifneq ($(filter y,$(CONFIG_STROLL_ASSERT_API) $(CONFIG_STROLL_ASSERT_INTERN)),)
common-cflags         := $(filter-out -DNDEBUG,$(common-cflags))
common-ldflags        := $(filter-out -DNDEBUG,$(common-ldflags))
endif # ($(filter y,$(CONFIG_STROLL_ASSERT_API) $(CONFIG_STROLL_ASSERT_INTERN)),)

ifeq ($(CONFIG_STROLL_PROVIDES_LIBS),y)
solibs               := libstroll.so
libstroll.so-objs    += $(call kconf_enabled,STROLL_ASSERT,shared/assert.o)
libstroll.so-objs    += $(call kconf_enabled,STROLL_POW2,shared/pow2.o)
libstroll.so-objs    += $(call kconf_enabled,STROLL_FBMAP,shared/fbmap.o)
libstroll.so-objs    += $(call kconf_enabled,STROLL_LVSTR,shared/lvstr.o)
libstroll.so-objs    += $(call kconf_enabled,STROLL_ARRAY,shared/array.o)
ifneq ($(filter y,$(CONFIG_STROLL_FBHEAP) $(CONFIG_STROLL_ARRAY_FBHEAP_SORT)),)
libstroll.so-objs    += shared/fbheap.o
endif
ifneq ($(filter y,$(CONFIG_STROLL_FWHEAP) $(CONFIG_STROLL_ARRAY_FWHEAP_SORT)),)
libstroll.so-objs    += shared/fwheap.o
endif
libstroll.so-cflags  := $(filter-out -fpie -fPIE,$(common-cflags)) -fpic
libstroll.so-ldflags := $(filter-out -pie -fpie -fPIE,$(common-ldflags)) \
                        -shared -Bsymbolic -fpic -Wl,-soname,libstroll.so

arlibs               := libstroll.a
libstroll.a-objs     += $(call kconf_enabled,STROLL_ASSERT,static/assert.o)
libstroll.a-objs     += $(call kconf_enabled,STROLL_POW2,static/pow2.o)
libstroll.a-objs     += $(call kconf_enabled,STROLL_FBMAP,static/fbmap.o)
libstroll.a-objs     += $(call kconf_enabled,STROLL_LVSTR,static/lvstr.o)
libstroll.a-objs     += $(call kconf_enabled,STROLL_ARRAY,static/array.o)
ifneq ($(filter y,$(CONFIG_STROLL_FBHEAP) $(CONFIG_STROLL_ARRAY_FBHEAP_SORT)),)
libstroll.a-objs     += static/fbheap.o
endif
ifneq ($(filter y,$(CONFIG_STROLL_FWHEAP) $(CONFIG_STROLL_ARRAY_FWHEAP_SORT)),)
libstroll.a-objs     += static/fwheap.o
endif
libstroll.a-cflags   := $(common-cflags)
endif # ifeq ($(CONFIG_STROLL_PROVIDES_LIBS),y)

# ex: filetype=make :
