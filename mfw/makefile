CC = g++
PCH = src/mfwpch.h
CFLAGS := -std=c++11 -Wall -Wextra -Wpedantic -Wstrict-aliasing -g -include $(PCH) -DDEBUG -DUNICODE -D_UNICODE
SOURCE = src src/**
LIBRARY = lib/stb_image lib/imgui
INCLUDE = -Ilib -Ilib/stb_image -Ilib/imgui -Isrc
LIB = -lopengl32 -lGdi32 -lWinmm -ld3dcompiler -ldwmapi


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
	@cd lib/stb_image && $(CC) -c stb_image.cpp
	@cd lib/imgui && $(CC) -c ./*.cpp
	@$(CC) -x c++-header $(INCLUDE) -o $(PCH).gch -c $(PCH)
	@echo "Completed"

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
