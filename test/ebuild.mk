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
test-ldflags := $(common-cflags) \
                -L$(BUILDDIR)/../src \
                $(EXTRA_LDFLAGS)

bins :=

ifeq ($(CONFIG_STROLL_BOPS),y)
bins                    += stroll-bops-ut
stroll-bops-ut-objs      = bops.o
stroll-bops-ut-cflags    = $(test-cflags)
stroll-bops-ut-ldflags   = $(test-ldflags) -lstroll
stroll-bops-ut-pkgconf   = cmocka
endif # ($(CONFIG_STROLL_BOPS),y)

ifeq ($(CONFIG_STROLL_BMAP),y)
bins                    += stroll-bmap-ut
stroll-bmap-ut-objs      = bmap.o
stroll-bmap-ut-cflags    = $(test-cflags)
stroll-bmap-ut-ldflags   = $(test-ldflags) -lstroll
stroll-bmap-ut-pkgconf   = cmocka
endif # ($(CONFIG_STROLL_BMAP),y)

ifeq ($(CONFIG_STROLL_LVSTR),y)
bins                    += stroll-lvstr-ut
stroll-lvstr-ut-objs     = lvstr.o
stroll-lvstr-ut-cflags   = $(test-cflags)
stroll-lvstr-ut-ldflags  = $(test-ldflags) -lstroll
stroll-lvstr-ut-pkgconf  = cmocka
endif # ($(CONFIG_STROLL_LVSTR),y)

# vim: filetype=make :
