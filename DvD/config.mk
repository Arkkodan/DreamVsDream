NAME		:= DvD
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

ifeq ($(PLATFORM),emscripten)
	TARGET			:= $(NAME).html
	EMSCRIPTEN_DIR		?= /home/mathew/emscripten

	CXXFLAGS		:= -DNO_ZLIB -DNO_NETWORK
	LDFLAGS			:= --preload-file ./res@/ -s LEGACY_GL_EMULATION=1 -s TOTAL_MEMORY=268435456

	ifeq ($(CFG),release)
		CXXFLAGS	+= -O2
		LDFLAGS		+= -O2 -s ASM_JS=1 --closure 1
	endif

	ifeq ($(COMPRESS),1)
		LDFLAGS		+= --compression $(EMSCRIPTEN_DIR)/third_party/lzma.js/lzma-native,$(EMSCRIPTEN_DIR)/third_party/lzma.js/lzma-decoder.js,LZMA.decompress
	endif
else
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
		LDFLAGS		:= -m$(ARCH) -pthread -lGL

		CXX		:= g++
		LD		:= g++
	else ifeq ($(PLATFORM),osx)
		TARGET		:= $(NAME)
		CXXFLAGS	:= -arch i386 -arch x86_64 -pthread
		LDFLAGS		:= -arch i386 -arch x86_64 -pthread -framework OpenGL

		CXX		:= g++
		LD		:= g++
	else ifeq ($(PLATFORM),w32)
		TARGET		:= $(NAME)-$(ARCH).exe
		CXXFLAGS	:= 
		LDFLAGS		:= -mwindows -lmingw32 -lSDL2main -lws2_32 -lopengl32 -static-libstdc++ -static-libgcc

		ifeq ($(ARCH),32)
			CXX	:= i686-w64-mingw32-g++
			LD	:= i686-w64-mingw32-g++
			WINDRES	:= i686-w64-mingw32-windres
		else
			CXX	:= x86_64-w64-mingw32-g++
			LD	:= x86_64-w64-mingw32-g++
			WINDRES	:= x86_64-w64-mingw32-windres
		endif
	endif
endif

# flags
CXXFLAGS	+= -DVERSION=\"$(VERSION)\" -DGAME -std=c++11 -I/opt/local/include
LDFLAGS		+= -lSDL2 -lpng -lz -lsndfile -std=c++11 -L/opt/local/lib

