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
static void mappingnotify(XEvent *e);

/* Configuration */
static Keybinding keys[] = {
	{ XK_Return, cmdexec, { "st", "NULL" } },
};

static Display *d;
static Window root;

void
grabkeys()
{
	for(int i = 0; i < LENGTH(keys); i++) {
		XGrabKey(d, XKeysymToKeycode(d, keys[i].keysym), Mod1Mask, root, True, GrabModeAsync, GrabModeAsync);
	}
}

void 
mappingnotify(XEvent *e)
{
	XMappingEvent *ev = &e->xmapping;
	XRefreshKeyboardMapping(ev);
	if(ev->request == MappingKeyboard) grabkeys();
}

void
maprequest(XEvent *e)
{
	static XWindowAttributes wa;
	XMapRequestEvent *ev = &e->xmaprequest;
	Window w = ev->window;
	XGetWindowAttributes(d, w, &wa); 
	XWindowChanges wc;
	wc.x = 20;
	wc.y = 20;
	XConfigureWindow(d, w, CWX|CWY, &wc);
	XSelectInput(d, w, EnterWindowMask|FocusChangeMask|PropertyChangeMask|StructureNotifyMask);
	XMapWindow(d, w);
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
	XWindowChanges wc;
	wc.x = ev->x;
	wc.y = ev->y;
	wc.width = ev->width;
	wc.height = ev->height;
	wc.border_width = ev->border_width;
	wc.sibling = ev->above;
	wc.stack_mode = ev->detail;
	XConfigureWindow(d, ev->window, ev->value_mask, &wc);
	XSync(d, False);
}

static void (*handle_event[LASTEvent]) (XEvent *) = {
	[ConfigureRequest] = configurerequest,
	[KeyPress] = keypress,
	[MapRequest] = maprequest,
	[MappingNotify] = mappingnotify,
};

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
	grabkeys();
}

int 
main(void)
{
	setup_root();
	handle_events();
	XCloseDisplay(d);
	return EXIT_SUCCESS;
}

