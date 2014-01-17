# emscripten makefile options

TARGET			:= DvD.html
EMSCRIPTEN_DIR		:= /home/mathew/emscripten

CXXFLAGS		:= -DNO_ZLIB -DNO_NETWORK
LDFLAGS			:= --preload-file ./res@/ -s LEGACY_GL_EMULATION=1 -s TOTAL_MEMORY=268435456

ifeq ($(CFG),release)
	CXXFLAGS	+= -O2
	LDFLAGS		+= -O2 -s ASM_JS=1 --closure 1
endif

ifeq ($(COMPRESS),1)
	LDFLAGS		+= --compression $(EMSCRIPTEN_DIR)/third_party/lzma.js/lzma-native,$(EMSCRIPTEN_DIR)/third_party/lzma.js/lzma-decoder.js,LZMA.decompress
endif
