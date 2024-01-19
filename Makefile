################################################################################
# SPDX-License-Identifier: LGPL-3.0-only
#
# This file is part of Stroll.
# Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
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

define list_check_confs_cmds :=
if ! nr=$$($(PYTHON) $(TOPDIR)/scripts/gen_check_confs.py count); then \
	exit 1; \
fi; \
c=0; \
while [ $$c -lt $$nr ]; do \
	echo $$c; \
	c=$$((c + 1)); \
done
endef

check_conf_list       := $(shell $(list_check_confs_cmds))
ifeq ($(strip $(check_conf_list)),)
$(error Missing testing build configurations !)
endif

check_conf_targets    := $(addprefix check-conf,$(check_conf_list))
checkall_builddir     := $(BUILDDIR)/checkall
check_lib_search_path := \
	$(BUILDDIR)/src$(if $(LD_LIBRARY_PATH),:$(LD_LIBRARY_PATH))

CHECK_FORCE ?= y
ifneq ($(filter y 1,$(CHECK_FORCE)),)
.PHONY: $(BUILDDIR)/test/stroll-utest.xml
endif

CHECK_HALT_ON_FAIL ?= n
ifneq ($(filter y 1,$(CHECK_HALT_ON_FAIL)),)
K := --no-keep-going
else
K := --keep-going
endif

CHECK_VERBOSE ?= --silent

.PHONY: checkall
checkall: $(check_conf_targets)

.PHONY: $(check_conf_targets)
$(check_conf_targets): check-conf%: $(checkall_builddir)/conf%/.config
	$(Q)$(MAKE) $(K) -C $(TOPDIR) check BUILDDIR:='$(abspath $(dir $(<)))'
	$(Q)cute-report join --package 'Stroll' \
	                       --revision '$(VERSION)' \
	                       $(checkall_builddir)/stroll-all-utest.xml \
	                       $(dir $(<))/test/stroll-utest.xml \
	                       stroll-conf$(*)

$(addprefix $(checkall_builddir)/conf,$(addsuffix /.config,$(check_conf_list))): \
$(checkall_builddir)/conf%/.config: $(checkall_builddir)/conf%/test.config \
                                    $(config-in)
	$(Q)env KCONFIG_ALLCONFIG='$(<)' \
	        KCONFIG_CONFIG='$(@)' \
	        $(KCONF) --allnoconfig '$(config-in)' >/dev/null
	$(Q)$(MAKE) -C $(TOPDIR) olddefconfig BUILDDIR:='$(abspath $(dir $(@)))'

$(addprefix $(checkall_builddir)/conf,$(addsuffix /test.config,$(check_conf_list))): \
$(checkall_builddir)/conf%/test.config: $(TOPDIR)/scripts/gen_check_confs.py
	@mkdir -p $(dir $(@))
	$(Q)$(PYTHON) $(TOPDIR)/scripts/gen_check_confs.py genone $(*) $(@)

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

clean-check: _clean-checkall

.PHONY: _clean-checkall
_clean-checkall:
	$(call rmr_recipe,$(checkall_builddir))

else  # ifneq ($(strip $(CROSS_COMPILE)),)

.PHONY: check
check checkall:
	$(error Cannot check while cross building !)

endif # ifeq ($(strip $(CROSS_COMPILE)),)

else  # ifneq ($(CONFIG_STROLL_UTEST),y)

.PHONY: check
check checkall:

endif # ifeq ($(CONFIG_STROLL_UTEST),y)
