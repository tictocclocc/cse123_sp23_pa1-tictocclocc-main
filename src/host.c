#include "host.h"
#include "sender.h"
#include "receiver.h"
#include <assert.h>

void init_host(Host* host, int id) {
    host->id = id;
    host->active = 0; 
    host->awaiting_ack = 0; 
    host->input_cmdlist_head = NULL;
    host->incoming_frames_head = NULL; 
    host->buffered_outframes_head = NULL; 
    host->buffered_inframes_head_1 = NULL; 
    host->outgoing_frames_head = NULL; 
    host->buffered_inacks_head = NULL;
    host->buffered_timeouts = NULL;
    host->send_window = calloc(glb_sysconfig.window_size, sizeof(struct send_window_slot));
    host->rec_window_1 = calloc(glb_sysconfig.window_size, sizeof(struct send_window_slot));  
    for (int i = 0; i < glb_sysconfig.window_size; i++) {
        host->send_window[i].frame = NULL;
        host->send_window[i].timeout = NULL;
    }
    host->fullstr = NULL;

    //host->parts = NULL;
    host->currseqnum = 0;
    // TODO: You should fill in this function as necessary to initialize variables
}

struct timeval* host_get_next_expiring_timeval(Host* host) {
    // TODO: You should fill in this function so that it returns the 
    // timeval when next timeout should occur
    // 
    // 1) Check your send_window for the timeouts of the frames. 
    // 2) Return the timeout of a single frame. 
    // HINT: It's not the frame with the furtherst/latest timeout. 
    return NULL;
}

void run_hosts() {
    struct timeval curr_timeval;
    gettimeofday(&curr_timeval, NULL);

    int i; 
    for (i = 0; i < glb_num_hosts; i++) {
        Host* host = &glb_hosts_array[i]; 

        // Check whether anything has arrived
        int input_cmd_length = ll_get_length(host->input_cmdlist_head);
        int inframe_queue_length = ll_get_length(host->incoming_frames_head);
        struct timeval* next_timeout = host_get_next_expiring_timeval(host); 
        
        // Conditions to "wake up" the host:
        //    1) Acknowledgement or new command
        //    2) Timeout      
        int incoming_frames_cmds = (input_cmd_length != 0) | (inframe_queue_length != 0); 
        long reached_timeout = (next_timeout != NULL) && (timeval_usecdiff(&curr_timeval, next_timeout) <= 0);

        host->awaiting_ack = 0; 
        host->active = 0; 

        if (incoming_frames_cmds || reached_timeout) {
            // Implement this
            handle_input_cmds(host);

            // Implement this
            handle_incoming_frames(host); 

            // Implement this
            handle_incoming_acks(host);
            
            // Implement this
            handle_timedout_frames(host);

            // Implement this
            handle_outgoing_frames(host); 
        }
        
        //Condition to indicate that the host is still waiting for an ack
        for (int j = 0; j < glb_sysconfig.window_size; j++) {
            if (host->send_window[j].frame != NULL) {
                host->awaiting_ack = 1; 
                break; 
            }
        }

        //Condition to indicate that the host is active 
        if (host->awaiting_ack || ll_get_length(host->buffered_outframes_head) > 0) {
            host->active = 1; 
        }
    }
 
}