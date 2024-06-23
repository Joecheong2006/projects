CC = gcc
CFLAGS = -std=c99 -Wall -Wextra
LIB = -lm

SRC = $(wildcard src/*.c)
OBJ = $(SRC:%.c=%.o)
DEP = $(OBJ:%.o=%.d)
OUT = todo

PROCESS_INDEX = -1
TOTAL_FILES = $(words $(SRC))
PROCESS_PERCENTAGE = 0

all: $(OBJ)
	@$(CC) $(CFLAGS) $(OBJ) -o $(OUT) $(LIB)
	@echo "Compilation completed"

run:
	@echo "Running"
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
