CC = gcc
# CFLAGS = -std=c11 -Wall -Wextra -g3

DLL = ../lib/glfw/glfw3.dll ../lib/openal/router/Win64/OpenAL32.dll ../lib/sndfile/sndfile.dll ../bin/gamefw.dll
LIB_WIN = -L../bin/ -lgamefw ../lib/glfw/libglfw3dll.a  -LC:\VulkanRT-1.3.290.0-Components\x64 -lvulkan-1
LIB_LIN = ../bin/gamefw.so -lglfw -lm -lvulkan

SOURCE = src src/** src/**/**
INCLUDE = -IC:\VulkanSDK\1.3.290.0\Include -I../src -I../lib -I../lib/glad/include -I../lib/stb_image -I../lib/glfw -I../lib/sndfile/include -I../lib/openal
LIBRARY = 

SRC = $(wildcard $(patsubst %, %/*.c, $(SOURCE)))
OBJ = $(SRC:%.c=%.o)
DEP = $(OBJ:%.o=%.d)

LIBOBJ = $(wildcard $(patsubst %, %/*.o, $(LIBRARY))) 
BIN = bin
OUT_NAME = $(BIN)/testing

LIB := 
ifeq ($(OS),Windows_NT)
	DLLFILES += lib/glfw/glfw3.dll
	LIB += $(LIB_WIN)
	OUT = $(OUT_NAME).exe
else
	LIB += $(LIB_LIN)
	OUT = $(OUT_NAME)
endif


all: bin app

run:
	@./$(OUT)

clean:
	@rm -rf $(OBJ) $(DEP) $(BIN)

bin:
	@mkdir -p $(BIN)
	@cp -p $(DLL) $(BIN)/.

app: $(OBJ)
	$(CC) -o $(OUT) $(OBJ) $(LIBOBJ) $(LIB)

debug:
	@gdb -q $(OUT) --eval-command=run --eval-command=exit
	
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $< -MMD

-include $(DEP)
