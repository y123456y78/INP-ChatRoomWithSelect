all:
	gcc -o client client.c
	gcc -o server server.c
clean:
	rm -f server client
