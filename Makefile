# ==========================================
# DNS Sinkhole Makefile
# ==========================================

# Compiler and standard
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iinclude -pthread

# Output executable name
TARGET = build/dns_server

# Find all .cpp files in the src/ directory
SRCS = $(wildcard src/*.cpp)

# Convert .cpp filenames to .o (object) filenames inside the build/ directory
OBJS = $(patsubst src/%.cpp, build/%.o, $(SRCS))

# Default target when you just type 'make'
all: prepare $(TARGET)

# Ensure the build directory exists before compiling
prepare:
	@mkdir -p build

# Link all compiled object files into the final executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Compile each individual .cpp file into an object (.o) file
build/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule to wipe the build directory
clean:
	rm -rf build/*
	@echo "[+] Build directory cleaned."