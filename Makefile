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
	@./build/vctest/execvctest -v

test: all
	@./build/vctest/execvctest -v
