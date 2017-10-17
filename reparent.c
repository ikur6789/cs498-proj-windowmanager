/* implementation file for
 * reparenting windows when they send a map request 
 * (want to be displayed on the screen) */

#include "reparent.h"
#include <X11/Xutil.h>
#include <X11/xpm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// counter and Windows to be used as window borders
//int frames_index=0;
//Window frames[10];

// global variables
Pixmap minPixmap; // minimize image
Pixmap maxPixmap; // maximize image
Pixmap unmaxPixmap; // unmaximize image
Pixmap closePixmap; // close window image

// variables from main.c
extern Display *d;
extern WMClient *clientHead;/* the start of the client linked list */

/* creates a frame for the input child window and reparents
 * it to the created frame */
Bool reparent_window(Window child, Bool before_wm)
{
	XWindowAttributes a; // get info about the child window to create
	                     // its border

	WMClient *c; // placeholder for the new entry in the window client list
	  
	/* moved as defines in reparent.h; used to resize windows */                   
	//const int border_width = 2; // border size of the parent window
	//const int title_height = 20; // size of the title bar
	
	/* get child information */
	XGetWindowAttributes(d, child, &a);
	
	/* exit if we have too many windows cuz no linked list yet */
	/*if(frames_index >= 10){
		fprintf(stderr, "MORE THAN 10 Windows AAAAHHH!\n");
		exit(0);
	}*/

	/* create a new entry in the list */
	if(clientHead == NULL) {
		clientHead = (WMClient *)malloc(sizeof(WMClient));
		/* TODO - test for NULL */

		c = clientHead;
	}
	else {
		/* get the last entry in the client list */
		WMClient *temp = clientHead;
		while(temp->next != NULL)
			temp = temp->next;

		c = (WMClient *)malloc(sizeof(WMClient));
		/* TODO - test for NULL */

		/* assign the new client structure to the end of the list */
		temp->next = c;
	}
	c->next  = NULL;
	c->child = child;
    c->maximized = False;
	c->w= c->h = c->x = c->y=0; // initialize the position variables
	
	/* create the border window */
	//frames[frames_index] = XCreateSimpleWindow(d,                                  // Display *d
	c->frame = XCreateSimpleWindow(d,
	                             RootWindow(d, DefaultScreen(d)),    // Display *parent
	                             0,                                // x coord
	                             0,                                // y coord
	                             a.width+(BORDER_WIDTH),           // window width
	                             a.height+TITLE_HEIGHT,              // window height
	                             BORDER_WIDTH,                       // border size
	                             WhitePixel(d, DefaultScreen(d)),    // border
	                             BlackPixel(d, DefaultScreen(d)));   // background
                                 
    XWindowAttributes wa;
    XGetWindowAttributes(d, c->frame, &wa);
    printf("Override redirect: %d\n", wa.override_redirect);
	
	/* select events on the frame */
	XSelectInput( d, 
	              //frames[frames_index], 
	              c->frame,
	              SubstructureRedirectMask | SubstructureNotifyMask );
    
    /* Create each button window */
    c->minWin = XCreateSimpleWindow(d,
	                             c->frame,    // Display *parent
	                             (a.width+BORDER_WIDTH)-(BUTTON_SIZE*3), // x coord
	                             0,                                // y coord
	                             BUTTON_SIZE,           // window width
	                             BUTTON_SIZE,              // window height
	                             0,                       // border size
	                             WhitePixel(d, DefaultScreen(d)),    // border
	                             BlackPixel(d, DefaultScreen(d)));   // background
	c->maxWin = XCreateSimpleWindow(d,
	                             c->frame,    // Display *parent
	                             (a.width+BORDER_WIDTH)-(BUTTON_SIZE*2), // x coord
	                             0,                                // y coord
	                             BUTTON_SIZE,           // window width
	                             BUTTON_SIZE,              // window height
	                             0,                       // border size
	                             WhitePixel(d, DefaultScreen(d)),    // border
	                             BlackPixel(d, DefaultScreen(d)));   // background      
    c->closeWin = XCreateSimpleWindow(d,
                                    c->frame,    // Display *parent
                                    (a.width+BORDER_WIDTH)-(BUTTON_SIZE*1), // x coord
                                    0,                                // y coord
                                    BUTTON_SIZE,           // window width
                                    BUTTON_SIZE,              // window height
                                    0,                       // border size
                                    WhitePixel(d, DefaultScreen(d)),    // border
                                    BlackPixel(d, DefaultScreen(d)));   // background        
    /* give each button window their image */
    XSetWindowBackgroundPixmap(d, c->minWin, minPixmap);
    XSetWindowBackgroundPixmap(d, c->maxWin, maxPixmap);
    XSetWindowBackgroundPixmap(d, c->closeWin, closePixmap);
	/* restores the child if we crash somehow */
	XAddToSaveSet(d, child);
	
	/* assuming last thing needed to do */
	XReparentWindow(d,                        // Display *d 
	                child,                    // Window w
	                //frames[frames_index],     // Window parent
	                c->frame,
	                //BORDER_WIDTH-(BORDER_WIDTH/2),             // int x - x position in new parent window
	                0,             // int x - x position in new parent window
	                TITLE_HEIGHT);            // int y - y position in new parent window


	  // 9. Grab universal window management actions on client window.
  //   a. Move windows with alt + left button.
  XGrabButton(
      d,
      Button1,
      None,
      child,
      False,
      ButtonPressMask | ButtonReleaseMask | ButtonMotionMask,
      GrabModeAsync,
      GrabModeAsync,
      None,
      None);
/*
  //   b. Resize windows with alt + right button.
  XGrabButton(
      d,
      Button3,
      Mod1Mask,
      child,
      False,
      ButtonPressMask | ButtonReleaseMask | ButtonMotionMask,
      GrabModeAsync,
      GrabModeAsync,
      None,
      None);
  //   c. Kill windows with alt + f4.
  XGrabKey(
      d,
      XKeysymToKeycode(d, XK_F4),
      Mod1Mask,
      child,
      False,
      GrabModeAsync,
      GrabModeAsync);
  //   d. Switch windows with alt + tab.
  XGrabKey(
      d,
      XKeysymToKeycode(d, XK_Tab),
      Mod1Mask,
      child,
      False,
      GrabModeAsync,
      GrabModeAsync);
*/
// Grab input to frame
XGrabButton(
    d,
    1,
    None,
    //frames[frames_index],
	c->frame,
    //False,
    True,
    //ButtonPressMask | ButtonReleaseMask | ButtonMotionMask | PointerMotionMask,
    //ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
    ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
    GrabModeAsync,
    GrabModeAsync,
    //None,
    //frames[frames_index],
	c->frame,
    None
);

    /* test loading a button window */
    //loadPixmap("files/close.xpm");


	/* map the parent window */
	//XMapWindow(d, frames[frames_index]);
	//XMapWindow(d, c->frame);
    XMapWindow(d, c->frame); //map the frame and the buttons
    XMapSubwindows(d, c->frame);
	//frames_index++;
	return True;
}

