
SQL_FLAGS=`mysql_config --cflags --libs`

# Files not to be included in library (executables, ...)
PROG_SRCS = sqlRunList.C

PROGS = $(PROG_SRCS:%.C=%.out)

%.out: %.C
	@echo ">> Making executable $@ ..."
	$(CXX) -g -o $@ $(SQL_FLAGS) $<

# Named targets
all: sqlRunList.out

all_clean: clean all

prog: $(PROGS)

test:
	@echo $(PROGS)

.PHONY: all all_clean clean

# Cleanup routines
clean:
	@echo ">> Deleting compiled executables ..."
	rm -fv $(PROGS)
