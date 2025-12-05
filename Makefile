# Makefile for Restaurant Delivery System
# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
EXECUTABLE = main

# Default target
all: $(EXECUTABLE)

# Build the executable from main.cpp
$(EXECUTABLE): main.cpp
	@echo "Compiling main.cpp..."
	$(CXX) $(CXXFLAGS) main.cpp -o $(EXECUTABLE)
	@echo "Build complete!"

# Run the program
run: $(EXECUTABLE)
	./$(EXECUTABLE)

# Clean build artifacts
clean:
	rm -f $(EXECUTABLE)

# Clean and rebuild
rebuild: clean all

# Declare phony targets
.PHONY: all run clean rebuild
