################################################################################
# SPDX-License-Identifier: LGPL-3.0-only
#
# This file is part of Stroll.
# Copyright (C) 2017-2023 Grégor Boirie <gregor.boirie@free.fr>
################################################################################

override PACKAGE := stroll
override VERSION := 1.0
EXTRA_CFLAGS     := -O2 -DNDEBUG -Wall -Wextra -Wformat=2
EXTRA_LDFLAGS    := -O2

export VERSION EXTRA_CFLAGS EXTRA_LDFLAGS

ifeq ($(strip $(EBUILDDIR)),)
ifneq ($(realpath ebuild/main.mk),)
EBUILDDIR := $(realpath ebuild)
else  # ($(realpath ebuild/main.mk),)
EBUILDDIR := $(realpath /usr/share/ebuild)
endif # !($(realpath ebuild/main.mk),)
endif # ($(strip $(EBUILDDIR)),)

ifeq ($(realpath $(EBUILDDIR)/main.mk),)
$(error '$(EBUILDDIR)': no valid eBuild install found !)
endif # ($(realpath $(EBUILDDIR)/main.mk),)

include $(EBUILDDIR)/main.mk

.PHONY: gh
gh: export EBUILDDOC_TARGET_PATH := http://grgbr.github.io/ebuild/
gh: html
	@$(RM) -r $(TOPDIR)/docs
	$(call installdir_recipe,,$(sphinxhtmldir),$(TOPDIR)/docs)
	@touch $(TOPDIR)/docs/.nojekyll

.PHONY: clean-gh
clean-gh:
	$(call rmr_recipe,$(TOPDIR)/docs)
