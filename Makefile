.DEFAULT_GOAL := build

BUILD_DIR ?= build
CMAKE ?= cmake

.PHONY: configure build test package install clean core-test

configure:
	$(CMAKE) -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr

build: configure
	$(CMAKE) --build $(BUILD_DIR) --parallel

test: build
	ctest --test-dir $(BUILD_DIR) --output-on-failure

package: build
	cd $(BUILD_DIR) && cpack

install: build
	$(CMAKE) --install $(BUILD_DIR)

core-test:
	./tests/run-core-tests.sh

clean:
	$(CMAKE) -E remove_directory $(BUILD_DIR)
