CC := gcc
SRCS := main.c
FLAGS := --std=c99
BIN := find_secrets

$(BIN):
	$(CC) $(FLAGS) $(SRCS) -o $(BIN)

clean:
	rm -rf $(BIN)