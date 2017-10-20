#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef DESTROY_H_INCLUDED
#define DESTROY_H_INCLUDED

/* Destroys the frame and children
 * whose close icon matches the sent
 * icon */
Bool destroyClientFromIcon(Window icon);

/* Destroys the frame and children
 * whose frame matches the sent
 * frame */
Bool destroyClientFromFrame(Window frame);

#endif
