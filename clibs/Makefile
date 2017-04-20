
CXX ?= g++

libint_DIR = libint/
eigenlib_DIR = eigen/

libint:
	cd $(libint_DIR);                         \
	bash autoconf.sh &&                       \
	./configure --prefix=$(PWD) CXX=$(CXX) && \
	make &&                                   \
	make install

eigenlib:
	{ test -d build/eigen && rm -rf build/eigen || echo "No build found"; } &&  \
	mkdir -p build/eigen && cd build/eigen           &&     \
	cmake ../../eigen -DCMAKE_INSTALL_PREFIX=$(PWD)  &&     \
	make install

