# Directory definitions
SRC=src/
BIN=bin/
#INC=inc/
LIB=lib/
#DEP=dep/

# Architecture and compiler name from root-config
ROOTVERSION:=$(shell $(ROOTSYS)/bin/root-config --version | cut -b1-4)
ifeq ($(ROOTVERSION),5.27)
MARCH:=`root-config --arch`
else
MARCH:=`root-config --arch`$(ROOTVERSION)
endif
CXX:=`root-config --cxx`

# Flags used in compilation
CPPFLAGS= -I$(SRC) -D_PGTRACK_ -D__ROOTSHAREDLIBRARY__ -Ilib -I$(AMSSRC)/include -I$(ROOTSYS)/include
# -Wno-pragmas hides warnings "ignoring #pragma omp atomic" in AMS ROOT code
CXXFLAGS= $(DEBUGOPT) -fPIC -Wno-pragmas -Wno-write-strings  $(CPPFLAGS)
CFLAGS = $(DEBUGOPT)

# By default, compiliation is optimized with -O3 flag.  Make target "debug" to use debug flag -g instead.
DEBUGOPT = -O3

# Assume that we are using SLC6.  This goes into the name of the AMS library used
SLC_NO?=6
AMSNTUPLELIB=ntuple_slc$(SLC_NO)_PG

# All libs to be built (The last two are symbolic links for legacy support)
ALL_LIBS = $(LIB)libTWR.so $(LIB)libTWR_a.a $(LIB)libResTemp.so $(LIB)libResTemp_a.a

# Files to be included in library (classes & common functions); each should have a corresponding header file.
# These should also match the pragma statements in the linkdef.h file
LIB_SRCS = $(SRC)resClassCommon.C $(SRC)HistoProofMan.C
LIB_SRCS+= $(SRC)twrNTuple.C $(SRC)twrLevel1R.C $(SRC)twrRTI.C $(SRC)twrRichQuality.C

LIB_HEADS = $(LIB_SRCS:$(SRC)%.C=$(SRC)%.h)
LIB_OBJS  = $(LIB_SRCS:$(SRC)%.C=$(BIN)%.o)
#LIB_DEPS  = $(LIB_SRCS:$(SRC)%.C=$(DEP)%.d)
LIB_HEADS_BARE = $(LIB_HEADS:$(SRC)%=%)
# The above list is used in the call to rootcint (strip directories off filenames)

# Files not to be included in library (executables, ...)
PROG_SRCS = $(SRC)processSingleFile.C
# Only remake main executables if there are no changes to be committed in git
ALL_PROGS_FINAL = processSingleFile.out
ALL_PROGS_NC = $(ALL_PROGS_FINAL:%.out=%_NOCOMMIT.out)

# Wildcard targets
$(BIN)%.o: $(SRC)%.C
	@echo ">> Compiling $< into $@ ..."
	@if ! [ -d $(BIN) ] ; then mkdir -p $(BIN); fi
	$(CXX) -c $(CXXFLAGS) $< -o $@ -L$(AMSWD)/lib/$(MARCH)/ -l$(AMSNTUPLELIB)

# %.out: $(SRC)%.C
# 	@echo ">> Making executable $@ ..."
# 	$(CXX) -o $@ $(CXXFLAGS) $< -L$(LIB) -lResTemp_a -L$(AMSWD)/lib/$(MARCH)/ -l$(AMSNTUPLELIB) `root-config --cflags --glibs` -lMinuit -lTMVA -lXMLIO -lMLP -lTreePlayer -lgfortran -lRFIO -lNetx

# Named targets
default:
	@echo "Please choose a specific named target (e.g., all, debug, clean, ...)"

all: dirStructure lib prog
	$(MAKE) -C sql

all_clean: clean_lib dirStructure lib prog
	$(MAKE) -C sql all_clean

debug: DEBUGOPT=-g
debug: all

debug_clean: DEBUGOPT=-g
debug_clean: all_clean

#prog: processSingleFile.out
prog: $(ALL_PROGS_FINAL) version

