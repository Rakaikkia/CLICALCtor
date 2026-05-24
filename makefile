CXX := g++
CXXFLAGS := -Ofast -Wall -Wextra -std=c++23 -pedantic -march=native \
	-I/usr/include -Isrc/lexer -Isrc -Isrc/ast -Isrc/parser
SRC_DIR := src
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
TARGET := $(BUILD_DIR)/main

# find all .cpp sources under src/
SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
# corresponding object files in OBJ_DIR preserving subdirs
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

.PHONY: all clean

all: $(TARGET)

# link
$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@

# compile .cpp -> .o, create directory if needed
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

