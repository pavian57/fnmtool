# you must  define at least the following variables if you want to
# include this file:
#OBJ    =.o
#RM     =rm
#DIRSEP =/
#CFLAGS =-I../h -I../.. -c
#LFLAGS =-s
#CDEFS  =-DUNIX
#CC     =gcc
EXENAMEFLAG=-o
#SRC_DIR=../src/
#LIBS   =-L$(LIBDIR) -lfidoconfig -lsmapi
_CXX=.cc

# Object files: programs

OLDFILES  := *~ $(SRC_DIR)*~ $(H_DIR)*~ make/*~ 

OBJS = 	msgid$(_OBJ) action$(_OBJ) area$(_OBJ) config$(_OBJ) ftnaddr$(_OBJ)	\
			 	global$(_OBJ) log$(_OBJ) mask$(_OBJ) msg$(_OBJ)	pkt$(_OBJ)	\
				rfcmail$(_OBJ) fnmsmtp$(_OBJ) strsep$(_OBJ)

commonall: $(OBJS) \
     fnmtool$(_EXE) \
     fnmrfc2ftn$(_EXE)

%$(_OBJ): $(SRC_DIR)%$(_CXX)
	$(CXX) $(CFLAGS) $(CDEFS) $(SRC_DIR)$*.cc

fnmtool$(_EXE) : $(BJS) fnmtool$(_OBJ)
	$(CXX) $(LFLAGS) $(EXENAMEFLAG) fnmtool$(_EXE)  fnmtool$(_OBJ) $(OBJS) $(LIBS)

fnmrfc2ftn$(_EXE) : $(OBJS) fnmrfc2ftn$(_OBJ)
	$(CXX) $(LFLAGS) $(EXENAMEFLAG) fnmrfc2ftn$(_EXE) fnmrfc2ftn$(_OBJ) $(OBJS) $(LIBS)


commonclean: clean

commondistclean:	distclean


clean:
	-$(RM) $(RMOPT) *$(_OBJ)
	-$(RM) $(RMOPT) core
	-$(RM) $(RMOPT) $(OLDFILES)
	-$(RM) $(RMOPT) .*~ 

distclean: clean
	-$(RM) $(RMOPT) fnmtool$(_EXE)
	-$(RM) $(RMOPT) fnmrfc2ftn$(_EXE)
	-$(RM) $(RMOPT) fnmtool.1.gz

