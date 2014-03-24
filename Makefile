# include Husky-Makefile-Config
include ../huskymak.cfg

ifeq ($(DEBUG), 1)
  CFLAGS=	$(WARNFLAGS) $(DEBCFLAGS) -I$(INCDIR) -DDEBUG
  LFLAGS= $(DEBLFLAGS)
else
  CFLAGS=	$(WARNFLAGS) $(OPTCFLAGS) -s -I$(INCDIR)
  LFLAGS= $(OPTLFLAGS)
endif

CDEFS=-D$(OSTYPE) -DCONFIGDIR=\"/usr/local/etc/fido/fnmtool.cfg\" $(ADDCDEFS)

#CDEFS=-D$(OSTYPE) -DCONFIGDIR=\"fnmtool.cfg\" $(ADDCDEFS)

#CDEFS=-D$(OSTYPE) -DCONFIGDIR=\"/home/ruedi/work/src/fidonet/husky/fnmtool/fnmtool.cfg\" $(ADDCDEFS)


ifeq ($(SHORTNAME), 1)
  LOPT= -L$(LIBDIR) -lhusky -lsmapi -lquickmaillight
else
  LOPT= -L$(LIBDIR) -lhusky -lsmapi -lquickmaillight
endif

OBJ=.o

OBJS=	msgid$(OBJ)	\
	action$(OBJ)	\
	area$(OBJ)	\
	config$(OBJ)	\
	ftnaddr$(OBJ)	\
	global$(OBJ)	\
	log$(OBJ)	\
	mask$(OBJ)	\
	msg$(OBJ)	\
	pkt$(OBJ)	\
	rfcmail$(OBJ)	\
	strsep$(OBJ)

OBJS1=fnmtool$(OBJ) 
OBJS2=fnmrfc2ftn$(OBJ) 


HDRS=action.h area.h bastypes.h config.h ftnaddr.h global.h log.h mask.h \
  msg.h op.h pkt.h scndr.h strsep.h version.h msgid.h

all: fnmtool$(EXE) fnmrfc2ftn$(EXE) fnmtool.1.gz

.cc$(OBJ): $(HDRS)
	$(CXX) $(CFLAGS) $(CDEFS) $(COPT) -c $< -o $@



fnmtool : $(OBJS) $(OBJS1)
	$(CXX) $(OBJS) $(OBJS1) $(LFLAGS) $(LOPT) -o fnmtool


fnmrfc2ftn : $(OBJS) $(OBJS2)
	$(CXX) $(OBJS) $(OBJS2) $(LFLAGS) $(LOPT) -o fnmrfc2ftn


fnmtool.1.gz: fnmtool.1
	gzip -c fnmtool.1 > fnmtool.1.gz

clean:
	-$(RM) $(RMOPT) *$(OBJ)
	-$(RM) $(RMOPT) core
	-$(RM) $(RMOPT) *~ 
	-$(RM) $(RMOPT) .*~ 

distclean: clean
	-$(RM) $(RMOPT) fnmtool$(EXE)
	-$(RM) $(RMOPT) fnmrfc2ftn$(EXE)
	-$(RM) $(RMOPT) fnmtool.1.gz

install: fnmtool$(EXE)
	$(INSTALL) -m 2555 --group=fido fnmtool$(EXE) $(BINDIR)
	$(INSTALL) -m 6555 --group=fido fnmrfc2ftn$(EXE) $(BINDIR)
	$(INSTALL) $(IMOPT) fnmtool.1.gz $(MANDIR)/man1

uninstall:
	-$(RM) $(RMOPT) $(BINDIR)$(DIRSEP)fnmtool$(EXE) 
	-$(RM) $(RMOPT) $(MANDIR)$(DIRSEP)man1$(DIRSEP)fnmtool.1.gz 

# g++ -c rfcmail.cc
# g++ rfcmail.o ftnaddr.o  rfc2ftn.cc -o rfc2ftn  