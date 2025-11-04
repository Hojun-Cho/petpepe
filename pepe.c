#include <X11/Xlib.h>
#include <X11/xpm.h>
#include <X11/extensions/shape.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "img.h"

Display *display;
Window root, win;
GC gc;

int
isqrt(int x)
{
	int s, u;
	
	if(x <= 0)
		return 0;
	if(x >= 32768L * (32768L/4))
		return(2*isqrt(x/4));
	for(s=2, u=4; u<x; s+=s, u*=4)
		;
	while((u=((x+s*s)/s)>>1) < s)
		s = u;
	return s;
}

void
loop(Pixmap *pix, Pixmap *msk, int spd)
{
	Window _root, _child;
	unsigned int _btn;
	int cx, cy, _wx, _wy;
	int i, x, y, dx, dy, dist;

	x = 0;
	y = 0;
	i = 0;
	for(;;){
		XQueryPointer(display, root, &_root, &_child, &cx, &cy, &_wx, &_wy, &_btn);
		dx = cx-x-W/2;
		dy = cy-y-W/2;
		dist = isqrt(dx*dx + dy*dy);
		if(dist > spd){
			x += dx*spd/dist;
			y += dy*spd/dist;
		}else if(dist){
			x += dx;
			y += dy;
		}
		i = (i+1)%N;
		XMoveWindow(display, win, x, y);
		XShapeCombineMask(display, win, ShapeBounding, 0, 0, msk[i], ShapeSet);
		XCopyArea(display, pix[i], win, gc, 0, 0, W, H, 0, 0);
		XFlush(display);
		usleep(1000 * 100);
	}
}

int
main(void)
{
	XRectangle rect;
	XSetWindowAttributes attr;
	Pixmap pix[N], msk[N];
	int screen, i;

	display = XOpenDisplay(NULL);
	screen = DefaultScreen(display);
	root = RootWindow(display, screen);
	memset(&attr, 0, sizeof(attr));
	attr.override_redirect = 1;
	win = XCreateWindow(display, root, 0, 0, W, H, 0, CopyFromParent,
						InputOutput, CopyFromParent, CWOverrideRedirect, &attr);
	XMapWindow(display, win);
	XShapeCombineRectangles(display, win, ShapeInput, 0, 0, &rect, 0, ShapeSet, 0);
	gc = XCreateGC(display, win, 0, NULL);

	for(i = 0; i < N; i++)
		XpmCreatePixmapFromData(display, win, masks[i], &pix[i], &msk[i], NULL);
	srand(time(NULL));
	loop(pix, msk, 40);
	return 0;
}
