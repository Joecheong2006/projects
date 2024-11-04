CC = g++
CFLAGS = -std=c++11 -Wall -Wextra -Wpedantic -Wstrict-aliasing -Werror -Isrc
DEBUG_FLAGS = -g3 -DDEBUG
RELEASE_FLAGS = -O2
DEBUGGER = gdb

FLAGS=

SRC = $(wildcard src/*.cpp src/**/*.cpp test/*.cpp)
OBJ = $(SRC:%.cpp=%.o)
DEP = $(OBJ:%.o=%.d)
BIN = test/bin
PROGRAM = test

ifeq ($(OS),Windows_NT)
	OUT = $(BIN)/$(PROGRAM).exe
else
	OUT = $(BIN)/(PROGRAM)
endif

bin:
	@mkdir -p $(BIN)

test-rel: CFLAGS += $(RELEASE_FLAGS) $(FLAGS)
test-rel: bin $(OBJ)
	@$(CC) $(CFLAGS) $(OBJ) -o $(OUT)
	@echo "Compilation completed"

test: CFLAGS += $(DEBUG_FLAGS) $(FLAGS)
test: bin $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(OUT)
	@echo "Compilation completed"

debug:
	@$(DEBUGGER) -q $(OUT) --eval-command=run --eval-command=exit

check:
	@valgrind --leak-check=yes $(OUT)

run:
	@./$(OUT) 

clean:
	@rm -rf $(OBJ) $(DEP) $(BIN)
	@echo Cleaned

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -MMD -o $@

-include $(DEP)
