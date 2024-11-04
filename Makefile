CC=gcc

# -Wl,-R flag to . and ./bin/ for running from the base project directory
RAYLIB_ARGS=-lm ./raylib/src/libraylib.a

# Set the WINDOWS flag on by default when running on Windows
ifeq ($(OS),Windows_NT)
	ifndef WINDOWS
		WINDOWS = true
	endif
endif

# Link winsock and windows raylib dependencies
ifdef WINDOWS
	SOCKET_ARGS += -Wl,-Bstatic -lpthread -lws2_32
	RAYLIB_ARGS += -lwinmm -lgdi32 -luser32 -lshell32
endif

make: main.c raylib
	mkdir -p bin && $(CC) -o bin/main main.c $(RAYLIB_ARGS)
run: main.c raylib
	mkdir -p bin && $(CC) -o bin/main main.c $(RAYLIB_ARGS)
	./bin/main
server: server.c
	mkdir -p bin && $(CC) -o bin/server server.c $(SOCKET_ARGS)
big_server: BIG_server.c
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
	make -C raylib/src/ PLATFORM=PLATFORM_DESKTOP PLATFORM_DESKTOP=WINDOWS PLATFORM_OS=WINDOWS CC=$(CC)
else
	make -C raylib/src/ PLATFORM=PLATFORM_DESKTOP CC=$(CC)
endif
