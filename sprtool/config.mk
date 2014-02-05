VERSION		:= 0.2
ARCH		:= 64
CFG		:= debug
include		config/win.mk

# flags
CXXFLAGS	+= -DVERSION=\"$(VERSION)\" -DSPRTOOL
LDFLAGS		+= -lSDL -lpng

ifeq ($(CFG),debug)
	CXXFLAGS	+= -DDEBUG -g
	LDFLAGS		+= -g
else
	CXXFLAGS	+= -O3
	LDFLAGS		+= -O3
endif
