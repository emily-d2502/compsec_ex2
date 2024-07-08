CC := gcc
SRCS := main.c
BIN := attack

$(BIN):
	$(CC) $(SRCS) -o $(BIN)

clean:
	rm -rf $(BIN)
