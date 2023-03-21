test-cflags    := -Wall -Wextra -Wformat=2 \
                  -D_GNU_SOURCE \
                  -D_STROLL_CMOCKA_UTEST \
                  -I$(TOPDIR)/include \
                  $(EXTRA_CFLAGS)
test-ldflags   := $(common-cflags) \
                  -L$(BUILDDIR)/../src \
                  $(EXTRA_LDFLAGS)

bins           := bitops
bitops-objs     = $(call kconf_enabled,STROLL_BITOPS,bitops.o)
bitops-cflags   = $(test-cflags)
bitops-ldflags  = $(test-ldflags) \
                  -lstroll
bitops-pkgconf  = cmocka

# vim: filetype=make :
