# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++20 

# Target executable
TARGET = main

# Source files
SRCS = \
	src/main.cpp \
	src/monte_carlo_engine.cpp \
	src/variance_reduction_module.cpp \
	src/cache_aware_module.cpp\
	src/black_scholes_model.h\
	src/imgui_setup

# Object files
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Link step
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

# Compile step
src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Cleanup
clean:
	rm -f $(OBJS) $(TARGET) *.stackdump

deepclean: clean
	rm -f *~
