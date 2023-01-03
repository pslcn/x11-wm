all: src/wm.c
	gcc src/wm.c -o wm -lX11
