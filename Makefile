CC := gcc

CFLAGS := -Wall -Wextra -Wpedantic
CFLAGS += -std=c17 -g3 -O0
CFLAGS += -Iinclude
CFLAGS += -MMD -MP

SRC_DIR := src
OBJ_DIR := obj
TARGET := app

SRC_C := $(shell find $(SRC_DIR) -type f -name '*.c')
OBJ_C := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_C))
DEP_FILES := $(OBJ_C:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJ_C)
	$(CC) $^ -o $@


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean

-include $(DEP_FILES)