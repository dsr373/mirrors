CXX = g++
CFLAGS = -g -std=c++14 -Wall -pedantic
LIBS = -L/usr/lib/x86_64-linux-gnu -lgsl -lgslcblas -lfftw3 -lm -l stdc++
SDIR = src/cpp
BDIR = bin
ODIR = obj
DDIR = data
INC = -I/usr/include

# these are the object file names (targets for compilation step)
# second line prepends the obj directory to object file names
_OBJ = main.o array2d.o util.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

.PHONY: default directories remove clean
default: $(BDIR)/main.exe

# create the necessary directories
directories:
	mkdir -p $(ODIR)
	mkdir -p $(BDIR)
	mkdir -p $(DDIR)

# compile the objects
$(ODIR)/%.o: $(SDIR)/%.cpp
	$(CXX) $(CFLAGS) -c -o $@ $<

# link to executable
$(BDIR)/main.exe: $(OBJ)
	$(CXX) $(CFLAGS) $^ $(LIBS) -o $@

remove:
	rm -f $(ODIR)/*.o
	rm -f $(BDIR)/*

clean:
	rm -f $(ODIR)/*.o
