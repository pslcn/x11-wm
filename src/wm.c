#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

int 
main(void)
{
	Display *d;
	int screen;
	Window root;
	XWindowAttributes attrs;
	XEvent e;

	if(!(d = XOpenDisplay(NULL))) return EXIT_FAILURE;
	screen = DefaultScreen(d);
	root = RootWindow(d, screen);

	XGrabButton(d, 1, Mod1Mask, root, True, ButtonPressMask, GrabModeAsync, GrabModeAsync, None, None);

	int right = 1;
	int up = 1;
	int x = 0;
	int y = 0;
	for(;;) {
		XNextEvent(d, &e);
		if(e.type == ButtonPress && e.xbutton.subwindow != None) {
			XGetWindowAttributes(d, e.xbutton.subwindow, &attrs);
			x = 40;
			y = 40;
			for(;;) {
				XMoveWindow(d, e.xbutton.subwindow, x, y);
				XSync(d, 0);
				if(x > 1280 - attrs.width) {
					right = 0;
				} else if(x < 0) {
					right = 1;
				}
				if(y < 0) {
					up = 1;
				} else if(y > 800 - attrs.height) {
					up = 0;
				}
				x += (right == 1) ? 8 : -6;
				y += (up == 1) ? 6 : -4;
				usleep(50 * 1000);
			}
		}
	}
	
	// return EXIT_SUCCESS;
}

