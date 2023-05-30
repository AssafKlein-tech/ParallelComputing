# Compiler
CC = g++
# Compiler flags
CFLAGS = -std=c++11 -O3

# Executable names
EXECUTABLE1 = BoundedFIFO
EXECUTABLE2 = SBoundedFIFO
EXECUTABLE3 = SUnboundedFIFO

# Source file names
SOURCE1 = BoundedFIFO.cpp
SOURCE2 = SBoundedFIFO.cpp
SOURCE3 = SUnboundedFIFO.cpp

# Object file names
OBJECTS1 = $(SOURCE1:.cpp=.o)
OBJECTS2 = $(SOURCE2:.cpp=.o)
OBJECTS3 = $(SOURCE3:.cpp=.o)

# Build targets
all: $(EXECUTABLE1) $(EXECUTABLE2) $(EXECUTABLE3)

$(EXECUTABLE1): $(OBJECTS1)
	$(CC) $(CFLAGS) -o $@ $^

$(EXECUTABLE2): $(OBJECTS2)
	$(CC) $(CFLAGS) -o $@ $^

$(EXECUTABLE3): $(OBJECTS3)
	$(CC) $(CFLAGS) -o $@ $^

# Object file targets
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target
clean:
	rm -f $(OBJECTS1) $(OBJECTS2) $(OBJECTS3) $(EXECUTABLE1) $(EXECUTABLE2) $(EXECUTABLE3)
