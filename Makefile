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

# test-debug: debug
# 	@./build/test/mycc_test -v
#
# test: all
# 	@./build/test/mycc_test -v
