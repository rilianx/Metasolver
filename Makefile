BUILD_DIR := build/
BUILD_TYPE := Release
TEST_LOG_FILE := test.log

NUMBER_OF_THREADS := $$(expr `nproc --all` / 2)

.PHONY: build test clean

build:
	cmake . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
	cmake --build $(BUILD_DIR) -- -j $(NUMBER_OF_THREADS)

test: build
	ctest --test-dir $(BUILD_DIR) -j $(NUMBER_OF_THREADS) --output-log $(TEST_LOG_FILE) --output-on-failure

clean:
	rm -rf $(BUILD_DIR) .cache/ *.log
