CXX = g++

CXXFLAGS = -Wall -Wextra -std=c++17 -MMD -MP

TARGET = main

OBJ = main.o math_utils.o

DEP = $(OBJ:.o=.d)

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(DEP)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) *.o *.d