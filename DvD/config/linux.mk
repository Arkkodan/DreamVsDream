# linux makefile options

TARGET		:= DvD-$(ARCH)
CXXFLAGS	:= -m$(ARCH) -pthread
LDFLAGS		:= -m$(ARCH) -pthread -lGL -lGLEW

CXX		:= g++
LD		:= g++
