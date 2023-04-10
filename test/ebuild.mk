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
                -Wl,-whole-archive $(BUILDDIR)/builtin.a -Wl,-no-whole-archive \
                -lstroll

builtins         := builtin.a
builtin.a-objs   := utest.o
builtin.a-cflags := $(test-cflags)

bins := stroll-cdefs-ut

stroll-cdefs-ut-objs    := cdefs.o
stroll-cdefs-ut-cflags  := $(test-cflags)
stroll-cdefs-ut-ldflags := $(test-ldflags)
stroll-cdefs-ut-pkgconf := cmocka

ifeq ($(CONFIG_STROLL_BOPS),y)
bins                    += stroll-bops-ut
stroll-bops-ut-objs     := bops.o
stroll-bops-ut-cflags   := $(test-cflags)
stroll-bops-ut-ldflags  := $(test-ldflags)
stroll-bops-ut-pkgconf  := cmocka
endif # ($(CONFIG_STROLL_BOPS),y)

ifeq ($(CONFIG_STROLL_BMAP),y)
bins                    += stroll-bmap-ut
stroll-bmap-ut-objs     := bmap.o
stroll-bmap-ut-cflags   := $(test-cflags)
stroll-bmap-ut-ldflags  := $(test-ldflags)
stroll-bmap-ut-pkgconf  := cmocka
endif # ($(CONFIG_STROLL_BMAP),y)

ifeq ($(CONFIG_STROLL_LVSTR),y)
bins                    += stroll-lvstr-ut
stroll-lvstr-ut-objs    := lvstr.o
stroll-lvstr-ut-cflags  := $(test-cflags)
stroll-lvstr-ut-ldflags := $(test-ldflags)
stroll-lvstr-ut-pkgconf := cmocka
endif # ($(CONFIG_STROLL_LVSTR),y)

# vim: filetype=make :
