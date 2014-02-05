# linux makefile options

TARGET		:= sprtool-$(ARCH)
CXXFLAGS	:= -m$(ARCH) -pthread
LDFLAGS		:= -m$(ARCH) -pthread -lGL -lGLEW

CXX		:= g++
LD		:= g++
