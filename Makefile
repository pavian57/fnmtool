# include Husky-Makefile-Config
ifeq ($(DEBIAN), 1)
# Every Debian-Source-Paket has one included.
include /usr/share/husky/huskymak.cfg
else ifdef RPM_BUILD_ROOT
# RPM build requires all files to be in the source directory
include huskymak.cfg
else
include ../huskymak.cfg
endif

SRC_DIR = src$(DIRSEP)
H_DIR = h$(DIRSEP)

ifeq ($(DEBUG), 1)
  CFLAGS = -I$(H_DIR) -I$(INCDIR) $(DEBCFLAGS) $(WARNFLAGS)
  LFLAGS = $(DEBLFLAGS)
else
  CFLAGS = -I$(H_DIR) -I$(INCDIR) $(OPTCFLAGS) $(WARNFLAGS)
  LFLAGS = $(OPTLFLAGS)
endif

ifeq ($(SHORTNAME), 1)
  LIBS=-L$(LIBDIR) -lsmapi -lhusky
else
  LIBS=-L$(LIBDIR) -lsmapi -lhusky
endif

GIT_VERSION := $(shell cat make$(DIRSEP)BUILDVERSION)

CDEFS =-D$(OSTYPE) -DVERSION=\"$(GIT_VERSION)\"  $(ADDCDEFS)

CDEFS += -DCONFIGDIR=\"$(DIRSEP)etc$(DIRSEP)fido$(DIRSEP)fnmtool.cfg\"

default: all

include makefile.inc

fnmtool.1.gz: man/fnmtool.1
	gzip -c man/fnmtool.1 > fnmtool.1.gz

ifeq ($(SHORTNAMES), 1)
all: commonall
else
all: commonall fnmtool.1.gz 
endif

ifeq ($(SHORTNAMES), 1)
install: fnmtool$(_EXE) fnmrfc2ftn$(_EXE)
	$(INSTALL) $(IMOPT) man/fnmtool.1 $(DESTDIR)$(MANDIR)/man1
else
install: fnmtool$(_EXE) fnmrfc2ftn$(_EXE) fnmtool.1.gz
	-$(MKDIR) $(MKDIROPT) $(DESTDIR)$(MANDIR)
	-$(MKDIR) $(MKDIROPT) $(DESTDIR)$(MANDIR)/man1
	$(INSTALL) $(IMOPT) fnmtool.1.gz $(DESTDIR)$(MANDIR)/man1
endif
	-$(MKDIR) $(MKDIROPT) $(DESTDIR)$(BINDIR)
	$(INSTALL) $(IBOPT) fnmtool$(_EXE) $(DESTDIR)$(BINDIR)
	$(INSTALL) $(IBOPT) fnmrfc2ftn$(_EXE) $(DESTDIR)$(BINDIR)

uninstall:
	-$(RM) $(RMOPT) $(DESTDIR)$(MANDIR)$(DIRSEP)man1$(DIRSEP)fnmtool.1
	-$(RM) $(RMOPT) $(DESTDIR)$(BINDIR)$(DIRSEP)fnmtool$(_EXE)
	-$(RM) $(RMOPT) $(DESTDIR)$(BINDIR)$(DIRSEP)fnmrfc2ftn$(_EXE)

