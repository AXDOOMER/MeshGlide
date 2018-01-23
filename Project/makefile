# Makefile for MeshGlide

SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)
DEP = $(OBJ:.o=.d)	# One dependency file for each source

CXXFLAGS = -Wall -Wextra -std=c++11 -O2 -pipe
LDFLAGS = -lstdc++ -lm -lglfw -lGL -lGLU -lSDL2 -lSDL2_image

TARGET = MeshGlide

all: TARGET

TARGET: $(OBJ)
	$(CXX) -o $(TARGET) $^ $(LDFLAGS)

-include $(DEP)	# Include all dep files in the makefile

# Rule to generate a dep file by using the C preprocessor
# (see man cpp for details on the -MM and -MT options)
%.d: %.c
	@$(CPP) $(CXXFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean
clean:
	$(RM) $(OBJ) $(TARGET)
	$(RM) *.lnb *.mtl

.PHONY: cleandep
cleandep:
	$(RM) $(DEP)
