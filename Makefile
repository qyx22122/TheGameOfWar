make:
	gcc -o main main.c -lraylib
run:
	gcc -o main main.c -lraylib && ./main
