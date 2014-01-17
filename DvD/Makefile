#Configuration
include		config.mk

SRCPATH		:= src
OBJPATH		:= obj

CXXFILES	:= main atlas error fighter file font graphics image init input menu network os parser player shader sound sprite stage util

CXXSRC		:= $(addprefix $(SRCPATH)/, $(addsuffix .cpp, $(CXXFILES)))
CSRC		:= $(addprefix $(SRCPATH)/, $(addsuffix .c, $(CFILES)))
OBJ		:= $(addprefix $(OBJPATH)/, $(addsuffix .o, $(CXXFILES) $(CFILES)))

.PHONY: all clean checkdirs

all: checkdirs $(TARGET)

# Binary
$(TARGET): $(OBJ)
	@echo LD $@
	@$(LD) -o $@ $(OBJ) $(LDFLAGS)

# Objects
$(OBJPATH)/%.o: $(SRCPATH)/%.cpp
	@echo CXX $<
	@$(CXX) -c -o $@ $(CXXFLAGS) $<

$(OBJPATH)/%.o: $(SRCPATH)/%.c
	@echo CC $<
	@$(CC) -c -o $@ $(CFLAGS) $<

clean:
	rm -rf $(OBJPATH) $(TARGET)
	rm -rf DvD.data DvD.js DvD.html DvD.html.map

checkdirs:
	@echo "making directories"
	@echo $(CSRC)
	@mkdir -p $(OBJPATH)
