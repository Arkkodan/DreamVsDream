#Configuration
include		config.mk

OBJ_DIR		:= obj

SRC_DIR		:= src
SRC_FILES	:= $(wildcard $(SRC_DIR)/*.cpp)

DVD_SRC_DIR	:= ../DvD/src
DVD_SRC		:= error fighter graphics image os parser sprite util file
DVD_SRC_FILES	:= $(addsuffix .cpp, $(addprefix $(DVD_SRC_DIR)/, $(DVD_SRC)))

OBJ_FILES	:= $(SRC_FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
OBJ_FILES	+= $(DVD_SRC_FILES:$(DVD_SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

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

$(OBJ_DIR)/%.o: $(DVD_SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo CXX $<
	@$(CXX) -c -o $@ $(CXXFLAGS) $<

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)
