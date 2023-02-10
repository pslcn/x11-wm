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
	void (*func)(const void **);
	const void *arg;
} Keybinding;

/* Function Declarations */
static void cmdexec(const void **arg);
static void configurerequest(XEvent *e); 
static void keypress(XEvent *e);
static void maprequest(XEvent *e); 

/* Variable Declarations */
static void (*handle_event[LASTEvent]) (XEvent *) = {
	[ConfigureRequest] = configurerequest,
	[KeyPress] = keypress,
	[MapRequest] = maprequest,
};
static Display *d;
static Window root;

/* Configuration */
static Keybinding keys[] = {
	{ XK_Return, cmdexec,		 { "st", "NULL" } },
	{ XK_C,			 killclient, { NULL } }, 
};

/* Function Implementations */
void
maprequest(XEvent *e)
{
	static XWindowAttributes wa;
	XMapRequestEvent *ev = &e->xmaprequest;
	seltag->sel = ev->window;
	XGetWindowAttributes(d, seltag->sel, &wa); 
	XWindowChanges wc = {.x = 20, .y = 20,};
	XConfigureWindow(d, seltag->sel, CWX|CWY, &wc);
	XSelectInput(d, seltag->sel, EnterWindowMask|FocusChangeMask|PropertyChangeMask|StructureNotifyMask);
	XMapWindow(d, seltag->sel);
}

void 
cmdexec(const void **arg)
{
	if(fork() == 0) {
		setsid();
		execvp(((char **)arg)[0], (char **)arg);
		printf("execvp '%s' failed\n", ((char **)arg)[0]);
	}
}

void
killclient(const void **arg)
{
	XGrabServer(d);
	XSetCloseDownMode(d, DestroyAll);
	XKillClient(d, seltag->sel);
	XSync(d, False);
	XUngrabServer(d);
}

void 
keypress(XEvent *e)
{
	XKeyEvent *ev = &e->xkey;
	KeySym keysym = XKeycodeToKeysym(d, (KeyCode)ev->keycode, 0);
	for(unsigned int i = 0; i < LENGTH(keys); i++) {
		if(keysym == keys[i].keysym && keys[i].func) keys[i].func(&(keys[i].arg)); 
	}
}

void 
configurerequest(XEvent *e)
{
	XConfigureRequestEvent *ev = &e->xconfigurerequest;
	XWindowChanges wc = {.x = ev->x, .y = ev->y, .width = ev->width, .height = ev->height, .border_width = ev->border_width, .sibling = ev->above, .stack_mode = ev->detail, };
	XConfigureWindow(d, ev->window, ev->value_mask, &wc);
	XSync(d, False);
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
	seltag = create_tag();
}

int 
main(void)
{
	setup_root();
	handle_events();
	free_tag(seltag);
	XCloseDisplay(d);
	return EXIT_SUCCESS;
}

