CC = g++
CFLAGS := -std=c++11 -Wall -Wextra -Wpedantic -Wstrict-aliasing -g

SOURCE = src src/**
LIBRARY = lib/glad/src lib/stb_image
INCLUDE = -Ilib -Ilib/glad/include -Ilib/stb_image -Ilib/glfw/

LIBOBJ = $(wildcard $(patsubst %, %/*.o, $(LIBRARY))) 
SRC = $(wildcard $(patsubst %, %/*.cpp, $(SOURCE)))
OBJ = $(SRC:%.cpp=%.o)
DEP = $(OBJ:%.o=%.d)

DLL = lib/glfw/glfw3.dll 
LIB = $(LIBOBJ) $(DLL) lib/glfw/libglfw3dll.a -lopengl32

BIN = bin
OUT = $(BIN)/glsl_test.exe

.PHONY: all clean compile_obj

all: bin app

build:
	@echo "Compiling library"
	@cd lib/stb_image && $(CC) -o stb_image.o -c stb_image.cpp
	@cd lib/glad && $(CC) -Iinclude -o src/glad.o -c src/glad.c
	@echo "Completed"

run:
	@./$(OUT)

clean:
	@rm -rf $(OBJ) $(DEP) $(BIN)
	@echo Cleaned

bin:
	@mkdir -p $(BIN)
	@cp $(DLL) $(BIN)/.


app: bin $(OBJ) compile_obj
	@$(CC) -o $(OUT) $(OBJ) $(LIB)
	@echo "[100%] Compilation completed"

.PROCESS_INDEX = -1
.PROCESS_PERCENTAGE = 0
.MFILECOUNT := 0
.MSRCFILE := 
.MOBJFILE := 

%.o: %.cpp
	$(eval .MSRCFILE += $<)
	$(eval .MOBJFILE += $@)
	$(eval .MFILECOUNT := $(shell expr $(.MFILECOUNT) + 1))

compile_obj:
	$(if $(filter-out 0, $(words $(.MSRCFILE))),\
		@$(foreach i, $(shell seq 1 $(words $(.MSRCFILE))),\
			$(eval .PROCESS_INDEX := $(shell expr $(.PROCESS_INDEX) + 1))\
			$(eval .PROCESS_PERCENTAGE := $(shell echo "$$(($(.PROCESS_INDEX) * 100))"))\
			$(eval .PROCESS_PERCENTAGE := $(shell echo "$$(($(.PROCESS_PERCENTAGE) / $(.MFILECOUNT)))"))\
			echo "[$(.PROCESS_PERCENTAGE)%] $(word $(i), $(.MSRCFILE))";\
			$(CC) $(CFLAGS) $(INCLUDE) -o $(word $(i), $(.MOBJFILE)) -c $(word $(i), $(.MSRCFILE)) -MMD;\
		)\
	)

-include $(DEP)
