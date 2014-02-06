#Configuration
include		config.mk

SRC_DIR		:= src
OBJ_DIR		:= obj

SRC_FILES	:= $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES	:= $(SRC_FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

ifeq ($(PLATFORM),w32)
	RES_FILE	:= resource.rc
	OBJ_FILES	+= $(RES_FILE:%.rc=$(OBJ_DIR)/%.o)
endif

.PHONY: all clean

all: $(TARGET)

# Binary
$(TARGET): $(OBJ_FILES)
	@echo LD $@
	@$(LD) -o $@ $(OBJ_FILES) $(LDFLAGS)

# Objects
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo CXX $<
	@$(CXX) -c -o $@ $(CXXFLAGS) $<

$(OBJ_DIR)/%.o: $(RES_FILE) | $(OBJ_DIR)
	@echo WINDRES $< -o $@
	@$(WINDRES) $< $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)
	rm -rf DvD.data DvD.js DvD.html DvD.html.map

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)
