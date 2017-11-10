/* CS498 Capstone - X11 Window Manager
 * Authors: William Blair, Ian Kurzrock 
 * 09/01/2017 */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>
#include <stdio.h>
#include <stdlib.h>      //Used for exit command
#include <string.h>
#include "reparent.h"
#include "destroy.h"

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
Window start_taskbar(Window pass);
Window start_window(Window pass, Window task_bar, int x_pos, unsigned long color);

/**************************************/
/**        Global Variables          **/
/**************************************/
Display 			*d = NULL;
XWindowAttributes 	xA;
XButtonEvent		xStart;
XEvent 				xE;
WMClient        	*clientHead = NULL;
GC 					gc_taskbar;
Window 				task_bar; //X11 window for taskbar


/* files in reparent.c */
// global variables
//extern Pixmap minPixmap; // minimize image
extern Pixmap maxPixmap; // maximize image
extern Pixmap unmaxPixmap; // unmaximize image
//extern Pixmap closePixmap; // close window image

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
    
    // True when we maximize a window, otherwise
    // Configure Notify will run when we move the window
    Bool maximize_window = False;
    
    // True when minimizing a window, so we can run in
    // ClientMessage without messing up other event types
    Bool minimizing = False;

	// Create connection to the X server
	openWindow();

	// set which events to handle
	setEvents();

    // DEBUG - bj test
    //loadPixmap("files/close.xpm");
    reparentLoadPixmaps("files/minimize.xpm",
                        "files/maximize.xpm",
	                    "files/unmaximize.xpm",
                        "files/close.xpm");
                        

	//Set event window (xStart) to nothing
	xStart.subwindow = None;
	
	task_bar = start_taskbar(task_bar);
	Window task_win;
	task_win = start_window(task_win, task_bar, 1, 0x4286f4);
	Window task_win2 = start_window(task_win, task_bar, 50, 0x0abcde);
	XDrawString(d, task_win2, DefaultGC(d, DefaultScreen(d)), 0, 0, "Win 1", strlen("Win 1"));
	
	//Drawing a rectangle to the taskbar for testing purposes
	GC window_min = DefaultGC(d, DefaultScreen(d));
	XGCValues send_vals;
	send_vals.fill_rule=FillSolid;
	send_vals.foreground=1;
	XChangeGC(d, window_min, GCForeground, &send_vals);
	XFillRectangle(d, task_bar, window_min, 25, 5, 20, 20);
	XSelectInput(d, task_bar, 0);
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
                    printf("X: %d, Y: %d\n", xE.xbutton.x, xE.xbutton.y);
                    printf("X root: %d, Y root: %d\n", xE.xbutton.x_root, xE.xbutton.y_root);
                    
                    XWindowAttributes winFrameAttribs;
					Window root, parent, *children = NULL;
					unsigned int numChildren;

					XQueryTree(d, xE.xbutton.subwindow,
					&root, &parent, &children, &numChildren);
                    
                    XGetWindowAttributes(
                        d, 
                        parent, 
                        &winFrameAttribs
                    );
                    
                    /* do if y_root < 20, and x within (window width - button_size),
                     * perform button action */
                    if(xE.xbutton.y < BUTTON_SIZE && // if the y coordinate isn't on the window border and is within the window frame height
                       xE.xbutton.y > BORDER_WIDTH &&
                       xE.xbutton.x > winFrameAttribs.width - BUTTON_SIZE*3 // if the mouse x is far enough over
                       )
                    {
                        printf("In button events area!\n");
                        
                        /* minimize */
                        if(xE.xbutton.x < winFrameAttribs.width - BUTTON_SIZE*2){
                            printf("Minimize area!\n");
                            
                            
                            /* Get current window size/position and store in client */
							WMClient *temp = clientHead;
							while(temp != NULL)
							{
								if(temp->minWin == xE.xbutton.subwindow) break;
								temp = temp->next;
							}
							if(temp != NULL){
								XWindowAttributes winAttribs;
								XGetWindowAttributes(d, temp->frame, &winAttribs);
                                
                                /* store current position */
                                temp->x = winAttribs.x;
                                temp->y = winAttribs.y;
                                temp->w = winAttribs.width;
                                temp->h = winAttribs.height;
                                
                                /* Iconify the window */
                                XIconifyWindow(d, parent, DefaultScreen(d));
                                printf("After iconify window!\n");
                                
                                /* Set minimizing so we will run the
                                 * code in ClientMessage */
                                minimizing = True;
                                
                            }
                            else{
                                printf("Failed to find client with the same minimize icon!\n");
                            }
                            
                            // don't do the raise window code and stuff
                            break;
                        }
                        /* maximize */
                        else if(xE.xbutton.x >= winFrameAttribs.width - BUTTON_SIZE*2&&
                                xE.xbutton.x < winFrameAttribs.width - BUTTON_SIZE*1
                               )
                        {
                            printf("Maximize area!\n");
                            
                            /* Get current window size/position and store in client */
							WMClient *temp = clientHead;
							while(temp != NULL)
							{
								if(temp->maxWin == xE.xbutton.subwindow) break;
								temp = temp->next;
							}
							if(temp != NULL){
								XWindowAttributes winAttribs;
								XGetWindowAttributes(d, temp->frame, &winAttribs);
                                if(!temp->maximized)
                                {
                                    temp->x = winAttribs.x;
                                    temp->y = winAttribs.y;
                                    temp->w = winAttribs.width;
                                    temp->h = winAttribs.height;

                                    printf("Stored attributes: (%d,%d,%d,%d)\n", temp->x, temp->y, temp->w, temp->h);

                                    /* Draw the unmaximize icon instead */
                                    //XClearWindow(d, temp->maxWin);
                                    //XSetWindowBackgroundPixmap(d, temp->maxWin, unmaxPixmap);
                                    //XFlush(d);	
                                    
                                    /* Maximize the window */
                                    XMoveResizeWindow(
                                        d, 
                                        parent,
                                        0, 0,    // x, y
                                        WidthOfScreen(DefaultScreenOfDisplay(d)) - BORDER_WIDTH*2, // w, h
                                        HeightOfScreen(DefaultScreenOfDisplay(d)) - BORDER_WIDTH*2
                                    );
                                }
                                else
                                {
                                    /* Restore the previous size and position of the
                                     * window before it was maximized */
                                    XMoveResizeWindow(
                                        d, 
                                        parent,
                                        temp->x, temp->y,    // x, y
                                        temp->w, temp->h // w, h
                                    );
                                }
                                
                                    // Signal in the configure notify event we want this to run
                                    maximize_window = True;
                                    // switch the status of the window's maximize status
                                    temp->maximized = !temp->maximized;
                            }
							else{
								printf("Failed to find window with same close icon!\n");
							}
                            
                        }
                        /* close */
                        else if(xE.xbutton.x >= winFrameAttribs.width - BUTTON_SIZE*1&&
                                xE.xbutton.x < winFrameAttribs.width - BORDER_WIDTH
                               ){
                            printf("Close area!\n");
                            
                            /* the subwindow is the close icon */
                            destroyClientFromIcon(xE.xbutton.subwindow);
                            
                            /* Get and ignore all of the events generated
                             * by destroying the windows - an unmap and
                             * destroy notify for each window (3 icons+child window + frame window)*/
                            
                            int i=0;
                            for(i=0;i<10;i++)
                            {
                                printf("Event %d: %s\n", i, (xE.type == DestroyNotify) ? "Destroy Notify" : "UnmapNotify");
                                XNextEvent(d, &xE);
                            }
                            break;
                        }
                    }
                    //printf("Before xgrabpointer!\n");
					// Take command of the mouse cursor, looking for motion and button release events
					/*XGrabPointer(d, xE.xbutton.subwindow, True, 
						PointerMotionMask|ButtonReleaseMask,
						GrabModeAsync, GrabModeAsync,
						None, None, CurrentTime
					);*/
                    
                    printf("Before XGetWindowAttributes!\n");
					//Store the attributes of the event window into xA (Window Attributes Struct)
					XGetWindowAttributes(d, xE.xbutton.subwindow, &xA);
					//Store the button event from the general events into xStart (xButton Event)
					xStart = xE.xbutton;
					
					//make sure the window isn't below any other windows when it is active
					//XRaiseWindow(d, xE.xbutton.subwindow);
                    
                    // Raise parent window (its frame)

					// raise the window then its child
                    //printf("Before XRaiseWindow!\n");
					XRaiseWindow(d, parent);
					//XRaiseWindow(d, xE.xbutton.subwindow);
                    //printf("After XRaiseWindow!\n");
					if(children) XFree(children);
					
					// set that we currently could be moving
					// a window
					//moving_window = True;
				}

				/* raise a window frame if pressed */
				else if(xE.xbutton.window != None )
				{
	             printf("Xbutton window button press!\n");
                 printf("X: %d, Y: %d\n", xE.xbutton.x, xE.xbutton.y);

				/*XGrabPointer(d, xE.xbutton.window, True, 
					PointerMotionMask|ButtonReleaseMask,
					GrabModeAsync, GrabModeAsync,
					None, None, CurrentTime
				);*/
	 
	             // make the window frame pressed bool True
	             moving_window = True;
	 
	             /* save current attributes of the active window so
	              * if something like motion happens we have its 
	              * attributes */
	             XGetWindowAttributes(d, xE.xbutton.window, &xA);
	             xStart = xE.xbutton;
                 //xStart = xE.xbutton.subwindow;
	 
	             /* make sure the window isn't below any other windows
	              * when it is active */
	             XRaiseWindow(d, xE.xbutton.window);
                 //XRaiseWindow(d, xE.xbutton.subwindow);
				
				// Figure out the main child window
				WMClient *temp = clientHead;
				while(temp != NULL)
				{
					if(temp->frame == xE.xbutton.window){
						printf("Found frame child!\n");
						break;
					}
					temp = temp->next;
				}
				/* raise the main child window instead of the frame */
				if(temp != NULL){
					//XRaiseWindow(d, temp->child);
					//printf("Raised frame child!\n");
				}
	 
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
               
                /* reconfigure child windows */
                Window root, parent, *children;
                unsigned int nchildren;
                XQueryTree(
                    d,
                    xE.xconfigurerequest.window,
                    &root,
                    &parent,
                    &children,
                    &nchildren
                );
                int i;
                for(i=0; i<nchildren; i++)
                {
                    printf("Child: %d\n", i);
                }
                
                if(children) XFree(children);

			}
			break;
            
            case(ConfigureNotify):
            {
                printf("Configure notify request!\n");
                if(maximize_window)
                {
                    printf("Maximize window happened, resetting boolean!\n");
                    maximize_window = False;
                    
                    if(xE.xconfigure.window == None)
                    {
                        printf("Configure window None!\n");
                        break;
                    }
                    
                    /* Resize the children */
                    
                    /* Find the client matching the frame window maximized */
                    WMClient *temp = clientHead;
                    while(temp->next != NULL)
                    {
                        if(temp->frame == xE.xconfigure.window) break;
                        temp = temp->next;
                    }
                    if(temp == NULL){
                        printf("Configure Request: temp window NULL!\n");
                        break;
                    }
                    if(temp->frame != xE.xconfigure.window){
                        printf("Failed to find configure request frame!\n");
                        break;
                    }
                    /* move the icons */
                    XWindowAttributes frameAttribs;
                    XGetWindowAttributes(d, temp->frame, &frameAttribs);
                    XMoveWindow(d, temp->closeWin, frameAttribs.width-BUTTON_SIZE*1, 0);
                    XMoveWindow(d, temp->maxWin, frameAttribs.width-BUTTON_SIZE*2, 0);
                    XMoveWindow(d, temp->minWin, frameAttribs.width-BUTTON_SIZE*3, 0);
                    /* resize the child window */
                    XResizeWindow(d, temp->child, frameAttribs.width,frameAttribs.height);
                }
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
				//WMClient *temp = clientHead;
				//WMClient *head = temp;
				//WMClient *caboose = temp->next;
                

				//while(temp != NULL){

					//if(temp->frame == parent) break;
				//	if(temp->frame == xE.xdestroywindow.event) break;
					/* keep track of the entry before and after temp */
				//	head = temp;
				//	temp = temp->next;
				//	if(temp != NULL) caboose = temp->next;
                 //   else             caboose = NULL;
				//}
                

                
                //if(temp != NULL){
                    /* Destroy the frame of the window (its parent) 
				 * and free memory */
				//XDestroyWindow(d, temp->frame);
                //XDestroy
                 //   printf("Before destroy subwindows!\n");
                 //   XDestroySubwindows(d, temp->frame);
                    
                 //   printf("Before destroy frame!\n");
                 //   XDestroyWindow(d, temp->frame);
                 //   printf("After destroy frame!\n");
                
                    /* reconnect the previous and after
                     * WMclients in the list */

                 //   if(temp != clientHead) head->next = caboose;
                 //   else                   clientHead = caboose;

                    //free(temp);

                //}
                //else{

                //}
                destroyClientFromFrame(xE.xdestroywindow.event);
				
			}
			break;
            
            case(Expose):
            {
                printf("Expose Event!\n");
                XDrawString(d, task_win2, DefaultGC(d, DefaultScreen(d)), 5, 15, "Win 1", strlen("Win 1"));
            }
            break;
            
            case(ClientMessage):
            {
                printf("Client Message!\n");
                
                /* TODO - make sure this is the right
                 * event type */
                // EDIT - or be lazy and add another boolean
                if(minimizing) 
                {
                    /* reset the boolean */
                    minimizing = False;
                    
                    /* Find the client matching the frame window maximized */
                    WMClient *temp = clientHead;
                    while(temp->next != NULL)
                    {
                        if(temp->frame == xE.xclient.window) break;
                        temp = temp->next;
                    }
                    if(temp != NULL) 
                    {
                        /* Make the windows invisible */
                        XUnmapSubwindows(d, temp->frame);
                        XUnmapWindow(d, temp->frame);
                        
                        /* set minimized to True*/
                        temp->minimized = True;
                    }
                    else{
                        printf("Failed to find client message (minimize) window!\n");
                    }
                }
            }
            break;
			
			default:
			{
				printf("Wassup\n");
                printf("Event type: %d\n", xE.type);
			}
		}
}	while(run==1);

	//Free memory (be free my children)
    reparentClosePixmaps();
	XCloseDisplay(d);
	return 0;	
}

