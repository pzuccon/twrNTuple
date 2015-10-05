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
ALL_LIBS = $(LIB)libTWR_DA.so $(LIB)libTWR_DA_a.a $(LIB)libTWR_MC.so $(LIB)libTWR_MC_a.a $(LIB)libResTemp.so $(LIB)libResTemp_a.a

# Files to be included in library (classes & common functions); each should have a corresponding header file.
# These should also match the pragma statements in the linkdef.h file
LIB_SRCS = $(SRC)resClassCommon.C $(SRC)HistoProofMan.C
LIB_SRCS+= $(SRC)twrNTuple.C $(SRC)twrLevel1R.C $(SRC)twrRTI.C $(SRC)twrTrdK.C $(SRC)twrTrdKHit.C $(SRC)twrRichQuality.C

LIB_HEADS = $(LIB_SRCS:$(SRC)%.C=$(SRC)%.h)
LIB_OBJS  = $(LIB_SRCS:$(SRC)%.C=$(BIN)%.o)
LIB_OBJS_MC = $(LIB_SRCS:$(SRC)%.C=$(BIN)MC/%.o)
#LIB_DEPS  = $(LIB_SRCS:$(SRC)%.C=$(DEP)%.d)
LIB_HEADS_BARE = $(LIB_HEADS:$(SRC)%=%)
#LIB_HEADS_BARE_MC = 
# The above list is used in the call to rootcint (strip directories off filenames)

# Files not to be included in library (executables, ...)
PROG_SRCS = $(SRC)processSingleFile.C

ALL_PROGS = processDAFile.out processMCFile.out

# Wildcard targets
$(BIN)%.o: $(SRC)%.C
	@echo ">> Compiling $< into $@ ..."
	@if ! [ -d $(BIN) ] ; then mkdir -p $(BIN); fi
	$(CXX) -c $(CXXFLAGS) $< -o $@ -L$(AMSWD)/lib/$(MARCH)/ -l$(AMSNTUPLELIB)
	
$(BIN)MC/%.o: $(SRC)%.C
	@echo ">> Compiling $< into $@ ..."
	@if ! [ -d $(BIN)MC/ ] ; then mkdir -p $(BIN)MC/; fi
	$(CXX) -c -D_IS_MC_ $(CXXFLAGS) $< -o $@ -L$(AMSWD)/lib/$(MARCH)/ -l$(AMSNTUPLELIB)

# %.out: $(SRC)%.C
# 	@echo ">> Making executable $@ ..."
# 	$(CXX) -o $@ $(CXXFLAGS) $< -L$(LIB) -lResTemp_a -L$(AMSWD)/lib/$(MARCH)/ -l$(AMSNTUPLELIB) `root-config --cflags --glibs` -lMinuit -lTMVA -lXMLIO -lMLP -lTreePlayer -lgfortran -lRFIO -lNetx

# Named targets
default:
	@echo "Please choose a specific named target (e.g., all, debug, mc_only, clean, ...)"

all: dirStructure lib prog
	$(MAKE) -C sql

all_clean: clean_lib dirStructure lib prog
	$(MAKE) -C sql all_clean

debug: DEBUGOPT=-g
debug: all

debug_clean: DEBUGOPT=-g
debug_clean: all_clean

#prog: processSingleFile.out
prog: $(ALL_PROGS)

mc_only: lib_MC processMCFile.out

da_only: lib_DA processDAFile.out

# Executable targets
#processSingleFile.out: $(BIN)twrNTupleFiller.o $(BIN)processSingleFile.o  $(LIB)libTWR_DA_a.a 
#	@echo ">> Making executable $@ ..."
#	$(CXX) -o $@ $^ $(CXXFLAGS) -L$(LIB) -lResTemp_a -L$(AMSWD)/lib/$(MARCH)/ -l$(AMSNTUPLELIB) `root-config --cflags --glibs` -lMinuit -lTMVA -lXMLIO -lMLP -lTreePlayer -lgfortran -lRFIO -lNetx

processSingleFile.out: | processDAFile.out
	ln -s processDAFile.out $@

processDAFile.out: $(BIN)twrNTupleFiller.o $(BIN)processSingleFile.o  $(LIB)libTWR_DA_a.a 
	@echo ">> Making executable $@ ..."
	$(CXX) -o $@ $^ $(CXXFLAGS) -L$(LIB) -lTWR_DA_a -L$(AMSWD)/lib/$(MARCH)/ -l$(AMSNTUPLELIB) `root-config --cflags --glibs` -lMinuit -lTMVA -lXMLIO -lMLP -lTreePlayer -lgfortran -lRFIO -lNetx

