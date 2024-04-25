CC = g++
CFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -Wstrict-aliasing -g3 -DDEBUG -O2
SOURCE = src src/**
INCLUDE = -Ilib/ -Ilib/mfw/lib

SRC = $(wildcard $(patsubst %, %/*.cpp, $(SOURCE)))
LIBOBJ = $(wildcard $(patsubst %, %/*.o, $(LIBRARY))) 
OBJ = $(SRC:%.cpp=%.o)
DEP = $(OBJ:%.o=%.d)
BIN = bin
LIB = $(BIN)/mfw.dll
OUT = $(BIN)/rigid2d.exe

PROCESS_INDEX = -1
TOTAL_FILES = $(words $(SRC))
PROCESS_PERCENTAGE = 0

.PHONY: all clean

all: bin app

run:
	@./$(OUT)

debug:
	@gdb -q $(OUT) --eval-command=run --eval-command=exit

clean:
	@rm -rf $(OBJ) $(DEP) $(BIN)
	@echo Cleaned

build:
	@echo "Compiling library"
	@echo "Completed"

bin:
	@mkdir -p $(BIN)
	@cp lib/mfw/mfw.dll $(BIN)/mfw.dll

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
