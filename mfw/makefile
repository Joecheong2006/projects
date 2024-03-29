FLAGS := 
DEFINE := 

CC = g++
PCH = src/mfwpch.h
CFLAGS := $(FLAGS) -include $(PCH) -DUNICODE $(DEFINE)
SOURCE = src src/**
LIBRARY = lib/stb_image lib/imgui
INCLUDE = -Ilib
LIB = -lopengl32 -lGdi32 -lWinmm -ld3dcompiler -ldwmapi

SRC = $(wildcard $(patsubst %, %/*.cpp, $(SOURCE)))
LIBOBJ = $(wildcard $(patsubst %, %/*.o, $(LIBRARY))) 
OBJ = $(SRC:%.cpp=%.o)
DEP = $(OBJ:%.o=%.d)
OUT = mfw.dll

PROCESS_INDEX = -1
TOTAL_FILES = $(words $(SRC))
PROCESS_PERCENTAGE = 0

.PHONY: all clean

all: app

clean:
	@rm -rf $(OBJ) $(DEP)
	@echo Cleaned

build:
	@echo "Compiling library"
	@cd lib/stb_image && $(CC) -c stb_image.cpp
	@cd lib/imgui && $(CC) -c -O3 ./*.cpp
	@$(CC) -x c++-header $(INCLUDE) -o $(PCH).gch -c $(PCH)
	@echo "Complete"

app: $(OBJ) 
	@$(CC) -shared -o $(OUT) $(OBJ) $(LIBOBJ) $(LIB)
	@echo "[100%] Compilation completed"

%.o: %.cpp
	$(eval PROCESS_INDEX := $(shell expr $(PROCESS_INDEX) + 1))
	$(eval PROCESS_PERCENTAGE := $(shell echo "$$(($(PROCESS_INDEX) * 100))"))
	$(eval PROCESS_PERCENTAGE := $(shell echo "$$(($(PROCESS_PERCENTAGE) / $(TOTAL_FILES)))"))
	@echo "[$(PROCESS_PERCENTAGE)%] $<"
	@$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $< -MMD

win_release:
	@make -j -s DEFINE="" FLAGS="-std=c++17 -O2"

win_debug:
	@make -j -s DEFINE="-DDEBUG" FLAGS="-std=c++17 -Wall -Wextra -Wpedantic -Wstrict-aliasing -g"

-include $(DEP)
