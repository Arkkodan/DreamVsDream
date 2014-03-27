NAME		:= fontbuild
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
	CFLAGS	+= -DDEBUG -g
	LDFLAGS		+= -g
else
	CFLAGS	+= -O3
	LDFLAGS		+= -O3
endif

ifeq ($(PLATFORM),linux)
	TARGET		:= $(NAME)-$(ARCH)
	CFLAGS		:= -m$(ARCH)
	LDFLAGS		:= -m$(ARCH)

	CC		:= clang
	LD		:= clang
else ifeq ($(PLATFORM),osx)
	TARGET	:= $(NAME)
	CFLAGS	:= -mmacosx-version-min=10.6 -isysroot /Developer/SDKs/MacOSX10.6.sdk -arch i386 -arch x86_64
	LDFLAGS		:= -mmacosx-version-min=10.6 -isysroot /Developer/SDKs/MacOSX10.6.sdk -arch i386 -arch x86_64

	CC		:= /opt/clang/bin/clang
	LD		:= /opt/clang/bin/clang
else ifeq ($(PLATFORM),w32)
	TARGET		:= $(NAME)-$(ARCH).exe
	CFLAGS		:= 
	LDFLAGS		:= 

	ifeq ($(ARCH),32)
		CC	:= i686-w64-mingw32-gcc
		LD	:= i686-w64-mingw32-gcc
		WINDRES	:= i686-w64-mingw32-windres
	else
		CC	:= x86_64-w64-mingw32-gcc
		LD	:= x86_64-w64-mingw32-gcc
		WINDRES	:= x86_64-w64-mingw32-windres
	endif
endif

# flags
CFLAGS		+= -DVERSION=\"$(VERSION)\" -DCOMPILER -DNO_ZLIB -std=c99
LDFLAGS		+= -std=c99

