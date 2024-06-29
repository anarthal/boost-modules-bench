export LLVM_PREFIX:=$(shell brew --prefix llvm@18)

export LDFLAGS:=-L${LLVM_PREFIX}/lib/c++ ${LDFLAGS}
export PATH:=${LLVM_PREFIX}/bin:${PATH}
export CXXFLAGS:=-std=c++23
export CXX:=clang++
export CC:=clang-18

.PHONY: all build install clean distclean
all: stagedir build
	ninja -C build
	cd build && ctest --verbose

build: stagedir CMakeLists.txt
	cmake -G Ninja -S . -B build -D CMAKE_PREFIX_PATH=stagedir #XXX --fresh #XXX --debug-find-pkg=Asio

stagedir: asio-module/CMakeLists.txt GNUmakefile
	cmake -G Ninja -S asio-module -B build/asio -D CMAKE_INSTALL_PREFIX=$(CURDIR)/stagedir --fresh
	cmake --build build/asio
	cmake --install build/asio # --prefix stagedir

clean:
	rm -rf build

distclean: clean
	rm -rf stagedir __build
