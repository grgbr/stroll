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

ifeq ($(CONFIG_STROLL_UTEST),y)

ifeq ($(strip $(CROSS_COMPILE)),)

CHECK_FORCE ?= y
ifneq ($(filter y 1,$(CHECK_FORCE)),)
.PHONY: $(BUILDDIR)/test/stroll-utest.xml
endif

CHECK_VERBOSE ?= --silent

check_lib_search_path := \
	$(BUILDDIR)/src$(if $(LD_LIBRARY_PATH),:$(LD_LIBRARY_PATH))

.PHONY: check
check: $(BUILDDIR)/test/stroll-utest.xml

$(BUILDDIR)/test/stroll-utest.xml: | build-check
	@echo "  CHECK   $(@)"
	$(Q)env LD_LIBRARY_PATH="$(check_lib_search_path)" \
	        $(BUILDDIR)/test/stroll-utest \
	        $(CHECK_VERBOSE) \
	        --xml='$(@)' \
	        run

clean-check: _clean-check

.PHONY: _clean-check
_clean-check:
	$(call rm_recipe,$(BUILDDIR)/test/stroll-utest.xml)

else  # ifneq ($(strip $(CROSS_COMPILE)),)

.PHONY: check
check:
	$(error Cannot check while cross building !)

endif # ifeq ($(strip $(CROSS_COMPILE)),)

else  # ifneq ($(CONFIG_STROLL_UTEST),y)

.PHONY: check
check:

endif # ifeq ($(CONFIG_STROLL_UTEST),y)
