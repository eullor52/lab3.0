BUILD_DIR := build
BUILD_TYPE := Release
JOBS := $(shell nproc 2>/dev/null || echo 4)
COMPILER_PREFIX := -G "MinGW Makefiles"

.PHONY: all configure build test bench clean rebuild

all: build

configure:
	cmake -S . -B $(BUILD_DIR) $(COMPILER_PREFIX) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)

build: configure
	cmake --build $(BUILD_DIR) -j$(JOBS)

test: build
	cd $(BUILD_DIR) && ctest --output-on-failure

bench: build
	./$(BUILD_DIR)/benchmarks/benchmark_uniq_ptr
	./$(BUILD_DIR)/benchmarks/benchmark_shared_ptr

clean:
	rm -rf $(BUILD_DIR)

rebuild: clean build