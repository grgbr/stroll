common-cflags        := -Wall \
                        -Wextra \
                        -Wformat=2 \
                        -Wconversion \
                        -Wundef \
                        -Wshadow \
                        -Wcast-qual \
                        -Wcast-align \
                        -Wmissing-declarations \
                        -D_GNU_SOURCE \
                        -I ../include \
                        $(EXTRA_CFLAGS)
common-ldflags       := $(common-cflags) $(EXTRA_LDFLAGS)

solibs               := libstroll.so
libstroll.so-objs    += $(call kconf_enabled,STROLL_ASSERT,shared/assert.o)
libstroll.so-objs    += $(call kconf_enabled,STROLL_LVSTR,shared/lvstr.o)
libstroll.so-cflags   = $(common-cflags) -fpic
libstroll.so-ldflags  = $(common-ldflags) \
                        -shared -Bsymbolic -fpic -Wl,-soname,libstroll.so

arlibs               := libstroll.a
libstroll.a-objs     += $(call kconf_enabled,STROLL_ASSERT,static/assert.o)
libstroll.a-objs     += $(call kconf_enabled,STROLL_LVSTR,static/lvstr.o)
libstroll.a-cflags    = $(common-cflags)

# vim: filetype=make :
