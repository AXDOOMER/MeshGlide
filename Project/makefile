# Makefile for MeshGlide

SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)
DEP = $(OBJ:.o=.d)	# One dependency file for each source

CXXFLAGS = -Wall -Wextra -std=c++14 -O2 -pipe
LDFLAGS = -lstdc++ -lm -lglfw -lGL -lGLU -lglut -lSDL2 -lSDL2_image

TARGET = MeshGlide

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

.PHONY: cleandep
cleandep:
	$(RM) $(DEP)
