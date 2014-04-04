#!/bin/bash
echo "#define PRGNAME \"fnmtool\"" > h/version.h
echo "#define RFC2FTN \"fnmrfc2ftn\""  >> h/version.h
echo "#define VERSION \"$(git describe --tags --long --dirty)\"" >> h/version.h
echo "#ifdef UNIX"  >> h/version.h
echo "  #define OS \"lnx\""  >> h/version.h
echo "#elif WIN"  >> h/version.h
echo "  #define OS \"win\""  >> h/version.h
echo "#endif"  >> h/version.h


