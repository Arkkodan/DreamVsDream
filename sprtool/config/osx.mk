# osx config options

TARGET		:= sprtool
CXXFLAGS	:= -arch i386 -arch x86_64 -pthread
LDFLAGS		:= -arch i386 -arch x86_64 -pthread -framework OpenGL -lGLEW

CXX		:= g++
LD		:= g++
