CC = gcc
# CFLAGS = -std=c99 -Wall -Wextra -Wpedantic -Wstrict-aliasing -Werror -g3 -DNDEBUG
CFLAGS = -std=c99 -Wall -Wextra -Wpedantic -Wstrict-aliasing -Werror -g3
DEBUGGER = gdb

SRC = $(wildcard src/*.c src/**/*.c)
OBJ = $(SRC:%.c=%.o)
DEP = $(OBJ:%.o=%.d)
OUT = cscript

all: $(OBJ)
	@$(CC) $(CFLAGS) $(OBJ) -o $(OUT)
	@echo "Compilation completed"

debug:
	@$(DEBUGGER) -q $(OUT) --eval-command=run --eval-command=exit

run:
	@./$(OUT)

clean:
	@rm -rf $(OBJ) $(DEP) $(OUT)
	@echo Cleaned

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -MMD -o $@

-include $(DEP)
