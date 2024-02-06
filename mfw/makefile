CC = g++
PCH = src/mfwpch.h
CFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -Wstrict-aliasing -g -lm -include $(PCH)
DFLAGS := -DDEBUG
RFLAGS := -O3 -march=native -flto -fdefer-pop -fmerge-constants -fthread-jumps -floop-optimize -fif-conversion2 -fif-conversion -fcrossjumping -fguess-branch-probability -fcprop-registers -foptimize-sibling-calls -fstrength-reduce -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop -frerun-loop-opt -fgcse -fgcse-lm -fgcse-sm -fdelete-null-pointer-checks -fexpensive-optimizations -fregmove -fschedule-insns -fschedule-insns2 -fsched-interblock -fsched-spec -fcaller-saves -fpeephole2 -freorder-blocks -freorder-functions -fstrict-aliasing -falign-functions -falign-jumps -falign-loops -falign-labels -finline-functions -frename-registers -DNDEBUG
SOURCE = src src/**
LIBRARY = lib/stb_image lib/glew
INCLUDE = -Ilib -Ilib/glew -Ilib/stb_image -Isrc
LIB = -lopengl32 -lUser32 -lGdi32 -lGlu32 -lShell32


SRC = $(wildcard $(patsubst %, %/*.cpp, $(SOURCE)))
LIBOBJ = $(wildcard $(patsubst %, %/*.o, $(LIBRARY))) 
OBJ = $(SRC:%.cpp=%.o)
DEP = $(OBJ:%.o=%.d)
BIN = bin
OUT = $(BIN)/mfw_demo.exe

PROCESS_INDEX = -1
TOTAL_FILES = $(words $(SRC))
PROCESS_PERCENTAGE = 0

.PHONY: all clean

all: bin app

run:
	@./$(OUT)

clean:
	@rm -rf $(OBJ) $(DEP) $(BIN)
	@echo Cleaned

build:
	@echo "Compiling library"
	@cd lib/stb_image && $(CC) -o stb_image.o -c stb_image.cpp
	@cd lib/glew && gcc -c glew.c -o glew.o
	@$(CC) -x c++-header $(INCLUDE) -o $(PCH).gch -c $(PCH)
	@echo "Completed"

release: $(eval CFLAGS += $(RFLAGS)) bin $(OBJ)
	@$(CC) -o $(OUT) $(OBJ) $(LIBOBJ) $(LIB) -mwindows
	@echo "[100%] Compilation completed"

bin:
	@mkdir -p $(BIN)

app: $(eval CFLAGS += $(DFLAGS)) bin $(OBJ) 
	@$(CC) -o $(OUT) $(OBJ) $(LIBOBJ) $(LIB)
	@echo "[100%] Compilation completed"

%.o: %.cpp
	$(eval PROCESS_INDEX := $(shell expr $(PROCESS_INDEX) + 1))
	$(eval PROCESS_PERCENTAGE := $(shell echo "$$(($(PROCESS_INDEX) * 100))"))
	$(eval PROCESS_PERCENTAGE := $(shell echo "$$(($(PROCESS_PERCENTAGE) / $(TOTAL_FILES)))"))
	@echo "[$(PROCESS_PERCENTAGE)%] $<"
	@$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $< -MMD

-include $(DEP)
