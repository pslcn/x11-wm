all: src/wm.c
	gcc src/wm.c -o bin/wm -lX11
