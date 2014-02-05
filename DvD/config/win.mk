# windows makefile options

TARGET		:= DvD-$(ARCH).exe
CXXFLAGS	:= 
LDFLAGS		:= -lmingw32 -lSDLmain -lws2_32 -lopengl32 -lglew32

ifeq ($(ARCH),32)
	CXX	:= i686-w64-mingw32-g++
	LD	:= i686-w64-mingw32-g++
else
	CXX	:= x86_64-w64-mingw32-g++
	LD	:= x86_64-w64-mingw32-g++
endif
