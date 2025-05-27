cflags=-I./include

compile: build run

build:
	gcc $(cflags) incercare.c -o incercare.exe -L./lib -lSDL3 -lSDL3_image 
run:
	cmd /c incercare.exe