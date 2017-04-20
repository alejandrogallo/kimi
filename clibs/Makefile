
CXX ?= g++

libint_DIR = libint/
eigenlib_DIR = eigen/

all: libint eigenlib
clean: clean_libint clean_eigenlib

libint: eigenlib
	cd $(libint_DIR)                                               && \
	./autogen.sh                                                   && \
	./configure --prefix=$(PWD) CXX=$(CXX)                         && \
	make                                                           && \
	make install

clean_libint:
	cd $(libint_DIR)                                               && \
	{ test -x configure || ( echo "Libint not built"; exit 1; ) }  && \
	make uninstall

eigenlib:
	{ test -d build/eigen && rm -rf build/eigen || echo "" ; }     && \
	mkdir -p build/eigen                                           && \
	cd build/eigen                                                 && \
	cmake ../../eigen -DCMAKE_INSTALL_PREFIX=$(PWD)                && \
	make install

clean_eigenlib:
	{ test -d build/eigen && cd build/eigen; }                     && \
	{ test -f Makefile  || ( echo "Eigen  not built"; exit 1; ) }  && \
	make uninstall
