test-cflags    := -Wall -Wextra -Wformat=2 \
                  -D_GNU_SOURCE \
                  -D_STROLL_CMOCKA_UTEST \
                  -I$(TOPDIR)/include \
                  $(EXTRA_CFLAGS)
test-ldflags   := $(common-cflags) \
                  -L$(BUILDDIR)/../src \
                  $(EXTRA_LDFLAGS)

bins           := $(call kconf_enabled,STROLL_BITOPS,bitops)
bitops-objs     = $(call kconf_enabled,STROLL_BITOPS,bitops.o)
bitops-cflags   = $(test-cflags)
bitops-ldflags  = $(test-ldflags) -lstroll
bitops-pkgconf  = cmocka

bins           += $(call kconf_enabled,STROLL_BITMAP,bitmap)
bitmap-objs     = $(call kconf_enabled,STROLL_BITMAP,bitmap.o)
bitmap-cflags   = $(test-cflags)
bitmap-ldflags  = $(test-ldflags) -lstroll
bitmap-pkgconf  = cmocka

# vim: filetype=make :
