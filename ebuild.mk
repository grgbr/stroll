config-in := Config.in
config-h  := stroll/config.h

HEADERDIR := $(CURDIR)/include
headers    = stroll/cdefs.h
headers   += $(call kconf_enabled,STROLL_ASSERT,stroll/assert.h)
headers   += $(call kconf_enabled,STROLL_BOPS,stroll/bops.h)
headers   += $(call kconf_enabled,STROLL_BMAP,stroll/bmap.h)
headers   += $(call kconf_enabled,STROLL_LVSTR,stroll/lvstr.h)

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

################################################################################
# Documentation generation
################################################################################

doxyconf  := $(CURDIR)/sphinx/Doxyfile
doxyenv   := SRCDIR="$(HEADERDIR) $(SRCDIR)"

sphinxsrc := $(CURDIR)/sphinx
sphinxenv := \
	VERSION="$(VERSION)" \
	$(if $(strip $(EBUILDDOC_TARGET_PATH)), \
	     EBUILDDOC_TARGET_PATH="$(strip $(EBUILDDOC_TARGET_PATH))") \
	$(if $(strip $(EBUILDDOC_INVENTORY_PATH)), \
	     EBUILDDOC_INVENTORY_PATH="$(strip $(EBUILDDOC_INVENTORY_PATH))")

# ex: filetype=make :
