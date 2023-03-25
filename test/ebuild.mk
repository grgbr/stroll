test-cflags    := -Wall -Wextra -Wformat=2 \
                  -D_GNU_SOURCE \
                  -D_STROLL_CMOCKA_UTEST \
                  -I$(TOPDIR)/include \
                  $(EXTRA_CFLAGS)
test-ldflags   := $(common-cflags) \
                  -L$(BUILDDIR)/../src \
                  $(EXTRA_LDFLAGS)

bins           :=

ifeq ($(CONFIG_STROLL_BITOPS),y)
bins           += bitops
bitops-objs     = bitops.o
bitops-cflags   = $(test-cflags)
bitops-ldflags  = $(test-ldflags) -lstroll
bitops-pkgconf  = cmocka
endif # ($(CONFIG_STROLL_BITOPS),y)

ifeq ($(CONFIG_STROLL_BITMAP),y)
bins           += bitmap
bitmap-objs     = bitmap.o
bitmap-cflags   = $(test-cflags)
bitmap-ldflags  = $(test-ldflags) -lstroll
bitmap-pkgconf  = cmocka
endif # ($(CONFIG_STROLL_BITMAP),y)

# vim: filetype=make :