processMCFile.out: $(BIN)MC/twrNTupleFiller.o $(BIN)MC/processSingleFile.o  $(LIB)libTWR_MC_a.a 
	@echo ">> Making executable $@ ..."
	$(CXX) -o $@ $^ -D_IS_MC_ $(CXXFLAGS) -L$(LIB) -lTWR_MC_a -L$(AMSWD)/lib/$(MARCH)/ -l$(AMSNTUPLELIB) `root-config --cflags --glibs` -lMinuit -lTMVA -lXMLIO -lMLP -lTreePlayer -lgfortran -lRFIO -lNetx

# Directory structure target
dirStructure:
	@if ! [ -d $(BIN) ] ; then mkdir -p $(BIN); fi
	@if ! [ -d $(BIN)MC/ ] ; then mkdir -p $(BIN); fi
	@if ! [ -d $(LIB) ] ; then mkdir -p $(LIB); fi

# Libraries
lib: $(ALL_LIBS)

lib_DA: $(LIB)libTWR_DA.so $(LIB)libTWR_DA_a.a

lib_MC: $(LIB)libTWR_MC.so $(LIB)libTWR_MC_a.a

# Library for DATA runs
$(LIB)libTWR_DA.so:  $(BIN)TWR_DA_Dict.o $(LIB_OBJS)
	@echo ">> Generating shared library $@ ..."
	$(CXX) -shared -o $@ $^ -L$(AMSWD)/lib/$(MARCH)/  `root-config --libs` -lMinuit -lTMVA -lXMLIO -lMLP -lTreePlayer

$(LIB)libTWR_DA_a.a: $(BIN)TWR_DA_Dict.o $(LIB_OBJS)
	@echo ">> Generating static library $@ ..."
	ar -rv $@ $^

$(BIN)TWR_DA_Dict.C: $(LIB_HEADS) $(SRC)linkdef.h
	@echo ">> Using rootcint to generate TWR_DA_Dict.C ..."
	rootcint -f $@ -c -p -I$(SRC) -I$(AMSSRC)/include $(LIB_HEADS_BARE) linkdef.h

$(BIN)TWR_DA_Dict.o: $(BIN)TWR_DA_Dict.C
	@echo ">> Compiling dictionary $< ..."
	$(CXX) -c $(CXXFLAGS) $< -o $@

# Library for MC runs
$(LIB)libTWR_MC.so:  $(BIN)MC/TWR_MC_Dict.o $(LIB_OBJS_MC)
	@echo ">> Generating shared library $@ ..."
	$(CXX) -D_IS_MC_ -shared -o $@ $^ -L$(AMSWD)/lib/$(MARCH)/ `root-config --libs` -lMinuit -lTMVA -lXMLIO -lMLP -lTreePlayer

$(LIB)libTWR_MC_a.a: $(BIN)MC/TWR_MC_Dict.o $(LIB_OBJS_MC)
	@echo ">> Generating static library $@ ..."
	ar -rv $@ $^

$(BIN)MC/TWR_MC_Dict.C: $(LIB_HEADS) $(SRC)linkdef.h
	@echo ">> Using rootcint to generate TWR_MC_Dict.C ..."
	rootcint -f $@ -c -D_IS_MC_ -p -I$(SRC) -I$(AMSSRC)/include $(LIB_HEADS_BARE) linkdef.h

$(BIN)MC/TWR_MC_Dict.o: $(BIN)MC/TWR_MC_Dict.C
	@echo ">> Compiling dictionary $< ..."
	$(CXX) -D_IS_MC_ -c $(CXXFLAGS) $< -o $@

# Old library names -> link to new names for data library
lib_aliases: $(LIB)libResTemp.so $(LIB)libResTemp_a.a

$(LIB)libResTemp.so: | $(LIB)libTWR_DA.so
	ln -s libTWR_DA.so $(LIB)libResTemp.so

$(LIB)libResTemp_a.a: | $(LIB)libTWR_DA_a.a
	ln -s libTWR_DA_a.a $(LIB)libResTemp_a.a


# Target used for testing
test:
	@echo $(LIB_SRCS)
	@echo $(LIB_OBJS)
	@echo $(LIB_HEADS)
	@echo $(LIB_HEADS_BARE)
	@echo $(ALL_PROGS)

.PHONY: all all_clean prog lib lib_DA lib_MC lib_aliases dirStructure test clean clean_lib debug debug_clean

# Cleanup routines
clean:
	@echo ">> Deleting object files and Dict files ..."
	rm -fv $(BIN)*.o
	rm -fv $(BIN)MC/*.o
	rm -fv $(BIN)TWR_DA_Dict.C
	rm -fv $(BIN)TWR_DA_Dict.h
	rm -fv $(BIN)MC/TWR_MC_Dict.C
	rm -fv $(BIN)MC/TWR_MC_Dict.h
	rm -fv $(ALL_PROGS)
	$(MAKE) -C sql clean

clean_lib: clean
	@echo ">> Deleting libraries ..."
	rm -fv $(ALL_LIBS)

