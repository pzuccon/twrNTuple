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
CXXFLAGS= $(DEBUGOPT) -fPIC -Wno-write-strings  $(CPPFLAGS)
CFLAGS = $(DEBUGOPT)
DEBUGOPT = -O3
SLC_NO=6
# if test -z "$$SLC_NO"; then SLC_NO="6"; echo "SLC_NO environment variable not set.  Using default value of SLC_NO=6."; fi;
AMSNTUPLELIB=ntuple_slc$(SLC_NO)_PG
# SLC_NO=4

# Files to be included in library (classes & common functions); each should have a corresponding header file.
# These should also match the pragma statements in the linkdef.h file
LIB_SRCS = $(SRC)resClassCommon.C $(SRC)HistoProofMan.C
LIB_SRCS+= $(SRC)twrNTuple.C $(SRC)twrLevel1R.C $(SRC)twrRTI.C 

LIB_HEADS = $(LIB_SRCS:$(SRC)%.C=$(SRC)%.h)
LIB_OBJS  = $(LIB_SRCS:$(SRC)%.C=$(BIN)%.o)
#LIB_DEPS  = $(LIB_SRCS:$(SRC)%.C=$(DEP)%.d)
LIB_HEADS_BARE = $(LIB_HEADS:$(SRC)%=%)
# The above list is used in the call to rootcint (strip directories off filenames)

# Files not to be included in library (executables, ...)
PROG_SRCS = $(SRC)processSingleFile.C

PROGS = $(PROG_SRCS:$(SRC)%.C=%.out)

# Wildcard targets
$(BIN)%.o: $(SRC)%.C
	@echo ">> Compiling $< ..."
	@if ! [ -d $(BIN) ] ; then mkdir -p $(BIN); fi
	$(CXX) -c $(CXXFLAGS) $< -o $@ -L$(AMSWD)/lib/$(MARCH)/ -l$(AMSNTUPLELIB)

# %.out: $(SRC)%.C
# 	@echo ">> Making executable $@ ..."
# 	$(CXX) -o $@ $(CXXFLAGS) $< -L$(LIB) -lResTemp_a -L$(AMSWD)/lib/$(MARCH)/ -l$(AMSNTUPLELIB) `root-config --cflags --glibs` -lMinuit -lTMVA -lXMLIO -lMLP -lTreePlayer -lgfortran -lRFIO -lNetx

# Named targets
all: dirStructure lib prog
	$(MAKE) -C sql

all_clean: clean_lib dirStructure lib prog
	$(MAKE) -C sql all_clean

debug: DEBUGOPT=-g
debug: all

debug_clean: DEBUGOPT=-g
debug_clean: all_clean

prog: processSingleFile.out


processSingleFile.out: $(BIN)twrNTupleFiller.o $(BIN)processSingleFile.o  $(LIB)libResTemp_a.a
	@echo ">> Making executable $@ ..."
	$(CXX) -o $@ $^ $(CXXFLAGS) -L$(LIB) -lResTemp_a -L$(AMSWD)/lib/$(MARCH)/ -l$(AMSNTUPLELIB) `root-config --cflags --glibs` -lMinuit -lTMVA -lXMLIO -lMLP -lTreePlayer -lgfortran -lRFIO -lNetx


lib: $(LIB)libResTemp.so $(LIB)libResTemp_a.a

dirStructure:
	@if ! [ -d $(BIN) ] ; then mkdir -p $(BIN); fi
	@if ! [ -d $(LIB) ] ; then mkdir -p $(LIB); fi

$(LIB)libResTemp.so:  $(BIN)ResTempDict.o $(LIB_OBJS)
	@echo ">> Generating shared library $@ ..."
	$(CXX) -shared -o $@ $^ -L$(AMSWD)/lib/$(MARCH)/  `root-config --libs` -lMinuit -lTMVA -lXMLIO -lMLP -lTreePlayer

$(LIB)libResTemp_a.a: $(BIN)ResTempDict.o $(LIB_OBJS)
	@echo ">> Generating static library $@ ..."
	ar -rv $@ $^

$(BIN)ResTempDict.C: $(LIB_HEADS) $(SRC)linkdef.h
	@echo ">> Using rootcint to generate ResTempDict.C ..."
	rootcint -f $@ -c -p -I$(SRC) -I$(AMSSRC)/include $(LIB_HEADS_BARE) linkdef.h

$(BIN)ResTempDict.o: $(BIN)ResTempDict.C
	@echo ">> Compiling dictionary $< ..."
	$(CXX) -c $(CXXFLAGS) $< -o $@

test:
	@echo $(LIB_SRCS)
	@echo $(LIB_OBJS)
	@echo $(LIB_HEADS)
	@echo $(LIB_HEADS_BARE)
	@echo $(PROGS)

.PHONY: all all_clean prog lib dirStructure test clean clean_lib clean_ntuples debug debug_clean

# Cleanup routines
clean:
	@echo ">> Deleting object files and ResTempDict files ..."
	rm -fv $(BIN)*.o
	rm -fv $(BIN)ResTempDict.C
	rm -fv $(BIN)ResTempDict.h
	rm -fv $(PROGS)
	$(MAKE) -C sql clean

clean_lib: clean
	@echo ">> Deleting libraries $(LIB)libResTemp.so, $(LIB)libResTemp_a.a ..."
	rm -fv $(LIB)libResTemp.so
	rm -fv $(LIB)libResTemp_a.a

