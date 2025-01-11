CC = gcc

CFLAGS = -Wall -Wextra -std=c89 -pedantic

BIN = graph.exe
OBJ = main.o tokens.o yard.o postfix_eval.o postscript.o stack.o

$(BIN): $(OBJ)
	$(CC) $(OBJ) -lm -o $(BIN) $(CFLAGS)

.c.o:
	$(CC) -c $< $(CFLAGS)

clean:
	rm -f *.o $(BIN)
