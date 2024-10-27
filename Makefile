CC=gcc

SOCKET_ARGS=
RAYLIB_ARGS=-L. -lm -lraylib 

# Set the WINDOWS flag on by default when running on Windows
ifeq ($(OS),Windows_NT)
	ifndef WINDOWS
		WINDOWS = true
	endif
endif

# Link winsock
ifdef WINDOWS
	SOCKET_ARGS += -lws2_32
endif

make: main.c raylib
	mkdir -p bin && $(CC) -o bin/main main.c $(RAYLIB_ARGS)
run: main.c raylib
	mkdir -p bin && $(CC) -o bin/main main.c $(RAYLIB_ARGS)
	./bin/main
server: server.c
	mkdir -p bin && $(CC) -o bin/server server.c $(SOCKET_ARGS)
BIG_server: BIG_server.c
	mkdir -p bin && $(CC) -o bin/BIG_server BIG_server.c $(SOCKET_ARGS)
client: client.c raylib
	mkdir -p bin && $(CC) -o bin/client client.c $(SOCKET_ARGS) $(RAYLIB_ARGS)
run-server: server.c
	mkdir -p bin && $(CC) -o bin/server server.c $(SOCKET_ARGS) && ./bin/server
run-client: client.c raylib
	mkdir -p bin && $(CC) -o bin/client client.c $(SOCKET_ARGS) $(RAYLIB_ARGS)
	./bin/client
run-big_server: BIG_server.c
	mkdir -p bin && $(CC) -o bin/BIG_server BIG_server.c $(SOCKET_ARGS) && ./bin/BIG_server
clean:
	rm raylib/src/*.o
	rm -r bin

raylib: raylib/src/*.h
ifdef WINDOWS
	make -C raylib/src/ PLATFORM=PLATFORM_DESKTOP PLATFORM_DESKTOP=WINDOWS PLATFORM_OS=WINDOWS RAYLIB_LIBTYPE=SHARED CC=$(CC)
	mkdir -p bin/
	cp raylib/src/raylib.dll bin/
else
	make -C raylib/src/ PLATFORM=PLATFORM_DESKTOP RAYLIB_LIBTYPE=SHARED CC=$(CC)
	mkdir -p bin/
	cp raylib/src/libraylib.so.5.5.0 bin/
	cp raylib/src/libraylib.so bin/
	cp raylib/src/libraylib.so.550 bin/
endif
