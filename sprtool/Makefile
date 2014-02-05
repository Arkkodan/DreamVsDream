#Configuration
include		config.mk

SRCPATH		:= src
OBJPATH		:= obj

DVDSRCPATH	:= ../DvD/src

CXXFILES	:= init input main
DVDCXXFILES	:= sprite parser error fighter os util graphics image

CXXSRC		:= $(addprefix $(SRCPATH)/, $(addsuffix .cpp, $(CXXFILES))) $(addprefix $(DVDSRCPATH)/, $(addsuffix .cpp, $(DVDCXXFILES)))
OBJ		:= $(addprefix $(OBJPATH)/, $(addsuffix .o, $(CXXFILES) $(DVDCXXFILES)))

.PHONY: all clean

all: $(TARGET)

# Binary
$(TARGET): $(OBJ)
	@echo LD $@
	@$(LD) -o $@ $(OBJ) $(LDFLAGS)

# Objects
$(OBJPATH)/%.o: $(SRCPATH)/%.cpp | $(OBJPATH)
	@echo CXX $<
	@$(CXX) -c -o $@ $(CXXFLAGS) $<

$(OBJPATH)/%.o: $(DVDSRCPATH)/%.cpp | $(OBJPATH)
	@echo CXX $<
	@$(CXX) -c -o $@ $(CXXFLAGS) $<

clean:
	rm -rf $(OBJPATH) $(TARGET)
	rm -rf DvD.data DvD.js DvD.html DvD.html.map

$(OBJPATH):
	@mkdir -p $(OBJPATH)