/* test loading a pixmap */
//Window loadPixmap(const char *filename)
Pixmap loadPixmap(const char *filename)
{
    Pixmap p;
    Pixmap mask;
    XpmAttributes xpmattribs;
    
    Window w;
    
    printf("Before create simple window!\n");
    
    /* create the window */
    w = XCreateSimpleWindow(
        d,
        RootWindow(d, DefaultScreen(d)),
        0,0, // x, y
        10,10, // w, h - can't be 0,0 or error
        0,   // border width,
        BlackPixel(d, DefaultScreen(d)),
        WhitePixel(d, DefaultScreen(d))
    );
    
    /* set pixmap attributes */
    xpmattribs.visual = DefaultVisual(d, DefaultScreen(d));
    xpmattribs.depth = DefaultDepth(d, DefaultScreen(d));
    xpmattribs.colormap = DefaultColormap(d, DefaultScreen(d));
    xpmattribs.valuemask = XpmDepth | XpmColormap | XpmVisual;
    
    printf("Before XpmReadFileToPixmap!\n");

    /* load the pixmap */
    if( XpmReadFileToPixmap(
            d,
            RootWindow(d, DefaultScreen(d)),
            filename,
            &p,
            &mask,
            &xpmattribs
        ) != XpmSuccess )
    {
        fprintf(stderr, "Failed to open pixmap %s\n", filename);
        //return (Window)0; // so you can test with if !Window
        return False;
    }
    
    printf("Before if mask free mask!\n");
    /* free the mask pixmap if not used */
    if(mask) {
        XFreePixmap(d, mask);
    }
    
    //printf("Before XSetWindowBackground!\n");
    
    /* draw the image to the window */
    //XSetWindowBackgroundPixmap(d, w, p);
    
    //printf("Before Resize Window!\n");
    
    /* resize the window to fit the image size */
    //XResizeWindow(d, w, xpmattribs.width, xpmattribs.height);
    
    //printf("Before XMapWindow!\n");
    
    /* make the window visible */
    //XMapWindow(d, w);
    
    /* debug test */
    //sleep(10);
    
    //XFreePixmap(d, p);
    //XDestroyWindow(d, w);
    //return (Window)0;
    //return Window;
    return p;
}

Bool reparentLoadPixmaps(const char *minimizePixmapName,
                         const char *maximizePixmapName,
                         const char *unmaxPixmapName,
                         const char *closePixmapName)
{
    /* TODO - error check */
    minPixmap   = loadPixmap(minimizePixmapName);
    maxPixmap   = loadPixmap(maximizePixmapName);
	unmaxPixmap = loadPixmap(unmaxPixmapName);
    closePixmap = loadPixmap(closePixmapName);
    
    return True;
}

void reparentClosePixmaps(void)
{
    if(minPixmap) XFreePixmap(d, minPixmap);
    if(maxPixmap) XFreePixmap(d, maxPixmap);
	if(unmaxPixmap) XFreePixmap(d, unmaxPixmap);
    if(closePixmap) XFreePixmap(d, closePixmap);
}
