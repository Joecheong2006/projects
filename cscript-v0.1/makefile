CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Wpedantic -Wstrict-aliasing -Werror -g3 -DDEBUG
DEBUGGER = gdb

SRC = $(wildcard src/*.c src/**/*.c)
OBJ = $(SRC:%.c=%.o)
DEP = $(OBJ:%.o=%.d)
BIN = bin
PROGRAM = cscript

ifeq ($(OS),Windows_NT)
	OUT = $(PROGRAM).exe
else
	OUT = $(PROGRAM)
endif


all: bin app

bin:
	@mkdir -p $(BIN)

app: $(OBJ)
	@$(CC) $(CFLAGS) $(OBJ) -o $(BIN)/$(OUT)
	@echo "Compilation completed"

debug:
	@$(DEBUGGER) -q $(OUT) --eval-command=run --eval-command=exit

run:
	@./$(BIN)/$(OUT) 

clean:
	@rm -rf $(OBJ) $(DEP) $(BIN)
	@echo Cleaned

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -MMD -o $@

-include $(DEP)
