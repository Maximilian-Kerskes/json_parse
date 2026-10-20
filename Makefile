CXX      ?= c++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -Iinclude

BUILD_DIR := build
TEST_TARGET := $(BUILD_DIR)/json_tests
TEST_SOURCE := tests/test_suite.cpp
EXAMPLE_SOURCES := $(wildcard examples/*.cpp)
EXAMPLE_TARGETS := $(patsubst examples/%.cpp,$(BUILD_DIR)/examples/%,$(EXAMPLE_SOURCES))

.PHONY: all test examples clean

all: test

$(TEST_TARGET): $(TEST_SOURCE) $(wildcard include/*.hpp)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(TEST_SOURCE) -o $(TEST_TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

examples: $(EXAMPLE_TARGETS)
	@for example in $(EXAMPLE_TARGETS); do \
		echo "Running $$example"; \
		./$$example; \
	done

$(BUILD_DIR)/examples/%: examples/%.cpp $(wildcard include/*.hpp)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -rf $(BUILD_DIR)
