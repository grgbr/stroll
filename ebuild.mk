SRCDIR               := $(CURDIR)/src
HEADERDIR            := $(CURDIR)/include

config-in            := Config.in
config-h             := stroll/config.h

common-cflags        := -Wall -Wextra -Wformat=2 -D_GNU_SOURCE $(EXTRA_CFLAGS)
common-ldflags       := $(common-cflags) $(EXTRA_LDFLAGS)

solibs               := libstroll.so
libstroll.so-objs    += $(call kconf_enabled,STROLL_ASSERT,shared/assert.o)
libstroll.so-objs    += $(call kconf_enabled,STROLL_BITMAP,shared/bitmap.o)
libstroll.so-objs    += $(call kconf_enabled,STROLL_LVSTR,shared/lvstr.o)
libstroll.so-cflags   = $(common-cflags) -fpic
libstroll.so-ldflags  = $(common-ldflags) \
                        -shared -Bsymbolic -fpic -Wl,-soname,libstroll.so

arlibs               := libstroll.a
libstroll.a-objs     += $(call kconf_enabled,STROLL_ASSERT,static/assert.o)
libstroll.a-objs     += $(call kconf_enabled,STROLL_BITMAP,static/bitmap.o)
libstroll.a-objs     += $(call kconf_enabled,STROLL_LVSTR,static/lvstr.o)
libstroll.a-cflags    = $(common-cflags)

subdirs := test
test: $(BUILDDIR)/$(solibs) $(BUILDDIR)/$(arlibs)

headers               = stroll/cdefs.h
headers              += $(call kconf_enabled,STROLL_ASSERT,stroll/assert.h)
headers              += $(call kconf_enabled,STROLL_BITOPS,stroll/bitops.h)
headers              += $(call kconf_enabled,STROLL_ATOMIC,stroll/atomic.h)
headers              += $(call kconf_enabled,STROLL_POW2,stroll/pow2.h)
headers              += $(call kconf_enabled,STROLL_BITMAP,utils/bitmap.h)
headers              += $(call kconf_enabled,STROLL_DLIST,utils/dlist.h)
headers              += $(call kconf_enabled,STROLL_SLIST,utils/slist.h)
headers              += $(call kconf_enabled,STROLL_LVSTR,utils/lvstr.h)

define libstroll_pkgconf_tmpl
prefix=$(PREFIX)
exec_prefix=$${prefix}
libdir=$${exec_prefix}/lib
includedir=$${prefix}/include

Name: libstroll
Description: Stroll library
Version: %%PKG_VERSION%%
Requires:
Cflags: -I$${includedir}
Libs: -L$${libdir} -lstroll
endef

pkgconfigs        := libstroll.pc
libstroll.pc-tmpl := libstroll_pkgconf_tmpl

doxyconf  := $(CURDIR)/doc/Doxyfile
doxyenv   := INCDIR="$(patsubst -I%,%,$(filter -I%,$(common-cflags)))" \
             VERSION="$(VERSION)"

sphinxsrc := $(CURDIR)/doc
sphinxenv := VERSION="$(VERSION)"

# vim: filetype=make :
