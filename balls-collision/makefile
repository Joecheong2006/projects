CC = g++
CFLAGS := -std=c++17
# CFLAGS += -O3 -march=native -flto -fdefer-pop -fmerge-constants -fthread-jumps -floop-optimize -fif-conversion2 -fif-conversion -fcrossjumping -fguess-branch-probability -fcprop-registers -foptimize-sibling-calls -fstrength-reduce -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop -frerun-loop-opt -fgcse -fgcse-lm -fgcse-sm -fdelete-null-pointer-checks -fexpensive-optimizations -fregmove -fschedule-insns -fschedule-insns2 -fsched-interblock -fsched-spec -fcaller-saves -fpeephole2 -freorder-blocks -freorder-functions -fstrict-aliasing -falign-functions -falign-jumps -falign-loops -falign-labels -finline-functions -frename-registers
# CFLAGS += -Wall -Wextra -Wpedantic -Wstrict-aliasing -lm

SOURCE = src src/**
LIBRARY = lib/glad/src lib/stb_image lib/imgui
INCLUDE = -Ilib -Ilib/glad/include -Ilib/stb_image -Ilib/glfw/ -Ilib/imgui

SRC = $(wildcard $(patsubst %, %/*.cpp, $(SOURCE)))
OBJ = $(SRC:%.cpp=%.o)
LIBOBJ = $(wildcard $(patsubst %, %/*.o, $(LIBRARY))) 
DEP = $(OBJ:%.o=%.d)

DLL = lib/glfw/glfw3.dll 
LIB = $(LIBOBJ) $(DLL) lib/glfw/libglfw3dll.a -lopengl32
# LIB = $(LIBOBJ) -lglfw -lOpenGL

BIN = bin
OUT = $(BIN)/balls_collision_test.exe

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
	@cd lib/glad && $(CC) -Iinclude -o src/glad.o -c src/glad.c
	@echo "Completed"

bin:
	@mkdir -p $(BIN)
	@cp $(DLL) $(BIN)/.

app: $(OBJ)
	$(CC) -o $(OUT) $(OBJ) $(LIB)
	@echo "[100%] Compilation completed"

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $< -MMD

-include $(DEP)
