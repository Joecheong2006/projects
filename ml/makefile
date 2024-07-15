CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra

SRC = $(wildcard src/*.c)
OBJ = $(SRC:%.c=%.o)
DEP = $(OBJ:%.o=%.d)
OUT = ml

PROCESS_INDEX = -1
TOTAL_FILES = $(words $(SRC))
PROCESS_PERCENTAGE = 0

all: $(OBJ)
	@$(CC) $(CFLAGS) $(OBJ) -o $(OUT) -lm
	@echo "Compilation completed"

run:
	@echo "Run Start"
	@./$(OUT)

clean:
	@rm $(OBJ) $(DEP) $(OUT)

%.o: %.c
	$(eval PROCESS_INDEX := $(shell expr $(PROCESS_INDEX) + 1))
	$(eval PROCESS_PERCENTAGE := $(shell echo "$$(($(PROCESS_INDEX) * 100))"))
	$(eval PROCESS_PERCENTAGE := $(shell echo "$$(($(PROCESS_PERCENTAGE) / $(TOTAL_FILES)))"))
	@echo "[$(PROCESS_PERCENTAGE)%] $<"
	@$(CC) $(CFLAGS) $(INCLUDE) -c $< -MMD -o $@

-include $(DEP)
