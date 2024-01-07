# Makefile for MeshGlide
# https://www3.ntu.edu.sg/home/ehchua/programming/cpp/gcc_make.html
# http://nuclear.mutantstargoat.com/articles/make/

SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)
DEP = $(OBJ:.o=.d)	# One dependency file for each source

CXXFLAGS = -Wall -Wextra -std=c++14 -O2 -pipe

ifeq ($(findstring MINGW,$(shell uname -s)),MINGW)
	LDFLAGS = -lSDL2main -lSDL2 -lSDL2_image -lkernel32 -luser32 -lgdi32 -lstdc++ -lm -lglfw3 -lglu32 -lopengl32 -lzmq
else
	LDFLAGS = -lstdc++ -lm -lglfw -lGL -lGLU -lSDL2 -lSDL2_image -lzmq
endif

TARGET = MeshGlide

all: TARGET

MeshGlide: TARGET

TARGET: $(OBJ)
	$(CXX) -o $(TARGET) $^ $(LDFLAGS)

-include $(DEP)	# Include all dep files in the makefile

# Rule to generate a dep file by using the C preprocessor
# (see 'man cpp' for details on the -MM and -MT options)
%.d: %.c
	@$(CPP) $(CXXFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean
clean:
	$(RM) $(OBJ) $(TARGET)
	$(RM) *.lnb *.mtl

.PHONY: cleandep
cleandep:
	$(RM) $(DEP)
