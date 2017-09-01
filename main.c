/*
 * */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>
#include <stdio.h>

Display 		*d = NULL;
XWindowAttributes 	xA;
XButtonEvent		xStart;
XEvent 			xE;

int main(int argc, char *argv[])
{
	// Open connection to display
	d = XOpenDisplay(NULL);
	if(d == NULL)
	{
		fprintf(stderr, "Failed to connect to display!\n");
		return 1; 	//Error1: Failed to connect to display
	}
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
	
	//Set event window (xStart) to nothing
	 xStart.subwindow = None;
	int run = 1;
	 /*For you Confer <3*/
	 do
	 	{
			XNextEvent(d, &xE);
			//Check for events and handle accordingly

			if(xE.type == KeyPress && xE.xkey.keycode == XKeysymToKeycode(d, XK_Escape))
			{
					run--;
			}	
			//Check if a key was pressed inside a window
			if(xE.type == KeyPress && xE.xkey.subwindow != None)
			{	
				//Make window with button pressed active
				XRaiseWindow(d, xE.xkey.subwindow);

			}
			//Check if a window was clicked with a mouse button
			else if(xE.type == ButtonPress && xE.xkey.subwindow!=None)
			{	
				//Store the attributes of the event window into xA (Window Attributes Struct)
				XGetWindowAttributes(d, xE.xkey.subwindow, &xA);
				//Store the button event from the general events into xStart (xButton Event)
				xStart = xE.xbutton;
			}

			else if(xE.type == MotionNotify && xE.xkey.subwindow!=None)
			{
				//Calculating Window Movement, xbutton is mouse button atrributes structure, 
				//xStart is button event when we first clicked on window
				int delta_x = xE.xbutton.x_root - xStart.x_root;	//integer to calculate window movement in x axis
				int delta_y = xE.xbutton.y_root - xStart.y_root;	//integer to calculate window movement in y axis
				
				if(xStart.button == 1)
				//If left button clicked
				XMoveWindow(d, xE.xkey.subwindow, xA.x + delta_x, xA.y + delta_y);
					//Arg1, Main window pointer
					//Arg2, Active window
					//Arg3, New x position of window, calculated by delta_x
					//Arg4, New y position of window, calculated by delta_y

				if(xStart.button == 3)
				//If right button clicked
				{
					if( xA.width + delta_x > 10 && xA.height + delta_y > 10)
					//Makes sure window is a reasonable size
					XResizeWindow(d, xE.xkey.subwindow, xA.width + delta_x, xA.height + delta_y);
				}
			}
			//Checks if mouse let go
			else if(xE.type == ButtonRelease)
			{
				xStart.subwindow = None;		
			}	



}	while(run==1);

	//Free memory (be free my children)
	XCloseDisplay(d);
	return 0;	
}
