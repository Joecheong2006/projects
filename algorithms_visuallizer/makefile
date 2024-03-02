CC = g++
CFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -Wstrict-aliasing -g

SOURCE = src src/**
LIBRARY = lib/glad/src lib/stb_image
INCLUDE = -Ilib -Ilib/glad/include -Ilib/stb_image -Ilib/glfw/

SRC = $(wildcard $(patsubst %, %/*.cpp, $(SOURCE)))
OBJ = $(SRC:%.cpp=%.o)
LIBOBJ = $(wildcard $(patsubst %, %/*.o, $(LIBRARY))) 
DEP = $(OBJ:%.o=%.d)

DLL = lib/glfw/glfw3.dll 
LIB = $(LIBOBJ) $(DLL) lib/glfw/libglfw3dll.a -lopengl32

BIN = bin
OUT = $(BIN)/main.exe


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
	@cd lib/glad && $(CC) -Iinclude -o src/glad.o -c src/glad.c
	@echo "Completed"

bin:
	@mkdir -p $(BIN)
	@cp $(DLL) $(BIN)/.

app: $(OBJ)
	@$(CC) -o $(OUT) $(OBJ) $(LIB)
	@echo "[100%] Compilation completed"

%.o: %.cpp
	$(eval PROCESS_INDEX := $(shell expr $(PROCESS_INDEX) + 1))
	$(eval PROCESS_PERCENTAGE := $(shell echo "$$(($(PROCESS_INDEX) * 100))"))
	$(eval PROCESS_PERCENTAGE := $(shell echo "$$(($(PROCESS_PERCENTAGE) / $(TOTAL_FILES)))"))
	@echo "[$(PROCESS_PERCENTAGE)%] $<"
	@$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $< -MMD

-include $(DEP)
