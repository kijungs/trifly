# Dir setting
HOME			= .
INCLUDE 		= $(HOME)/include
SRC 			= $(HOME)/src
TESTDIR 		= $(HOME)/test
BIN			= $(HOME)/bin

# Custom
TARGET 			= $(BIN)/mpi
TEST			= $(BIN)/mpi_test

# Sources
SOURCES			= $(wildcard $(SRC)/*.cpp)
SOURCES_TEST		= $(wildcard $(TESTDIR)/*.cpp)
TEST_HEADER		= $(INCLUDE)/catch.hpp

#OBJECTS			= $(patsubst %.cpp, %.o, $(SOURCES))
OBJECTS			= $(patsubst %.cpp, %.o, $(SOURCES))
OBJECTS_TEST		= $(patsubst %.cpp, %.test.o, $(filter-out $(SRC)/main.cpp, $(SOURCES))) \
				  $(patsubst %.cpp, %.test.o, $(SOURCES_TEST))

# Compiler
ifeq (, $(shell which mpic++))
CXX			= g++
EXTRAFLAGS		=
else 
CXX			= mpic++
EXTRAFLAGS		= -D_MPI
endif

CXXFLAGS		= -std=c++0x -g -Wall -fmessage-length=0 -O3 $(EXTRAFLAGS)
LDFLAGS			= -static-libstdc++
LDLIBS			= 
RM				= rm -rf

## Binaries
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.test.o: %.cpp $(TEST_HEADER) 
	$(CXX) $(CXXFLAGS) $(LDFALGS) -I$(INCLUDE) -D_TEST_ -c -o $@ $<

$(TEST): $(OBJECTS_TEST) $(TEST_HEADER)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(OBJECTS_TEST) $(LDLIBS)

# External library download
$(TEST_HEADER):
	wget -O $(INCLUDE)/catch.hpp https://raw.githubusercontent.com/philsquared/Catch/master/single_include/catch.hpp
	touch $@

# Check
check: MPI-exists
MPI-exists: ; @which $(MPICXX) > /dev/null

# Public rules
all: $(TARGET)

demo: $(TARGET)
	mpirun -n 31 $(TARGET) --trial 3 --budget 100000 example_graph.txt output

dist: $(TARGET)
	mpiexec -f machinefile -n 31 $(TARGET)

clean:
	$(RM) $(OBJECTS) $(filter-out $(TESTDIR)/test_main.test.o, $(OBJECTS_TEST)) $(TARGET) $(TEST)

clear:
	$(RM) $(OBJECTS) $(filter-out $(TESTDIR)/test_main.test.o, $(OBJECTS_TEST))

.PHONY: all test clean check MPI-exists

