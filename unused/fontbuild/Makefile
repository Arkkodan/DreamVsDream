#Configuration
include		config.mk

SRC_DIR		:= src
OBJ_DIR		:= obj

SRC_FILES	:= $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES	:= $(SRC_FILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

.PHONY: all clean

all: $(TARGET)

# Binary
$(TARGET): $(OBJ_FILES)
	@echo LD $@
	@$(LD) -o $@ $(OBJ_FILES) $(LDFLAGS)

# Objects
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@echo CC $<
	@$(CC) -c -o $@ $(CFLAGS) $<

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)
