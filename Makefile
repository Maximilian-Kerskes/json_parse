CXX      ?= c++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -Iinclude

BUILD_DIR := build
TEST_TARGET := $(BUILD_DIR)/json_tests
TEST_SOURCE := tests/test_suite.cpp

.PHONY: all test clean

all: test

$(TEST_TARGET): $(TEST_SOURCE) $(wildcard include/*.hpp)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(TEST_SOURCE) -o $(TEST_TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -rf $(BUILD_DIR)
