CC = g++
PCH = ../include/mfwpch.h
CFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -Wstrict-aliasing -g -include $(PCH) -DDEBUG -DUNICODE
SOURCE = src src/**
INCLUDE = -I../lib -I../include
LIB = -lmingw32 -lopengl32 -lGdi32 -lWinmm -ld3dcompiler -ldwmapi bin/mfw.dll


SRC = $(wildcard $(patsubst %, %/*.cpp, $(SOURCE)))
LIBOBJ = $(wildcard $(patsubst %, %/*.o, $(LIBRARY))) 
OBJ = $(SRC:%.cpp=%.o)
DEP = $(OBJ:%.o=%.d)
BIN = bin
OUT = $(BIN)/demo.exe

PROCESS_INDEX = -1
TOTAL_FILES = $(words $(SRC))
PROCESS_PERCENTAGE = 0

.PHONY: all clean

all: bin app

run:
	@./$(OUT)

debug:
	@gdb -q "$(OUT)" --eval-command=run --eval-command=exit;

clean:
	@rm -rf $(OBJ) $(DEP)
	@echo Cleaned

build: bin
	@cd ../ && make clean -s && make win_release -s
	@cp ../mfw.dll bin/mfw.dll

bin:
	@mkdir -p $(BIN)

app: bin $(OBJ) 
	@$(CC) -o $(OUT) $(OBJ) $(LIBOBJ) $(LIB)
	@echo "[100%] Compilation completed"

%.o: %.cpp
	$(eval PROCESS_INDEX := $(shell expr $(PROCESS_INDEX) + 1))
	$(eval PROCESS_PERCENTAGE := $(shell echo "$$(($(PROCESS_INDEX) * 100))"))
	$(eval PROCESS_PERCENTAGE := $(shell echo "$$(($(PROCESS_PERCENTAGE) / $(TOTAL_FILES)))"))
	@echo "[$(PROCESS_PERCENTAGE)%] $<"
	@$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $< -MMD

-include $(DEP)
