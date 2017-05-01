EXECUTABLE = $(BINDIR)/kimi

CFLAGS_CLIBS = -I$(PWD)/clibs/include
LDFLAGS_CLIBS = -L$(PWD)/clibs/lib -L$(PWD)/clibs/lib64
CFLAGS_libeigen = -I$(PWD)/clibs/include/eigen3/
LDFLAGS_libint = -lint2
CFLAGS_libint = -I$(PWD)/clibs/include/libint2

INCLUDE_FLAGS= \
-I$(PWD) \
-I$(PWD)/src \
$(CFLAGS_CLIBS) \
$(CFLAGS_libeigen) \

CXXFLAGS = \
$(MACROS) \
$(INCLUDE_FLAGS) \

CFLAGS = \
$(MACROS) \
$(INCLUDE_FLAGS)

LD_FLAGS = \
$(LDFLAGS_CLIBS) \
$(LDFLAGS_libint) \

CFILES = \
clibs/inih/ini.c \

CXXFILES = \
$(wildcard $(SRCDIR)/*.cxx) \
clibs/inih/cpp/INIReader.cpp \

CXX = g++
CC = cc

CXXFLAGS += -std=c++11
