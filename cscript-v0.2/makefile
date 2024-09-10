CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Wpedantic -Wstrict-aliasing -Werror -Isrc
DEBUG_FLAGS = -g3 -DDEBUG
RELEASE_FLAGS = -O2
DEBUGGER = gdb

CONFIG=

WINLIB = -lWinmm

SRC = $(wildcard src/*.c src/**/*.c)
OBJ = $(SRC:%.c=%.o)
DEP = $(OBJ:%.o=%.d)
BIN = bin
LIB = 
PROGRAM = script

ifeq ($(OS),Windows_NT)
	OUT = $(PROGRAM).exe
	LIB += $(WINLIB)
else
	OUT = $(PROGRAM)
endif


all: bin app 

rel: CFLAGS += $(RELEASE_FLAGS) $(CONFIG)
rel: bin $(OBJ)
	@$(CC) $(CFLAGS) $(OBJ) -o $(BIN)/$(OUT) $(LIB)
	@echo "Compilation completed"

bin:
	@mkdir -p $(BIN)

app: CFLAGS += $(DEBUG_FLAGS) $(CONFIG)
app: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LIB) -o $(BIN)/$(OUT)
	@echo "Compilation completed"

debug:
	@$(DEBUGGER) -q $(BIN)/$(OUT) --eval-command=run --eval-command=exit

check:
	@valgrind --leak-check=yes $(BIN)/$(OUT)

run:
	@./$(BIN)/$(OUT) 

clean:
	@rm -rf $(OBJ) $(DEP) $(BIN)
	@echo Cleaned

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -MMD -o $@

-include $(DEP)
