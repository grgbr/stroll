config-in := Config.in
config-h  := stroll/config.h

HEADERDIR := $(CURDIR)/include
headers    = stroll/cdefs.h
headers   += $(call kconf_enabled,STROLL_ASSERT,stroll/assert.h)
headers   += $(call kconf_enabled,STROLL_BOPS,stroll/bops.h)
headers   += $(call kconf_enabled,STROLL_BMAP,stroll/bmap.h)

subdirs   := src

ifeq ($(CONFIG_STROLL_UTEST),y)
subdirs   += test
test-deps := src
endif # ($(CONFIG_STROLL_UTEST),y)

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

doxyconf  := $(CURDIR)/sphinx/Doxyfile
doxyenv   := INCDIR="$(patsubst -I%,%,$(filter -I%,$(common-cflags)))" \
             VERSION="$(VERSION)"

sphinxsrc := $(CURDIR)/sphinx
sphinxenv := VERSION="$(VERSION)"

# vim: filetype=make :
