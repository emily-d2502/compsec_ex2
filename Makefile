CC := gcc
SRCS := main.c
BIN := find_secrets

$(BIN):
	$(CC) $(SRCS) -o $(BIN)

clean:
	rm -rf $(BIN)