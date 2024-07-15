CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -g

SOURCE = src src/**
INCLUDE = -Ilib -Ilib/glad/include -Ilib/stb_image
LIBRARY = lib/glad/src lib/stb_image

SRC = $(wildcard $(patsubst %, %/*.c, $(SOURCE)))
OBJ = $(SRC:%.c=%.o)
DEP = $(OBJ:%.o=%.d)

LIBOBJ = $(wildcard $(patsubst %, %/*.o, $(LIBRARY))) 

# DLL = lib/glfw/glfw3.dll 
# LIB = $(LIBOBJ) $(DLL) lib/glfw/libglfw3dll.a -lGL 
LIB = -lglfw -lGL -lOpenGL -lm
BIN = bin
OUT = $(BIN)/main


all: bin app

run:
	@./$(OUT)

clean:
	@rm -rf $(OBJ) $(BIN)

build:
	cd lib/glad && $(CC) -Iinclude -o src/glad.o -c src/glad.c
	cd lib/stb_image && $(CC) -c stb_image.c

bin:
	mkdir -p $(BIN)

app: $(OBJ)
	$(CC) -o $(OUT) $(OBJ) $(LIBOBJ) $(LIB)

release: bin $(OBJ)
	$(CC) -o $(OUT) $(OBJ) $(LIB) -mwindows

debug:
	@gdb -q $(OUT) --eval-command=run --eval-command=exit
	
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<
