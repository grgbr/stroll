test-cflags  := -Wall -Wextra -Wformat=2 \
                -D_GNU_SOURCE \
                -D_STROLL_CMOCKA_UTEST \
                -I../include \
                $(EXTRA_CFLAGS)
test-ldflags := $(common-cflags) \
                -L$(BUILDDIR)/../src \
                $(EXTRA_LDFLAGS)

bins         :=

ifeq ($(CONFIG_STROLL_BOPS),y)
bins         += bops
bops-objs     = bops.o
bops-cflags   = $(test-cflags)
bops-ldflags  = $(test-ldflags) -lstroll
bops-pkgconf  = cmocka
endif # ($(CONFIG_STROLL_BOPS),y)

ifeq ($(CONFIG_STROLL_BMAP),y)
bins         += bmap
bmap-objs     = bmap.o
bmap-cflags   = $(test-cflags)
bmap-ldflags  = $(test-ldflags) -lstroll
bmap-pkgconf  = cmocka
endif # ($(CONFIG_STROLL_BMAP),y)

# vim: filetype=make :
