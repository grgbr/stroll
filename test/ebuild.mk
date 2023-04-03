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
bins                   += stroll-bops-ut
stroll-bops-ut-objs     = bops.o
stroll-bops-ut-cflags   = $(test-cflags)
stroll-bops-ut-ldflags  = $(test-ldflags) -lstroll
stroll-bops-ut-pkgconf  = cmocka
endif # ($(CONFIG_STROLL_BOPS),y)

ifeq ($(CONFIG_STROLL_BMAP),y)
bins                   += stroll-bmap-ut
stroll-bmap-ut-objs     = bmap.o
stroll-bmap-ut-cflags   = $(test-cflags)
stroll-bmap-ut-ldflags  = $(test-ldflags) -lstroll
stroll-bmap-ut-pkgconf  = cmocka
endif # ($(CONFIG_STROLL_BMAP),y)

# vim: filetype=make :
