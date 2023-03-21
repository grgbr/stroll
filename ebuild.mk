config-in := Config.in
config-h  := stroll/config.h

HEADERDIR := $(CURDIR)/include
headers    = stroll/cdefs.h
headers   += $(call kconf_enabled,STROLL_ASSERT,stroll/assert.h)
headers   += $(call kconf_enabled,STROLL_BITOPS,stroll/bitops.h)
headers   += $(call kconf_enabled,STROLL_ATOMIC,stroll/atomic.h)
headers   += $(call kconf_enabled,STROLL_POW2,stroll/pow2.h)
headers   += $(call kconf_enabled,STROLL_BITMAP,utils/bitmap.h)
headers   += $(call kconf_enabled,STROLL_DLIST,utils/dlist.h)
headers   += $(call kconf_enabled,STROLL_SLIST,utils/slist.h)
headers   += $(call kconf_enabled,STROLL_LVSTR,utils/lvstr.h)

subdirs   := src test
test-deps := src

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
