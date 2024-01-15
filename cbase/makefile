CC = gcc
CFLAGS := -std=c99 -Wall -Wextra -Wstrict-aliasing -g

FILETYPE = c
SOURCE = src src/**
LIBRARY = 
INCLUDE = 

LIBOBJ = $(wildcard $(patsubst %, %/*.o, $(LIBRARY))) 
SRC = $(wildcard $(patsubst %, %/*.$(FILETYPE), $(SOURCE)))
OBJ = $(SRC:%.$(FILETYPE)=%.o)
DEP = $(OBJ:%.o=%.d)

DLL = 
LIB = 

BIN = bin
OUT = $(BIN)/cbase.exe

.PHONY: all clean compile_obj

all: bin app

run:
	@./$(OUT)

clean:
	@rm -rf $(OBJ) $(DEP) $(BIN)
	@echo Cleaned

bin:
	@mkdir -p $(BIN)


app: bin $(OBJ) compile_obj
	@$(CC) -o $(OUT) $(OBJ) $(LIB)
	@echo "[100%] Compilation completed"

.PROCESS_INDEX = -1
.PROCESS_PERCENTAGE = 0
.MFILECOUNT := 0
.MSRCFILE := 
.MOBJFILE := 

%.o: %.$(FILETYPE)
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
