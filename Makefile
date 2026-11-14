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

test-debug:
	@cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
	@cmake --build build --target exectests
	@./build/tests/exectests

test:
	@cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON
	@cmake --build build --target exectests
	@./build/tests/exectests
	# @cd ./build && ctest --rerun-failed --output-on-failure
