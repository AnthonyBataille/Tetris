CXX = g++

PROJDIR := $(realpath $(CURDIR)/..)
SRCDIR := $(PROJDIR)/src
BUILDDIR := $(PROJDIR)/build
HEADERSDIR :=$(SRCDIR)/headers

VERBOSE = TRUE

SDL_LIB := -lSDL2 -lSDL2_ttf -lSDL2_mixer
SDL_INCLUDE := -I/usr/include/SDL2 -D_REENTRANT
INCLUDE := -I$(HEADERSDIR)


CXXFLAGS := -Wall -O2 -c -std=c++11 $(SDL_INCLUDE) $(INCLUDE)
LDFLAGS := $(SDL_LIB)

VPATH = $(SRCDIR)

EXE = Tetris
SRC = $(wildcard $(SRCDIR)/*.cpp)
OBJ = $(subst $(SRCDIR), $(BUILDDIR), $(SRC:.cpp=.o))

all: $(EXE)

$(EXE): $(OBJ)
	$(CXX) $^ $(LDFLAGS) -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@


.PHONY:
	clean

clean:
	rm $(BUILDDIR)/*.o

mrproper:
	rm $(EXE)
