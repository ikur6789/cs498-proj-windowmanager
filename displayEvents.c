/* For when windows are drawn to the screen */

#include "displayEvents.h"

// variables from main.c
extern Display *d;
extern Window task_win2;
extern WMClient *clientHead;

Bool hCreateNotify(const XCreateWindowEvent e)
{
    printf("Create Notify Event!\n");
    
    return True;
}

Bool hReparentNotify(const XReparentEvent e)
{
    printf("Reparent Notify Event!\n");
    
    return True;
}

Bool hMapNotify(const XMapEvent e)
{
    printf("Map Notify Event!\n");
    
    return True;
}

Bool hMapRequest(const XMapRequestEvent e)
{
    printf("Map Request Event!\n");
    
    reparentWindow(e.window, False);
    
    XMapWindow(d, e.window);
	
    WMClient *temp = clientHead;
    XWindowAttributes fAttribs;
	while(temp!=NULL)
	{

		XGetWindowAttributes(d, temp->frame, &fAttribs);
		XDrawString(
		d,
		temp->titleBar,
		DefaultGC(d, DefaultScreen(d)),
		(fAttribs.width / 2)- strlen(temp->title)*CHAR_WIDTH, 
		(TITLE_HEIGHT / 2) + CHAR_WIDTH/2,
		temp->title,
		strlen(temp->title)
    		);

		temp=temp->next;
	}
    
    return True;
}

Bool hExpose(const XExposeEvent e)
{
    printf("Expose Event!\n");

    WMClient *temp = clientHead;
    XWindowAttributes fAttribs;
	while(temp!=NULL)
	{

		XGetWindowAttributes(d, temp->frame, &fAttribs);
		XDrawString(
		d,
		temp->titleBar,
		DefaultGC(d, DefaultScreen(d)),
		(fAttribs.width / 2)- strlen(temp->title)*CHAR_WIDTH, 
		(TITLE_HEIGHT / 2) + CHAR_WIDTH/2,
		temp->title,
		strlen(temp->title)
    		);

		temp=temp->next;
	}
    return True;
	
}
