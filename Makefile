CC=gcc
VERSION=0.0.19.1
PORT=42042
THREADNUM=10
# enable to dynamically link raylib and use the system installation
#SYSTEM_RAYLIB=true

ARGS=-Wall -Wextra
ifndef SYSTEM_RAYLIB
	RAYLIB_ARGS=-lm ./raylib/src/libraylib.a
else
	RAYLIB_ARGS=-lraylib
endif

# Link winsock and windows raylib dependencies
ifeq ($(OS), Windows_NT)
	SOCKET_ARGS += -Wl,-Bstatic -lpthread -lws2_32
	RAYLIB_ARGS += -lwinmm -lgdi32 -luser32 -lshell32
endif

make: main.c raylib
	mkdir -p bin && $(CC) -o bin/main main.c $(ARGS) $(RAYLIB_ARGS)
run: main.c raylib
	mkdir -p bin && $(CC) -o bin/main main.c $(ARGS) $(RAYLIB_ARGS)
	./bin/main
server: server.c
	mkdir -p bin && $(CC) -o bin/server server.c $(ARGS) $(SOCKET_ARGS) -D'TGW_VERSION="$(VERSION)"' -D'TGW_PORT=$(PORT)'
big_server: BIG_server.c
	mkdir -p bin && $(CC) -o bin/BIG_server BIG_server.c $(ARGS) $(SOCKET_ARGS) -D'TGW_VERSION="$(VERSION)"' -D'TGW_PORT=$(PORT)' -D'TGW_THREADNUM=$(THREADNUM)'
client: client.c raylib
	mkdir -p bin && $(CC) -o bin/client client.c $(ARGS) $(SOCKET_ARGS) $(RAYLIB_ARGS) -D'TGW_VERSION="$(VERSION)"'
run-server: server.c
	mkdir -p bin && $(CC) -o bin/server server.c $(ARGS) $(SOCKET_ARGS) -D'TGW_VERSION="$(VERSION)"' -D'TGW_PORT=$(PORT)' && ./bin/server
run-client: client.c raylib
	mkdir -p bin && $(CC) -o bin/client client.c $(ARGS) $(SOCKET_ARGS) $(RAYLIB_ARGS) -D'TGW_VERSION="$(VERSION)"'
	./bin/client
run-big_server: BIG_server.c
	mkdir -p bin && $(CC) -o bin/BIG_server BIG_server.c $(ARGS) $(SOCKET_ARGS) -D'TGW_VERSION="$(VERSION)"' -D'TGW_PORT=$(PORT)' -D'TGW_THREADNUM=$(THREADNUM)' && ./bin/BIG_server
bot: bot.c raylib
	mkdir -p bin && $(CC) -o bin/bot bot.c $(ARGS) $(SOCKET_ARGS) $(RAYLIB_ARGS) -D'TGW_VERSION="$(VERSION)"'
clean:
	rm raylib/src/*.o
	rm -r bin

raylib: raylib/src/*.h
ifndef SYSTEM_RAYLIB

ifeq ($(OS), Windows_NT)
	make -C raylib/src/ PLATFORM=PLATFORM_DESKTOP PLATFORM_DESKTOP=WINDOWS PLATFORM_OS=WINDOWS CC=$(CC)
else
	make -C raylib/src/ PLATFORM=PLATFORM_DESKTOP CC=$(CC)
endif

endif
