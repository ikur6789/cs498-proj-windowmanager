/* CS498 Capstone - X11 Window Manager
 * Authors: William Blair, Ian Kurzrock 
 * 09/01/2017 */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>
#include <stdio.h>
#include <stdlib.h>      //Used for exit command

#include "reparent.h"

/**************************************/
/**         Defines/Constants        **/
/**************************************/
#define DEBUG

/**************************************/
/**       Function Declarations      **/
/**************************************/
void openWindow(void);
void setEvents(void);
void processEvents(void);

/**************************************/
/**        Global Variables          **/
/**************************************/
Display 		*d = NULL;
XWindowAttributes 	xA;
XButtonEvent		xStart;
XEvent 			xE;
WMClient        *clientHead = NULL;

/**************************************/
/**        Function Definitions      **/
/**************************************/
void openWindow(void)
{
	// Open connection to display
	d = XOpenDisplay(NULL);
	if(d == NULL)
	{
		fprintf(stderr, "Failed to connect to display!\n");
		exit(1); 	//Error1: Failed to connect to display
	}

	// Say that we want to be able to reparent windows
	XSelectInput(
		d, RootWindow(d, DefaultScreen(d)),
		SubstructureRedirectMask | SubstructureNotifyMask
	);

	// clear current events
	XSync(d, False);
}


void setEvents(void)
{
	//Tell X what events to process
	XGrabKey(d, XKeysymToKeycode(d, XStringToKeysym("F1")), Mod1Mask, DefaultRootWindow(d), True, GrabModeAsync, GrabModeAsync);
		//Arg1, d = main display pointer
		//Arg2, XKeysymToKeycode(), looks for which key to get events from, F1 key to raise a window
		//Arg2.1 XStringToKeysym, reports string version of keyboard event
		//Arg3 Mod1Mask = modifier key (alt, ctrl, shift, etc.)
		//Arg4 DefaultRootWindow(), the default window (background of screen)
		//Arg5 Boolean = True, Ownerevents, determines whether event is sent to the x system
		//Arg6 GrabModeAsync, Which method to handle events for keys
		//Arg7 GrabModeAsync, Which method to handle events for pointer (mouse)
	
	XGrabKey(d, XKeysymToKeycode(d, XStringToKeysym("Escape")), None, DefaultRootWindow(d), True, GrabModeAsync, GrabModeAsync);
		//Arg1, d = main display pointer
		//Arg2, XKeysymToKeycode(), looks for which key to get events from, Escape key for exit
		//Arg2.1 XStringToKeysym, reports string version of keyboard event
		//Arg3 Mod1Mask = modifier key (alt, ctrl, shift, etc.)
		//Arg4 DefaultRootWindow(), the default window (background of screen)
		//Arg5 Boolean = True, Ownerevents, determines whether event is sent to the x system
		//Arg6 GrabModeAsync, Which method to handle events for keys
		//Arg7 GrabModeAsync, Which method to handle events for pointer (mouse)
	
	XGrabButton(d, 1, Mod1Mask, DefaultRootWindow(d), True, ButtonPressMask | ButtonReleaseMask | PointerMotionMask, 
			GrabModeAsync, GrabModeAsync, None, None);
		//Arg1, d = main display pointer
		//Arg2, Mouse button, 1 means left button
		//Arg3, Mod1Mask = modifier key (for alt, ctrl, shift, etc)
		//Arg4, The default window (background)
		//Arg5, Boolean = True, Ownerevents, when true reports owner events to x system
		//Arg6, ButtonPressMask|ButtonReleaseMask|PointerMotionMask, integer values logically OR'd together to specifiy which type of events to get
		//Arg7, GrabModeAsync, Which method to handle for keys
		//Arg8, GrabModeAsync, Which method to handle for pointer (mouse) events
		//Arg9, Window to Confine to, Only handle events in this window, set to none meaning other windows can be controlled
		//Arg10, Cursor, which cursor to draw when this event is happening, none meaning don't change the cursor
		
	XGrabButton(d, 3, Mod1Mask, DefaultRootWindow(d), True, ButtonPressMask | ButtonReleaseMask | PointerMotionMask, 
			GrabModeAsync, GrabModeAsync, None, None);
		//Arg1, d = main display pointer
		//Arg2, Mouse button, 3 means left button
		//Arg3, Mod1Mask = modifier key (for alt, ctrl, shift, etc)
		//Arg4, The default window (background)
		//Arg5, Boolean = True, Ownerevents, when true reports owner events to x system
		//Arg6, ButtonPressMask|ButtonReleaseMask|PointerMotionMask, integer values logically OR'd together to specifiy which type of events to get
		//Arg7, GrabModeAsync, Which method to handle for keys
		//Arg8, GrabModeAsync, Which method to handle for pointer (mouse) events
		//Arg9, Window to Confine to, Only handle events in this window, set to none meaning other windows can be controlled
		//Arg10, Cursor, which cursor to draw when this event is happening, none meaning don't change the cursor
}

