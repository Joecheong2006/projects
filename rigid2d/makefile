CC = g++
CFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -Wstrict-aliasing -g -O2 -DDEBUG
SOURCE = src src/**
INCLUDE = -Ilib/ -Ilib/mfw/lib

SRC = $(wildcard $(patsubst %, %/*.cpp, $(SOURCE)))
LIBOBJ = $(wildcard $(patsubst %, %/*.o, $(LIBRARY))) 
OBJ = $(SRC:%.cpp=%.o)
DEP = $(OBJ:%.o=%.d)
BIN = bin
LIB = -Llib/mfw/ -lmfw
OUT = $(BIN)/rigid2d.exe

.PHONY: all clean

all: bin app

run:
	@./$(OUT)

debug:
	@gdb -q $(OUT) --eval-command=run --eval-command=exit

clean:
	@rm -rf $(OBJ) $(DEP) $(BIN)
	@echo Cleaned

bin:
	mkdir -p $(BIN)
	cp lib/mfw/mfw.dll $(BIN)/mfw.dll

app: bin $(OBJ) 
	$(CC) -o $(OUT) $(OBJ) $(LIBOBJ) $(LIB) -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -Wl,-Bdynamic

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $< -MMD

-include $(DEP)
