CC = g++
CFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -Wstrict-aliasing -g -O2
SOURCE = src src/**
INCLUDE = -I../lib -I../include
LIB = bin/mfw.dll -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic

SRC = $(wildcard $(patsubst %, %/*.cpp, $(SOURCE)))
OBJ = $(SRC:%.cpp=%.o)
DEP = $(OBJ:%.o=%.d)
BIN = bin

PROGRAM = demo
OUT = $(BIN)/$(PROGRAM)

ifeq ($(OS), Windows_NT)
	OUT = $(BIN)/$(PROGRAM).exe
endif

.PHONY: all clean

all: bin app

run:
	@./$(OUT)

debug:
	@gdb -q $(OUT) --eval-command=run --eval-command=exit

clean:
	@rm -rf $(OBJ) $(DEP) $(OUT)
	@echo Cleaned

build: bin
	@cd ../ && make win_debug -j
	@cp ../mfw.dll bin/mfw.dll

bin:
	@mkdir -p $(BIN)

app: bin $(OBJ) 
	$(CC) -o $(OUT) $(OBJ) $(LIB)
	@echo "[100%] Compilation completed"

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $< -MMD

-include $(DEP)
