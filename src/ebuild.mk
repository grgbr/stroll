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

solibs               := libstroll.so
libstroll.so-objs    := shared/page.o
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
libstroll.so-objs    += $(call kconf_enabled,STROLL_SLIST,shared/slist.o)
libstroll.so-objs    += $(call kconf_enabled,STROLL_DLIST,shared/dlist.o)
libstroll.so-objs    += $(call kconf_enabled,STROLL_HPRHEAP,shared/hprheap.o)
libstroll.so-objs    += $(call kconf_enabled,STROLL_DRPHEAP,shared/drpheap.o)
libstroll.so-objs    += $(call kconf_enabled,STROLL_DPRHEAP,shared/dprheap.o)
libstroll.so-objs    += $(call kconf_enabled,STROLL_PPRHEAP,shared/pprheap.o)
libstroll.so-objs    += $(call kconf_enabled,STROLL_DBNHEAP,shared/dbnheap.o)
libstroll.so-objs    += $(call kconf_enabled,STROLL_HLIST,shared/hlist.o)
libstroll.so-objs    += $(call kconf_enabled,STROLL_ALLOC,shared/alloc.o)
libstroll.so-objs    += $(call kconf_enabled,STROLL_PALLOC,shared/palloc.o)
libstroll.so-objs    += $(call kconf_enabled,STROLL_LALLOC,shared/lalloc.o)
libstroll.so-objs    += $(call kconf_enabled,STROLL_FALLOC,shared/falloc.o)
libstroll.so-objs    += $(call kconf_enabled,STROLL_MSG,shared/message.o)
libstroll.so-cflags  := $(filter-out -fpie -fPIE,$(common-cflags)) -fpic
libstroll.so-ldflags := $(filter-out -pie -fpie -fPIE,$(common-ldflags)) \
                        -shared -Bsymbolic -fpic -Wl,-soname,libstroll.so

arlibs               := libstroll.a
libstroll.a-objs     := static/page.o
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
libstroll.a-objs     += $(call kconf_enabled,STROLL_SLIST,static/slist.o)
libstroll.a-objs     += $(call kconf_enabled,STROLL_DLIST,static/dlist.o)
libstroll.a-objs     += $(call kconf_enabled,STROLL_HPRHEAP,static/hprheap.o)
libstroll.a-objs     += $(call kconf_enabled,STROLL_DRPHEAP,static/drpheap.o)
libstroll.a-objs     += $(call kconf_enabled,STROLL_DPRHEAP,static/dprheap.o)
libstroll.a-objs     += $(call kconf_enabled,STROLL_PPRHEAP,static/pprheap.o)
libstroll.a-objs     += $(call kconf_enabled,STROLL_DBNHEAP,static/dbnheap.o)
libstroll.a-objs     += $(call kconf_enabled,STROLL_HLIST,static/hlist.o)
libstroll.a-objs     += $(call kconf_enabled,STROLL_ALLOC,static/alloc.o)
libstroll.a-objs     += $(call kconf_enabled,STROLL_PALLOC,static/palloc.o)
libstroll.a-objs     += $(call kconf_enabled,STROLL_LALLOC,static/lalloc.o)
libstroll.a-objs     += $(call kconf_enabled,STROLL_FALLOC,static/falloc.o)
libstroll.a-objs     += $(call kconf_enabled,STROLL_MSG,static/message.o)
libstroll.a-cflags   := $(common-cflags)

# ex: filetype=make :
