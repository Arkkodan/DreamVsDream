NAME		:= sprtool
VERSION		:= 0.2
ARCH		?= 64
CFG		?= release

ifndef PLATFORM
	ifeq ($(OS),Windows_NT)
		PLATFORM := w32
	else
		UNAME_S := $(shell uname -s)
		ifeq ($(UNAME_S),Linux)
			PLATFORM := linux
		else ifeq ($(UNAME_S),Darwin)
			PLATFORM := osx
		endif
	endif
endif

ifeq ($(CFG),debug)
	CXXFLAGS	+= -DDEBUG -g
	LDFLAGS		+= -g
else
	CXXFLAGS	+= -O3
	LDFLAGS		+= -O3
endif

ifeq ($(PLATFORM),linux)
	TARGET		:= $(NAME)-$(ARCH)
	CXXFLAGS	:= -m$(ARCH) -pthread
	LDFLAGS		:= -m$(ARCH) -pthread -lGL -lGLEW

	CXX		:= g++
	LD		:= g++
else ifeq ($(PLATFORM),osx)
	TARGET		:= $(NAME)
	CXXFLAGS	:= -arch i386 -arch x86_64 -pthread
	LDFLAGS		:= -arch i386 -arch x86_64 -pthread -framework OpenGL -lGLEW

	CXX		:= g++
	LD		:= g++
else ifeq ($(PLATFORM),w32)
	TARGET		:= $(NAME)-$(ARCH).exe
	CXXFLAGS	:= 
	LDFLAGS		:= -lmingw32 -lSDLmain -lws2_32 -lopengl32 -lglew32

	ifeq ($(ARCH),32)
		CXX	:= i686-w64-mingw32-g++
		LD	:= i686-w64-mingw32-g++
	else
		CXX	:= x86_64-w64-mingw32-g++
		LD	:= x86_64-w64-mingw32-g++
	endif
endif

# flags
CXXFLAGS	+= -DVERSION=\"$(VERSION)\" -DSPRTOOL
LDFLAGS		+= -lSDL -lpng -lz

