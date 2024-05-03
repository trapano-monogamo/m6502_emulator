CC := g++
CFLAGS := -g -Wall

SRC := ./src
BIN := ./bin
INCLUDE := ./include
TESTS := ./tests

EXE := emulator
TEST_EXE := emulator_test

compile: *.o
	$(CC) $^ -o $(BIN)/$(EXE)
	mv *.o $(BIN)

%.o: $(SRC)/%.cpp
	$(CC) $(CFLAGS) -I$(INCLUDE) -c $^

run: compile
	$(BIN)/$(EXE)

debug: compile
	gdb $(BIN)/$(EXE)

.PHONY: clean

clean:
	-rm $(BIN)/*.o
	-rm $(BIN)/$(EXE)
