CC = g++
CFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -Wstrict-aliasing -g -DDEBUG
SOURCE = src src/**

SRC = $(wildcard $(patsubst %, %/*.cpp, $(SOURCE)))
OBJ = $(SRC:%.cpp=%.o)
DEP = $(OBJ:%.o=%.d)

BIN = bin

PROCESS_INDEX = -1
TOTAL_FILES = $(words $(SRC))
PROCESS_PERCENTAGE = 0

.PHONY: all clean

all: bin app

clean:
	@rm -rf $(OBJ) $(DEP) $(BIN)
	@echo Cleaned

.test:
	@cd test && make -s
	@cd test && make run -s

app: bin $(OBJ) 
	@$(CC) -o $(OUT) $(OBJ)
	@echo "[100%] Compilation completed"

%.o: %.cpp
	$(eval PROCESS_INDEX := $(shell expr $(PROCESS_INDEX) + 1))
	$(eval PROCESS_PERCENTAGE := $(shell echo "$$(($(PROCESS_INDEX) * 100))"))
	$(eval PROCESS_PERCENTAGE := $(shell echo "$$(($(PROCESS_PERCENTAGE) / $(TOTAL_FILES)))"))
	@echo "[$(PROCESS_PERCENTAGE)%] $<"
	@$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $< -MMD

-include $(DEP)
