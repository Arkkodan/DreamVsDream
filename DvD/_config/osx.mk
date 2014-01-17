# osx config options

TARGET		:= DvD-$(ARCH)
CXXFLAGS	:= -m$(ARCH) -pthread
LDFLAGS		:= -m$(ARCH) -pthread -framework OpenGL -lGLEW

ifeq ($(CFG),release)
	CXXFLAGS	+= -O3
	LDFLAGS		+= -O3
else
	CXXFLAGS	+= -g
	LDFLAGS		+= -g
endif