Window start_taskbar(Window pass)
{
	pass=XCreateSimpleWindow(d, DefaultRootWindow(d),0,(HeightOfScreen(DefaultScreenOfDisplay(d))-25),(WidthOfScreen(DefaultScreenOfDisplay(d))),25, 0, BlackPixel(d,DefaultScreen(d)), WhitePixel(d, DefaultScreen(d)));
	XSelectInput(d, pass, ExposureMask|ButtonPressMask|KeyPressMask);
	gc_taskbar=XCreateGC(d, pass, 0,0);
	XSetBackground(d, gc_taskbar, WhitePixel(d, DefaultScreen(d)));
	XSetForeground(d, gc_taskbar, BlackPixel(d, DefaultScreen(d)));
	XClearWindow(d, pass);
	XMapRaised(d, pass);

	return pass;
}

Window start_window(Window pass, Window task_bar, int x_pos, unsigned long color)
{
	XWindowAttributes 	get_task_attrbs;
	XGetWindowAttributes(d, task_bar, &get_task_attrbs);
	unsigned task_win_h = ((get_task_attrbs.height*3)/4);
	printf("\nHeight of taskbar %u\n", task_win_h);
	pass = XCreateSimpleWindow(d, task_bar, x_pos, ((get_task_attrbs.height)/4), 40, task_win_h, 0, 0, color);
	XMapWindow(d, pass);

	return pass;
}
