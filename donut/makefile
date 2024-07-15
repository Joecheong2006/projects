CC = gcc
CFLAGS = -std=c11 -Wall -Wextra

SRC = $(wildcard src/*.c)
OBJ = $(SRC:%.c=%.o)
DEP = $(OBJ:%.o=%.d)
OUT = donut

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
	@rm $(OBJ) $(DEP) $(OUT).exe

%.o: %.c
	$(eval PROCESS_INDEX := $(shell expr $(PROCESS_INDEX) + 1))
	$(eval PROCESS_PERCENTAGE := $(shell echo "$$(($(PROCESS_INDEX) * 100))"))
	$(eval PROCESS_PERCENTAGE := $(shell echo "$$(($(PROCESS_PERCENTAGE) / $(TOTAL_FILES)))"))
	@echo "[$(PROCESS_PERCENTAGE)%] $<"
	@$(CC) $(CFLAGS) $(INCLUDE) -c $< -MMD -o $@

-include $(DEP)
