FLAGS := 
DEFINE := 

CC = g++
MAKE = make
PCH = include/mfwpch.h
CFLAGS := $(FLAGS) -include $(PCH) -DUNICODE $(DEFINE)
SOURCE = src src/**
LIBRARY = lib/stb_image lib/imgui
INCLUDE = -Ilib -Iinclude
LIB = -lopengl32 -lGdi32 -lWinmm -ld3dcompiler -ldwmapi

SRC = $(wildcard $(patsubst %, %/*.cpp, $(SOURCE)))
LIBOBJ = $(wildcard $(patsubst %, %/*.o, $(LIBRARY))) 
OBJ = $(SRC:%.cpp=%.o)
DEP = $(OBJ:%.o=%.d)
OUT = mfw.dll

.PHONY: all clean

all:

clean:
	@rm -rf $(OBJ) $(DEP)
	@echo Cleaned

build:
	@echo "Compiling library"
	cd lib/stb_image && $(CC) -c stb_image.cpp
	cd lib/imgui && $(CC) -c -O2 ./*.cpp
	$(CC) -x c++-header $(INCLUDE) -o $(PCH).gch -c $(PCH)
	@echo "Complete"

app: $(OBJ) 
	$(CC) -shared -o $(OUT) $(OBJ) $(LIBOBJ) $(LIB)
	@echo "[100%] Compilation completed"

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $< -MMD

win_release:
	@$(MAKE) app DEFINE="" FLAGS="-std=c++17 -O2"

win_debug:
	@$(MAKE) app DEFINE="-DDEBUG" FLAGS="-std=c++17 -Wall -Wextra -Wpedantic -Wstrict-aliasing -g"

-include $(DEP)
