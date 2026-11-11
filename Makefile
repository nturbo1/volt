.PHONY: all debug clean test test-debug

all:
	@mkdir -p build
	@cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
	@$(MAKE) -C build

debug:
	@mkdir -p build
	@cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
	@$(MAKE) -C build

clean:
	@rm -rf build

test-debug: debug
	@./build/exectests

test: all
	@./build/exectests
	# @cd ./build && ctest --rerun-failed --output-on-failure
