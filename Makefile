make: main.c
	mkdir -p bin && gcc -o bin/main main.c -lraylib
run: main.c
	mkdir -p bin && gcc -o bin/main main.c -lraylib && ./bin/main
server: server.c
	mkdir -p bin && gcc -o bin/server server.c
BIG_server: BIG_server.c
	mkdir -p bin && gcc -o bin/BIG_server BIG_server.c
client: client.c
	mkdir -p bin && gcc -o bin/client client.c -lraylib
run-server: server.c
	mkdir -p bin && gcc -o bin/server server.c && ./bin/server
run-client: client.c
	mkdir -p bin && gcc -o bin/client client.c -lraylib && ./bin/client
clean:
	rm -r bin
