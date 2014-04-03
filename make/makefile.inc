# Use defines before including this file:
# _OBJ	object files suffix begins with point (".o", ".obj", ... )
# _LIB	library files suffix begins with point (".a", ".lib", ... )
# _DLL	dinamic linking library files suffix begins with point (".dll", ".so", ...)
# LIBPREFIX  library filename prefix ("lib", "", ...)
# LIBSUFFIX  library filename suffix (compiler ID, three characters, somewhere empty)
# DLLPREFIX  dinamic linking library prefix (usually empty)
# DLLSUFFIX  dinamic linking library suffix (compiler ID or empty)
# O, B, L, H prefixes to use with names of objects, binaries, libraries and headers respectively

LIBNAME = fnmtool

# Version
VERMAJOR= 1
VERMINOR= 0
VERPATCH= 0
VERH	= $(VERMAJOR).$(VERMINOR)
VER	= $(VERH).$(VERPATCH)

# Sources directory
_SRC_DIR= ../src/
# Headers directory
H_DIR	= ../h/

# Programms name
FNMTOOL = $(B)fnmtool$(_EXE)
FNMRFC2FTN = $(B)fnmrfc2ftn$(_EXE)

# Header files
HEADERS = $(H)action.h $(H)area.h $(H)bastypes.h $(H)config.h $(H)ftnaddr.h $(H)global.h	\
			$(H)log.h $(H)mask.h $(H)msg.h $(H)op.h $(H)pkt.h $(H)scndr.h	\
			$(H)strsep.h $(H)version.h $(H)msgid.h $(H)fnmsmtp.h

# Object files: library
OBJS	=

# Object files: programs
FNMTOOLOBJ =  $(O)fnmtool$(_OBJ)  $(O)msgid$(_OBJ) $(O)action$(_OBJ) $(O)area$(_OBJ) $(O)config$(_OBJ)	\
			$(O)ftnaddr$(_OBJ) $(O)global$(_OBJ) $(O)log$(_OBJ)	$(O)mask$(_OBJ)	\
			$(O)msg$(_OBJ) $(O)pkt$(_OBJ) $(O)rfcmail$(_OBJ) $(O)fnmsmtp$(_OBJ)	\
			$(O)strsep$(_OBJ)
      
FNMRFC2FTNOBJ = $(O)fnmrfc2ftn$(_OBJ)  $(O)msgid$(_OBJ) $(O)action$(_OBJ) $(O)area$(_OBJ) $(O)config$(_OBJ)	\
			$(O)ftnaddr$(_OBJ) $(O)global$(_OBJ) $(O)log$(_OBJ)	$(O)mask$(_OBJ)	\
			$(O)msg$(_OBJ) $(O)pkt$(_OBJ) $(O)rfcmail$(_OBJ) $(O)fnmsmtp$(_OBJ)	\
			$(O)strsep$(_OBJ) 


PRGOBJS = $(FNMTOOLOBJ) $(FNMRFC2FTNOBJ)


# Binary files(s) to bild from sources
PROGRAMS  = $(FNMTOOL) $(FNMRFC2FTN)


# List of libraries required to build binary file(s)
LIBS	=
