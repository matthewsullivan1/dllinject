# Define the C++ compiler and flags
CXX := g++
CXXFLAGS := -std=c++11 -Wall -static-libstdc++ -static-libgcc -static

# Define the output executable and source files
TARGET := injector.exe
SOURCES := src/main.cpp

# Define the manifest file
MANIFEST := injector.manifest

# Define the mt.exe command
MT := mt.exe

# Default build target
all: $(TARGET)

# Rule to compile the source files into the target executable
$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo Embedding manifest...
	$(MT) -manifest $(MANIFEST) -outputresource:$@;#1

# Clean up build artifacts
clean:
	rm -f $(TARGET) *.o
