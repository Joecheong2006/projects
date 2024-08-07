CC = gcc
# CFLAGS = -std=c11 -Wall -Wextra -g3 -DDEBUG -fPIC
CFLAGS = -std=c11 -Wall -Wextra -g3 -fPIC

DLL = lib/glfw/glfw3.dll lib/openal/router/Win64/OpenAL32.dll lib/sndfile/sndfile.dll
LIB_WIN = -Llib/openal/router/WIN64 -Llib/sndfile -Llib/glfw lib/glfw/libglfw3dll.a -lopengl32 -lOpenAL32 -lsndfile -lWinmm
LIB_LIN = -lglfw -lGL -lOpenGL -lopenal -lsndfile -lm -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

SOURCE = src src/** src/**/**
INCLUDE = -Isrc -Ilib -Ilib/glad/include -Ilib/stb_image -Ilib/glfw -Ilib/sndfile/include -Ilib/openal
LIBRARY = lib/glad/src lib/stb_image

SRC = $(wildcard $(patsubst %, %/*.c, $(SOURCE)))
OBJ = $(SRC:%.c=%.o)
DEP = $(OBJ:%.o=%.d)

LIBOBJ = $(wildcard $(patsubst %, %/*.o, $(LIBRARY))) 
BIN = bin
OUT_NAME := $(BIN)/gamefw

LIB := 
ifeq ($(OS),Windows_NT)
	DLLFILES += lib/glfw/glfw3.dll
	LIB += $(LIB_WIN)
	OUT = $(OUT_NAME).dll
else
	LIB += $(LIB_LIN)
	OUT = $(OUT_NAME).so
endif


all: bin app

clean:
	@rm -rf $(OBJ) $(DEP) $(BIN)
	
build:
	cd lib/glad && $(CC) -Iinclude -o src/glad.o -c src/glad.c -fPIC
	cd lib/stb_image && $(CC) -c stb_image.c -fPIC

bin:
	@mkdir -p $(BIN)
	@cp -p $(DLL) $(BIN)/.

app: $(OBJ)
	$(CC) -shared -fPIC -o $(OUT) $(OBJ) $(LIBOBJ) $(LIB)

release: bin $(OBJ)
	$(CC) -shared -fPIC -o $(OUT) $(OBJ) $(LIBOBJ) $(LIB) -mwindows

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $< -MMD

-include $(DEP)