/**************************************/
/**     Main - Program Entrypoint    **/
/**************************************/
int main (int argc, char *argv[])
{
	// set when we click on a window to 
	//move it - fixes the issue where you will stop moving 
	//the window if the cursor moves off of it too fast
	Bool moving_window = False; 

	// Create connection to the X server
	openWindow();

	// set which events to handle
	setEvents();

	//Set event window (xStart) to nothing
	xStart.subwindow = None;
	int run = 1;
	/*For you Confer <3*/
	do
	{
		XNextEvent(d, &xE);

#ifdef DEBUG
		if(xE.type == KeyPress && xE.xkey.keycode == XKeysymToKeycode(d, XK_Escape))
		{
				run--;
		}
#endif
		//Check for events and handle accordingly
		switch(xE.type)
		{
			//Check if a key was pressed inside a window
			case(KeyPress): 
			{      
				if(xE.xkey.subwindow != None)
					//Make window with button pressed active
					XRaiseWindow(d, xE.xkey.subwindow);
			}
			break;
			
			//Check if a window was clicked with a mouse button
			case(ButtonPress):
			{	 
				if(xE.xbutton.subwindow!=None)
				{	
					printf("XSubwindow button press!\n");

					// Take command of the mouse cursor, looking for motion and button release events
					XGrabPointer(d, xE.xbutton.subwindow, True, 
						PointerMotionMask|ButtonReleaseMask,
						GrabModeAsync, GrabModeAsync,
						None, None, CurrentTime
					);
					//Store the attributes of the event window into xA (Window Attributes Struct)
					XGetWindowAttributes(d, xE.xbutton.subwindow, &xA);
					//Store the button event from the general events into xStart (xButton Event)
					xStart = xE.xbutton;
					
					//make sure the window isn't below any other windows when it is active
					XRaiseWindow(d, xE.xbutton.subwindow);

					// Raise parent window (its frame)
					Window root, parent, *children = NULL;
					unsigned int numChildren;

					XQueryTree(d, xE.xbutton.subwindow,
					&root, &parent, &children, &numChildren);

					XRaiseWindow(d, parent);

					if(children) XFree(children);
					
					// set that we currently could be moving
					// a window
					//moving_window = True;
				}

				/* raise a window frame if pressed */
				else if(xE.xbutton.window != None )
				{
	             printf("Xbutton window button press!\n");

				XGrabPointer(d, xE.xbutton.window, True, 
					PointerMotionMask|ButtonReleaseMask,
					GrabModeAsync, GrabModeAsync,
					None, None, CurrentTime
				);
	 
	             // make the window frame pressed bool True
	             moving_window = True;
	 
	             /* save current attributes of the active window so
	              * if something like motion happens we have its 
	              * attributes */
	             XGetWindowAttributes(d, xE.xbutton.window, &xA);
	             xStart = xE.xbutton;
	 
	             /* make sure the window isn't below any other windows
	              * when it is active */
	             XRaiseWindow(d, xE.xbutton.window);
	 
	         	}
	

			}
			break;

			case(MotionNotify):
			{	
				if(moving_window)
				{
					//Calculating Window Movement, xbutton is mouse button atrributes structure, 
					//xStart is button event when we first clicked on window
					int delta_x = xE.xbutton.x_root - xStart.x_root;	//integer to calculate window movement in x axis
					int delta_y = xE.xbutton.y_root - xStart.y_root;	//integer to calculate window movement in y axis
				
					//If left button clicked
					if(xStart.button == 1)
						XMoveWindow(d, xE.xmotion.window, xA.x + delta_x, xA.y + delta_y);
							//Arg1, Main window pointer
							//Arg2, Active window
							//Arg3, New x position of window, calculated by delta_x
							//Arg4, New y position of window, calculated by delta_y
					
					//If right button clicked
					else if(xStart.button == 3)
					{
						//Makes sure window is a reasonable size
						if( xA.width + delta_x > 10 && xA.height + delta_y > 10)
							XResizeWindow(d, xE.xkey.subwindow, xA.width + delta_x, xA.height + delta_y);
					}
				}
				
				
			}
			break;

			//Checks if mouse let go
			case(ButtonRelease):
			{
				// we're not moving a window now
				moving_window = False;
				
				// reset the starting window
				xStart.subwindow = None;
				
				// Release command of the cursor
				XUngrabPointer(d, CurrentTime);
			}	
			break;

			case(MapRequest):
			{
				printf("Map Request!\n");
				reparent_window(xE.xmaprequest.window, False);
				XMapWindow(d, xE.xmaprequest.window);
			}
			break;

			case(CreateNotify):
			{
				printf("Create Notify Event!\n");
			}
			break;

			case(ConfigureRequest):
			{
				XWindowChanges changes;
               changes.x = xE.xconfigurerequest.x;
               changes.y = xE.xconfigurerequest.y;
               changes.width = xE.xconfigurerequest.width;
               changes.height = xE.xconfigurerequest.height;
               changes.border_width = xE.xconfigurerequest.border_width;
               changes.sibling = xE.xconfigurerequest.above;
               changes.stack_mode = xE.xconfigurerequest.detail;
               XConfigureWindow(d, xE.xconfigurerequest.window, xE.xconfigurerequest.value_mask, &changes);
               printf("Resize: (%d, %d), X,Y: (%d,%d)\n", xE.xconfigurerequest.width, xE.xconfigurerequest.height, xE.xconfigurerequest.x, xE.xconfigurerequest.y);

			}
			break;

			case(UnmapNotify):
			{
				printf("Unmap Notify Event!\n");
			}
			break;

			case(DestroyNotify):
			{
				printf("Destroy Notify Event!\n");
				/* Because the code below generates another destroy notify event,
				 * and the parent of the frames is the root window, running
				 * this code will destroy the root window and hence shut down
				 * the X server unless we skip it with this */
				if(xE.xdestroywindow.event == RootWindow(d, DefaultScreen(d))){
					printf("Destroywindow.event == Root! NOT destroying!\n");
					break;
				}

				/* find the matching client parent window */
				WMClient *temp = clientHead;
				WMClient *head = temp;
				WMClient *caboose = temp->next;
				while(temp != NULL){
					//if(temp->frame == parent) break;
					if(temp->frame == xE.xdestroywindow.event) break;
					/* keep track of the entry before and after temp */
					head = temp;
					temp = temp->next;
					caboose = temp->next;
				}
				
				/* Destroy the frame of the window (its parent) 
				 * and free memory */
				XDestroyWindow(d, temp->frame);
				free(temp);

				/* reconnect the previous and after
				 * WMclients in the list */
				if(temp != clientHead) head->next = caboose;
				else                   clientHead = caboose;
				
			}
			break;
			
			default:
			{
				printf("Wassup\n");
			}
		}
}	while(run==1);

	//Free memory (be free my children)
	XCloseDisplay(d);
	return 0;	
}
