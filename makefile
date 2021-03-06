CXX = g++
CFLAGS = -g -std=c++14 -Wall -pedantic
LIBS = -L/usr/lib/x86_64-linux-gnu -lgsl -lgslcblas -lfftw3_threads -lfftw3 -lm -lpthread -l stdc++
SDIR = src/cpp
BDIR = bin
ODIR = obj
DDIR = data
INC = -I/usr/include

# these are the object file names (targets for compilation step)
# second line prepends the obj directory to object file names
_OBJ = array2d.o generators.o util.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

.PHONY: default test all directories remove clean
default: $(BDIR)/main.exe
test: $(BDIR)/test.exe
all: directories default test

# create the necessary directories
directories:
	mkdir -p $(ODIR)
	mkdir -p $(BDIR)
	mkdir -p $(DDIR)

# compile the objects
$(ODIR)/%.o: $(SDIR)/%.cpp
	$(CXX) $(CFLAGS) -c -o $@ $<

# link to executable
$(BDIR)/%.exe: $(ODIR)/%.o $(OBJ)
	$(CXX) $(CFLAGS) $^ $(LIBS) -o $@

remove:
	rm -f $(ODIR)/*.o
	rm -f $(BDIR)/*

clean:
	rm -f $(ODIR)/*.o
