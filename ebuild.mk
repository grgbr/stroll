################################################################################
# SPDX-License-Identifier: LGPL-3.0-only
#
# This file is part of Stroll.
# Copyright (C) 2017-2023 Grégor Boirie <gregor.boirie@free.fr>
################################################################################

config-in  := Config.in
config-h   := stroll/config.h
config-obj := config.o

HEADERDIR := $(CURDIR)/include
headers    = stroll/cdefs.h
headers   += $(call kconf_enabled,STROLL_ASSERT,stroll/assert.h)
headers   += $(call kconf_enabled,STROLL_BOPS,stroll/bops.h)
headers   += $(call kconf_enabled,STROLL_BMAP,stroll/bmap.h)
headers   += $(call kconf_enabled,STROLL_FBMAP,stroll/fbmap.h)
headers   += $(call kconf_enabled,STROLL_LVSTR,stroll/lvstr.h)
headers   += $(call kconf_enabled,STROLL_BISECT,stroll/bisect.h)

subdirs   := src

ifeq ($(CONFIG_STROLL_UTEST),y)
subdirs   += test
test-deps := src
endif # ($(CONFIG_STROLL_UTEST),y)

ifeq ($(CONFIG_STROLL_PROVIDES_LIBS),y)
override libstroll_pkgconf_libs := Libs: -L$${libdir} \
                                         -Wl,--push-state,--as-needed \
                                         -lstroll \
                                         -Wl,--pop-state
endif # ifeq ($(CONFIG_STROLL_PROVIDES_LIBS),y)

define libstroll_pkgconf_tmpl
prefix=$(PREFIX)
exec_prefix=$${prefix}
libdir=$${exec_prefix}/lib
includedir=$${prefix}/include

Name: libstroll
Description: Stroll library
Version: $(VERSION)
Cflags: -I$${includedir}
$(libstroll_pkgconf_libs)
endef

pkgconfigs        := libstroll.pc
libstroll.pc-tmpl := libstroll_pkgconf_tmpl

################################################################################
# Source code tags generation
################################################################################

tagfiles        := $(shell find $(addprefix $(CURDIR)/,$(subdirs)) \
                                $(HEADERDIR) \
                                -type f)

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
	     EBUILDDOC_INVENTORY_PATH="$(strip $(EBUILDDOC_INVENTORY_PATH))") \
	$(if $(strip $(CUTEDOC_TARGET_PATH)), \
	     CUTEDOC_TARGET_PATH="$(strip $(CUTEDOC_TARGET_PATH))") \
	$(if $(strip $(CUTEDOC_INVENTORY_PATH)), \
	     CUTEDOC_INVENTORY_PATH="$(strip $(CUTEDOC_INVENTORY_PATH))")

################################################################################
# Source distribution generation
################################################################################

# Declare the list of files under revision control to include into final source
# distribution tarball.
override distfiles = $(list_versioned_recipe)

# Override InterSphinx eBuild base documentation URI and make it point to online
# GitHub pages when building final source distribution tarball
dist: export EBUILDDOC_TARGET_PATH := http://grgbr.github.io/ebuild/
dist: export CUTEDOC_TARGET_PATH := http://grgbr.github.io/cute/

# ex: filetype=make :
