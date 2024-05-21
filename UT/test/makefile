CC = g++
TYPE = cpp
CFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -Wstrict-aliasing -g3

BIN = bin
SRC = $(wildcard *.$(TYPE))
TEST = $(SRC:%.$(TYPE)=%)

all: bin
	@for test in $(TEST); do $(CC) $(CFLAGS) $$test.$(TYPE) -o $(BIN)/$$test; done

bin:
	@mkdir -p $(BIN)

run:
	@for test in $(TEST); do ./$(BIN)/$$test; done

clean:
	@rm -rf $(BIN)
	@echo Cleaned

