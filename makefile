CC := g++
CFLAGS := -g -Wall

SRC := ./src
BIN := ./bin
INCLUDE := ./include

EXE := emulator

compile: *.o
	$(CC) $^ -o $(BIN)/$(EXE)
	mv *.o $(BIN)

%.o: $(SRC)/%.cpp
	$(CC) $(CFLAGS) -I$(INCLUDE) -c $^

test: CFLAGS += -DTEST_MODE
test: compile
	$(BIN)/$(EXE)

run: compile
	$(BIN)/$(EXE)

debug: compile
	gdb $(BIN)/$(EXE)

.PHONY: clean

clean:
	-rm $(BIN)/*.o
	-rm $(BIN)/$(EXE)
