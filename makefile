CC := gcc
CFLAGS := -g -Wall

SRC := ./src
BIN := ./bin
EXE := emulator

debug: compile
	gdb $(BIN)/$(EXE)

run: compile
	$(BIN)/$(EXE)

compile: *.o
	$(CC) $^ -o $(BIN)/$(EXE)
	mv *.o $(BIN)

%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $^

.PHONY: clean

clean:
	-rm $(BIN)/*.o
	-rm $(BIN)/$(EXE)
