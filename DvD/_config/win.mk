# windows makefile options

TARGET		:= DvD-$(ARCH).exe
CXXFLAGS	:= -m$(ARCH)
LDFLAGS		:= -m$(ARCH) -lmingw32 -lSDLmain -lws2_32 -lOpenGL32 -lGLEW32

ifeq ($(CFG),release)
	CXXFLAGS	+= -O3
	LDFLAGS		+= -O3
else
	CXXFLAGS	+= -g
	LDFLAGS		+= -g
endif
