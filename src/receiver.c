#include "host.h"
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

void handle_incoming_frames(Host* host) {
    int winsize = glb_sysconfig.window_size;
    // TODO: Suggested steps for handling incoming frames
    //    1) Dequeue the Frame from the host->incoming_frames_head
    //    2) Compute CRC of incoming frame to know whether it is corrupted
    //    3) If frame is corrupted, drop it and move on.
    //    4) If frame is not corrupted, check if it is an ack frame
    //    5) If frame is an ack frame, save it to handle it in handle_incoming_acks
    //    6) Implement logic to check if the expected frame has come
    //    7) Implement logic to combine payload received from all frames belonging to a message
    //       and print the final message when all frames belonging to a message have been received.
    //    8) Implement the cumulative acknowledgement part of the sliding window protocol
    //    9) Append acknowledgement frames to the outgoing_frames_head queue
    int incoming_frames_length = ll_get_length(host->incoming_frames_head);
    while (incoming_frames_length > 0) {
        
        // Pop a node off the front of the link list and update the count
        LLnode* ll_inmsg_node = ll_pop_node(&host->incoming_frames_head);
        incoming_frames_length = ll_get_length(host->incoming_frames_head);

        Frame* inframe = ll_inmsg_node->value; 
        fprintf(stderr,"received frame number %d\n",inframe->seq_num);

         //fprintf(stderr, "hehhhhhh");
        //if new frame is within window
        if(inframe->seq_num < host->currseqnum+glb_sysconfig.window_size){
             //fprintf(stderr, "inside");
            host->rec_window_1[(inframe->seq_num)%winsize].frame = inframe;
            //fprintf(stderr, "here");
            if(host->rec_window_1[(inframe->seq_num)%winsize].timeout == NULL){
                struct timeval* to = malloc(sizeof(struct timeval));
                host->rec_window_1[(inframe->seq_num)%winsize].timeout = to;
            }
            
            host->rec_window_1[(inframe->seq_num)%winsize].timeout->tv_sec = 1;
            

            
        }
        //fprintf(stderr,"where is: %s", host->rec_window_1[0].frame->data);

        //fprintf(stderr,"placed frame %d:%s in window index %d\n",host->rec_window_1[(inframe->seq_num)%winsize].frame->seq_num,host->rec_window_1[(inframe->seq_num)%winsize].frame->data,(inframe->seq_num)%winsize );
        if(host->fullstr == NULL){
            fprintf(stderr,"callocing");
            host->fullstr = calloc(inframe->total_frames, FRAME_PAYLOAD_SIZE);
        }
           // fprintf(stderr,"where is: %s", host->rec_window_1[0].frame->data);
        bool fullwindow = true;
        for(int i=0;i<winsize;i++){
            if(host->rec_window_1[i].frame == NULL){
                fprintf(stderr,"found empty frame\n");
                fullwindow = false;
            }else{
                fprintf(stderr,"found frame\n");
            }
        }
       // fprintf(stderr,"where is: %s", host->rec_window_1[0].frame->data);
        bool msgdone = false;
        int end;
        for(int j=0;j<winsize;j++){
            if(host->rec_window_1[j].frame != NULL && host->rec_window_1[j].frame->seq_num == host->rec_window_1[j].frame->total_frames-1){
                fprintf(stderr,"found end of msg at idx %d\n",j);
                msgdone = true;
                end = j+1;
                break;

            }
            fprintf(stderr, "not end of msg a idx %d\n",j);

        }
      //  fprintf(stderr,"where is: %s", host->rec_window_1[0].frame->data);
        if(fullwindow){
            for(int k=0;k<winsize;k++){
                strcat(host->fullstr, host->rec_window_1[k].frame->data);
                host->rec_window_1[k].frame = NULL;
                host->rec_window_1[k].timeout->tv_sec = NULL;
                //printf("combine <RECV_%d>:[",host->id);
        
                //printf("%s", host->fullstr);
                //printf("]\n");
                //host->fullstr = NULL;

            }
        }else if(msgdone){
            for(int m=0;m<end;m++){
       //         fprintf(stderr,"adding %s", host->rec_window_1[m].frame->data);
                strcat(host->fullstr, host->rec_window_1[m].frame->data);
                host->rec_window_1[m].frame = NULL;
                host->rec_window_1[m].timeout->tv_sec = NULL;
            }
            printf("<RECV_%d>:[",host->id);
        
            printf("%s", host->fullstr);
            printf("]\n");
            host->fullstr = NULL;

        }


        //free(inframe);
        //free(ll_inmsg_node);
    }
}
