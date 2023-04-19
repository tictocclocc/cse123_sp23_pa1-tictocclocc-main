#include "host.h"
#include <assert.h>

void handle_incoming_acks(Host* host) {
    // TODO: Suggested steps for handling incoming ACKs
    //    1) Dequeue the ACK from the data structure that saved ack frames in the handle_incoming_frames function 
    //    2) Compute CRC of the ack frame to know whether it is corrupted
    //    3) Check if the ack is valid i.e. within the window slot 
    //    4) Implement logic as per sliding window protocol to track ACK for what frame is expected,
    //       and what to do when ACK for expected frame is received
}

void handle_input_cmds(Host* host) {
    // TODO: Suggested steps for handling input cmd
    //    1) Dequeue the Cmd from host->input_cmdlist_head
    //    2) Implement fragmentation if the message length is larger than FRAME_PAYLOAD_SIZE
    //    3) Set up the frame according to the protocol
    //    4) Append each frame to host->buffered_outframes_head

    int input_cmd_length = ll_get_length(host->input_cmdlist_head);

    while (input_cmd_length > 0) {
        // Pop a node off and update the input_cmd_length
        LLnode* ll_input_cmd_node = ll_pop_node(&host->input_cmdlist_head);
        input_cmd_length = ll_get_length(host->input_cmdlist_head);

        // Cast to Cmd type and free up the memory for the node
        Cmd* outgoing_cmd = (Cmd*) ll_input_cmd_node->value;
        free(ll_input_cmd_node);
 
        int msg_length = strlen(outgoing_cmd->message) + 1; // +1 to account for null pointer
        if (1) {
             int totalframes = (msg_length - (msg_length % FRAME_PAYLOAD_SIZE))/FRAME_PAYLOAD_SIZE;
            if(msg_length % FRAME_PAYLOAD_SIZE != 0){
                totalframes++;
            } 
            fprintf(stderr,"msg length is %d, payload is %d\n",msg_length,FRAME_PAYLOAD_SIZE);
            // Do something about messages that exceed the frame size
            
            int curr_idx = 0;
            int seq = 0;
            
            while(curr_idx < msg_length){
                
                //fprintf(stderr,"currently on idx %d\n",curr_idx);
                
                Frame* outgoing_frame = malloc(sizeof(Frame));
                assert(outgoing_frame);
                for(int i=0;i<FRAME_PAYLOAD_SIZE;i++){
                    if(curr_idx < msg_length){
                        //fprintf(stderr,"copying idx %d\n",curr_idx);
                        //fprintf(stderr,"%s\n",outgoing_cmd->message);
                        //fprintf(stderr,"char to copy: %c\n",outgoing_cmd->message[curr_idx]);
                        outgoing_frame->data[i] = outgoing_cmd->message[curr_idx];

                    }
                    curr_idx++; 
                }
                outgoing_frame->isAck = 0; 
                outgoing_frame->remaining_msg_bytes = msg_length-curr_idx+1;
                outgoing_frame->src_id = outgoing_cmd->src_id;
                outgoing_frame->dst_id = outgoing_cmd->dst_id;
                outgoing_frame->seq_num = seq;
                outgoing_frame->total_frames = totalframes;
                
                ll_append_node(&host->buffered_outframes_head, outgoing_frame);
                seq++;

            }
            //free(outgoing_cmd->message);
            //free(outgoing_cmd);
            printf(
                "<SEND_%d>: sending messages of length greater than %d is "
                "implemented\n",
                host->id, MAX_FRAME_SIZE);
        } /*else {
            Frame* outgoing_frame = malloc(sizeof(Frame));
            assert(outgoing_frame);
            strcpy(outgoing_frame->data, outgoing_cmd->message);
            outgoing_frame->isAck = 0; 
            outgoing_frame->src_id = outgoing_cmd->src_id;
            outgoing_frame->dst_id = outgoing_cmd->dst_id;
            
            // At this point, we don't need the outgoing_cmd
            free(outgoing_cmd->message);
            free(outgoing_cmd);

            ll_append_node(&host->buffered_outframes_head, outgoing_frame);
        }*/
    }
}

void handle_timedout_frames(Host* host) {
    // TODO: Handle frames that have timed out
    // Check your send_window for the frames that have timed out and append them to outgoing_frames_head queue with a new timeout
}

void handle_outgoing_frames(Host* host) {
    //TODO: The code is incomplete and needs to be changed to have a correct behavior
    //Suggested steps: 
    //1) Within the for loop, check if the window is not full and there's space to send more frames i.e LFS - LAR <= window size
    //2) If there is, pop from the buffered_outframes_head queue and fill your send_window_slot data structure with appropriate fields. 
    //3) Append the popped frame to the host->outgoing_frames_head

    struct timeval curr_timeval;
    gettimeofday(&curr_timeval, NULL);
    long additional_ts = 0; 
    for (int i = 0; i < glb_sysconfig.window_size && ll_get_length(host->buffered_outframes_head) > 0; i++) {
        if(host->send_window[glb_sysconfig.window_size-1].timeout==NULL){
            
            LLnode* ll_outframe_node = ll_pop_node(&host->buffered_outframes_head);
            Frame* outgoing_frame = ll_outframe_node->value; 

            ll_append_node(&host->outgoing_frames_head, outgoing_frame); 
            
            //Set a timeout for this frame
            //NOTE: Each dataframe(not ack frame) that is appended to the 
            //host->outgoing_frames_head has to have a 10ms offset from 
            //the previous frame to enable selective retransmission mechanism. 
            //Already implemented below
            struct timeval* next_timeout = malloc(sizeof(struct timeval));
            memcpy(next_timeout, &curr_timeval, sizeof(struct timeval)); 
            timeval_usecplus(next_timeout, TIMEOUT_INTERVAL_USEC + additional_ts);
            additional_ts += 10000; //ADD ADDITIONAL 10ms

            int j=0;
            while(j<glb_sysconfig.window_size){
                fprintf(stderr, "loop?");
                if(host->send_window[j].timeout==NULL){
                    host->send_window[j].timeout=next_timeout;
                    host->send_window[j].frame=outgoing_frame;
                    //fprintf(stderr, "next timeout: %ld", next_timeout->tv_sec);
                    break;
                }
                j++;
            }

            //free(ll_outframe_node);
        }
    }
}