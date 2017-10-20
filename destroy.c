#include "destroy.h"
#include "reparent.h"

// Variables from main.c
extern Display *d;
extern WMClient *clientHead;/* the start of the client linked list */

Bool destroyClientFromIcon(Window icon)
{
    WMClient *temp = clientHead;
    WMClient *head = temp;
    WMClient *caboose = temp->next;
    
    /* Make temp equal to the client with the sent child */
    while(temp != NULL){

        //if(temp->frame == parent) break;
        if(temp->closeWin == icon) break;
        /* keep track of the entry before and after temp */
        head = temp;
        temp = temp->next;
        if(temp != NULL) caboose = temp->next;
        else             caboose = NULL;
    }
    
    /* If we found a matching client with the given child */
    if(temp != NULL){
        /* Destroy the frame of the window (its parent) 
         * and free memory */
        printf("Before destroy subwindows!\n");
        XDestroySubwindows(d, temp->frame);
        
        printf("Before destroy frame!\n");
        XDestroyWindow(d, temp->frame);
        printf("After destroy frame!\n");
    
        /* reconnect the previous and after
         * WMclients in the list */

        if(temp != clientHead) head->next = caboose;
        else                   clientHead = caboose;

        free(temp);

    }
    /* We didn't find the child, send error */
    else{
        printf("Destroy.c: Failed to find Client with sent child!\n");
    }
    
    return True;
}

Bool destroyClientFromFrame(Window frame)
{
    WMClient *temp = clientHead;
    WMClient *head = temp;
    WMClient *caboose = temp->next;
    
    /* Make temp equal to the client with the sent child */
    while(temp != NULL){

        //if(temp->frame == parent) break;
        //if(temp->closeWin == icon) break;
        //if(temp->child == child) break;
        if(temp->frame == frame) break;
        /* keep track of the entry before and after temp */
        head = temp;
        temp = temp->next;
        if(temp != NULL) caboose = temp->next;
        else             caboose = NULL;
    }
    
    /* If we found a matching client with the given child */
    if(temp != NULL){
        /* Destroy the frame of the window (its parent) 
         * and free memory */
        printf("Before destroy subwindows!\n");
        XDestroySubwindows(d, temp->frame);
        
        printf("Before destroy frame!\n");
        XDestroyWindow(d, temp->frame);
        printf("After destroy frame!\n");
    
        /* reconnect the previous and after
         * WMclients in the list */

        if(temp != clientHead) head->next = caboose;
        else                   clientHead = caboose;

        free(temp);

    }
    /* We didn't find the child, send error */
    else{
        printf("Destroy.c: Failed to find Client with sent child!\n");
    }
    
    return True;
}
