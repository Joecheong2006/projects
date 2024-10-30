CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra
DFLAGS =

SRC = $(wildcard src/*.c)
OBJ = $(SRC:%.c=%.o)
DEP = $(OBJ:%.o=%.d)
OUT = typing

all: $(OBJ)
	@$(CC) $(CFLAGS) $(DFLAGS) $(OBJ) -o $(OUT)
	@echo "Compilation completed"

run:
	@echo "Run Start"
	@./$(OUT)

clean:
	@rm $(OBJ) $(DEP) $(OUT).exe

%.o: %.c
	$(CC) $(CFLAGS) $(DFLAGS) $(INCLUDE) -c $< -MMD -o $@

-include $(DEP)
