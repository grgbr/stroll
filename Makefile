#EBUILDDIR     := $(CURDIR)/ebuild
override PACKAGE := stroll
override VERSION := 1.0

EXTRA_CFLAGS     := -O2 -DNDEBUG -Wall -Wextra -Wformat=2
EXTRA_LDFLAGS    := -O2

export VERSION EXTRA_CFLAGS EXTRA_LDFLAGS

EBUILDDIR ?= /usr/share/ebuild
ifeq ($(realpath $(EBUILDDIR)/main.mk),)
$(error '$(EBUILDDIR)': no valid Ebuild install found !)
endif

include $(EBUILDDIR)/main.mk
