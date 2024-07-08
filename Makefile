CC := gcc
SRCS := main.c
BIN := find_secrets

$(BIN):
	echo "usage: $(BIN) [--run-bash|-b][--secret-num|-n num of secret]\n"
	$(CC) $(SRCS) -o $(BIN)

clean:
	rm -rf $(BIN)