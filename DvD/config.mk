VERSION		:= 0.12
ARCH		:= 64
CFG		:= debug
include		_config/linux.mk

# flags
CXXFLAGS	+= -DVERSION=\"$(VERSION)\" -DGAME
LDFLAGS		+= -lSDL -lpng -lz -lportaudio -lsndfile

ifeq ($(CFG),debug)
	CXXFLAGS	+= -DDEBUG
endif

# compiler and linker
CXX			:= g++
LD			:= g++
