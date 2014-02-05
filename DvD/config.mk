VERSION		:= 0.2
ARCH		:= 64
CFG		:= debug
include		config/linux.mk

# flags
CXXFLAGS	+= -DVERSION=\"$(VERSION)\" -DGAME
LDFLAGS		+= -lSDL -lpng -lz -lportaudio -lsndfile

ifneq ($(EMSCRIPTEN),1)
ifeq ($(CFG),debug)
	CXXFLAGS	+= -DDEBUG -g
	LDFLAGS		+= -g
else
	CXXFLAGS	+= -O3
	LDFLAGS		+= -O3
endif
endif
