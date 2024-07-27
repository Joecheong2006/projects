CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -g3 -DDEBUG

DLL = lib/glfw/glfw3.dll lib/openal/router/Win64/OpenAL32.dll lib/sndfile/sndfile.dll
LIB_WIN = -Llib/openal/router/WIN64 -Llib/sndfile -Llib/glfw lib/glfw/libglfw3dll.a -lopengl32 -lOpenAL32 -lsndfile
LIB_LIN = -lglfw -lGL -lOpenGL -lopenal -lsndfile -lm

SOURCE = src src/**
INCLUDE = -Ilib -Ilib/glad/include -Ilib/stb_image -Ilib/glfw -Ilib/sndfile/include -Ilib/openal
LIBRARY = lib/glad/src lib/stb_image

SRC = $(wildcard $(patsubst %, %/*.c, $(SOURCE)))
OBJ = $(SRC:%.c=%.o)
DEP = $(OBJ:%.o=%.d)

LIBOBJ = $(wildcard $(patsubst %, %/*.o, $(LIBRARY))) 
BIN = bin
OUT = $(BIN)/chess

LIB := 
ifeq ($(OS),Windows_NT)
	DLLFILES += lib/glfw/glfw3.dll
	LIB += $(LIB_WIN)
else
	LIB += $(LIB_LIN)
endif


all: bin app

run:
	@./$(OUT)

clean:
	@rm -rf $(OBJ) $(DEP) $(BIN)
	
build:
	cd lib/glad && $(CC) -Iinclude -o src/glad.o -c src/glad.c
	cd lib/stb_image && $(CC) -c stb_image.c

bin:
	@mkdir -p $(BIN)
	@cp -p $(DLL) $(BIN)/.

app: $(OBJ)
	$(CC) -o $(OUT) $(OBJ) $(LIBOBJ) $(LIB)

release: bin $(OBJ)
	$(CC) -o $(OUT) $(OBJ) $(LIBOBJ) $(LIB) -mwindows

debug:
	@gdb -q $(OUT) --eval-command=run --eval-command=exit
	
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $< -MMD

-include $(DEP)
