CC=gcc

make: main.c raylib
	mkdir -p bin && gcc -o bin/main main.c -lm bin/libraylib.a
run: main.c raylib
	mkdir -p bin && gcc -o bin/main main.c -lm bin/libraylib.a
	./bin/main
server: server.c
	mkdir -p bin && gcc -o bin/server server.c
BIG_server: BIG_server.c
	mkdir -p bin && gcc -o bin/BIG_server BIG_server.c
client: client.c
	mkdir -p bin && gcc -o bin/client client.c -lm bin/libraylib.a
run-server: server.c
	mkdir -p bin && gcc -o bin/server server.c && ./bin/server
run-client: client.c
	mkdir -p bin && gcc -o bin/client client.c -lm bin/libraylib.a
	./bin/client
run-big_server: BIG_server.c
	mkdir -p bin && gcc -o bin/BIG_server BIG_server.c && ./bin/BIG_server
clean:
	rm -r bin

raylib: raylib/src/raylib.h
	make -C raylib/src/ PLATFORM=PLATFORM_DESKTOP CC=$(CC)
	cp raylib/src/*.a bin/
	rm raylib/src/*.o
	rm raylib/src/*.a