# Executable targets
#processSingleFile.out: $(BIN)twrNTupleFiller.o $(BIN)processSingleFile.o  $(LIB)libTWR_DA_a.a 
#	@echo ">> Making executable $@ ..."
#	$(CXX) -o $@ $^ $(CXXFLAGS) -L$(LIB) -lResTemp_a -L$(AMSWD)/lib/$(MARCH)/ -l$(AMSNTUPLELIB) `root-config --cflags --glibs` -lMinuit -lTMVA -lXMLIO -lMLP -lTreePlayer -lgfortran -lRFIO -lNetx

# Common dependencies
processSingleFile.out version: $(BIN)twrNTupleFiller.o $(BIN)processSingleFile.o  $(LIB)libTWR_a.a

# Version tracking file for executables
version:
	@echo ">> Updating versioning file .version ..."
	@git describe --tags --long > .version_NOCOMMIT
	@./.moveFileIfRepoClean.sh .version_NOCOMMIT .version

processSingleFile.out: 
	@echo ">> Making executable $@ ..."
	$(CXX) -o processSingleFile_NOCOMMIT.out $^ $(CXXFLAGS) -L$(LIB) -lTWR_a -L$(AMSWD)/lib/$(MARCH)/ -l$(AMSNTUPLELIB) `root-config --cflags --glibs` -lMinuit -lTMVA -lXMLIO -lMLP -lTreePlayer -lgfortran -lRFIO -lNetx
	@./.moveFileIfRepoClean.sh processSingleFile_NOCOMMIT.out $@

# Directory structure target
dirStructure:
	@if ! [ -d $(BIN) ] ; then mkdir -p $(BIN); fi
	@if ! [ -d $(LIB) ] ; then mkdir -p $(LIB); fi

# Libraries
lib: $(ALL_LIBS)

# Library for DATA runs
$(LIB)libTWR.so:  $(BIN)TWR_Dict.o $(LIB_OBJS)
	@echo ">> Generating shared library $@ ..."
	$(CXX) -shared -o $@ $^ -L$(AMSWD)/lib/$(MARCH)/  `root-config --libs` -lMinuit -lTMVA -lXMLIO -lMLP -lTreePlayer

$(LIB)libTWR_a.a: $(BIN)TWR_Dict.o $(LIB_OBJS)
	@echo ">> Generating static library $@ ..."
	ar -rv $@ $^

$(BIN)TWR_Dict.C: $(LIB_HEADS) $(SRC)linkdef.h
	@echo ">> Using rootcint to generate TWR_Dict.C ..."
	rootcint -f $@ -c -p -I$(SRC) -I$(AMSSRC)/include $(LIB_HEADS_BARE) linkdef.h

$(BIN)TWR_Dict.o: $(BIN)TWR_Dict.C
	@echo ">> Compiling dictionary $< ..."
	$(CXX) -c $(CXXFLAGS) $< -o $@

# Old library names -> link to new names for data library
lib_aliases: $(LIB)libResTemp.so $(LIB)libResTemp_a.a

$(LIB)libResTemp.so: | $(LIB)libTWR.so
	ln -s libTWR.so $(LIB)libResTemp.so

$(LIB)libResTemp_a.a: | $(LIB)libTWR_a.a
	ln -s libTWR_a.a $(LIB)libResTemp_a.a


# Target used for testing
test:
	@echo $(LIB_SRCS)
	@echo $(LIB_OBJS)
	@echo $(LIB_HEADS)
	@echo $(LIB_HEADS_BARE)
	@echo $(ALL_PROGS_FINAL)
	@echo "Git clean: $(GIT_CLEAN)"

.PHONY: all all_clean prog lib lib_aliases dirStructure test clean clean_lib debug debug_clean

# Cleanup routines
clean:
	@echo ">> Deleting object files and Dict files ..."
	rm -fv $(BIN)*.o
	rm -fv $(BIN)TWR_Dict.C
	rm -fv $(BIN)TWR_Dict.h
# Keep the "commit" version of the main executable, if it exists.
	rm -fv $(ALL_PROGS_NC)
	rm -fv .version_NOCOMMIT
#	$(MAKE) -C sql clean

clean_lib: clean
	@echo ">> Deleting libraries ..."
	rm -fv $(ALL_LIBS)

