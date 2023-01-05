#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#define LENGTH(X) (sizeof X / sizeof X[0])

typedef struct {
	KeySym keysym;
	void (*func)(const void *);
	const void *arg;
} Keybinding;

/* Function Declarations */
static void cmdexec(const void *arg);
static void keypress(XEvent *e);

/* Configuration */
static Keybinding keys[] = {
	{ XK_Return, cmdexec, { "xterm", "-display", ":1", "NULL" } }, /* test spawn xterm */
};

static Display *d;
static Window root;

static void (*handle_event[LASTEvent]) (XEvent *) = {
	[KeyPress] = keypress,
};

void 
cmdexec(const void *arg)
{
	execvp(((char **)arg)[0], (char **)arg);
	printf("execvp '%s' failed\n", ((char **)arg)[0]);
}

void 
keypress(XEvent *e)
{
	keys[0].func(&(keys[0].arg));
	/*
	KeySym keysym = XKeycodeToKeysym(d, (KeyCode)(&(&e->xkey)->keycode), 0);
	for(int i = 0; i < LENGTH(keys); i++) {
		if(keysym == keys[i].keysym) keys[i].func(&(keys[i].arg)); // change i to keycode for faster indexing
	}
	*/
}

void 
handle_events(void)
{
	XEvent e;
	while(1) {
		XNextEvent(d, &e);
		XSync(d, False);
		if(handle_event[e.type]) handle_event[e.type](&e);
	}
}

int
setup_root(void)
{
	XSetWindowAttributes a;
	if(!(d = XOpenDisplay(NULL))) return EXIT_FAILURE;
	root = RootWindow(d, DefaultScreen(d));
	a.event_mask = StructureNotifyMask|SubstructureNotifyMask|EnterWindowMask|LeaveWindowMask|PropertyChangeMask|SubstructureRedirectMask;
	XSelectInput(d, root, a.event_mask);
	for(int i = 0; i < LENGTH(keys); i++) {
		XGrabKey(d, XKeysymToKeycode(d, keys[i].keysym), Mod1Mask, root, True, GrabModeAsync, GrabModeAsync);
	}
}

int 
main(void)
{
	setup_root();
	handle_events();
	XCloseDisplay(d);
	return EXIT_SUCCESS;
}

