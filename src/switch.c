#include "switch.h"
//*********************************************************************
// NOTE: We will overwrite this file, so whatever changes you put here
//      WILL NOT persist
//*********************************************************************


void init_ingress_ports() {
    for (int i = 0; i < glb_num_hosts; i++) {
        Ingress* ings_port = &glb_ingress_ports_array[i]; 
        ings_port->buffer_queue_head = NULL; 
        ings_port->buffer_queue_size = 0; 
    }
    INGRESS_PORT_QUEUE_CAPACITY = glb_sysconfig.window_size * 2; 
}

void init_egress_ports() {
    for (int i = 0; i < glb_num_hosts; i++) {
        Egress* egs_port = &glb_egress_ports_array[i]; 
        egs_port->buffer_queue_head = NULL; 
        egs_port->buffer_queue_size = 0; 
    }
}

// ***************** SECTION FOR TESTING *********************

//Arrays that count the number of packets sent and received
// int* host_out_dataframe_num; 
// int* host_out_ackframe_num; 
// int* host_in_dataframe_num; 
// int* host_in_ackframe_num; 
// int* host_out_dataframes_num_since_ack_received;

//Use this buffer to test out of order frames 
// Frame *backup_buffer[20];

//Use this timeval to test any timing requirements between packets
// struct timeval previous_packet;

// Use this function for your fine-grained testing initialization
void init_test_setup() {
//     //TODO: ADD more testing fields
//     host_out_dataframe_num = calloc(glb_num_hosts, sizeof(int)); 
//     host_out_ackframe_num = calloc(glb_num_hosts, sizeof(int)); 
//     host_in_dataframe_num = calloc(glb_num_hosts, sizeof(int)); 
//     host_in_ackframe_num = calloc(glb_num_hosts, sizeof(int)); 
//     host_out_dataframes_num_since_ack_received = calloc(glb_num_hosts, sizeof(int)); 
}
// **************** END OF TESTING SECTION ********************

/* 
* This functions returns a random sequence of numbers from 0 to len. 
* It is used to randomly choose the host_order of frames that are sent out 
* from the outgoing_frames_head queues. 
*/

void get_rand_seq(int len, int* host_order) {
    int i; 

    for (i = 0; i < len; i++) {
        host_order[i] = i; 
    }

    for (i = 0; i < len; i++) {
        int rand_idx_1 = rand() % len; 
        int rand_idx_2 = rand() % (i + 1); 

        int temp = host_order[rand_idx_1]; 
        host_order[rand_idx_1] = host_order[rand_idx_2];
        host_order[rand_idx_2] = temp;
    }
}

/* 
* This method is used to unicast the frame from the src to the dst. 
*/
void send_frame(Frame* incoming_frame, double glb_corrupt_prob) {
    uint8_t dst_id = incoming_frame->dst_id;

    // this ensures that only MAX_FRAME_SIZE bytes are sent via send_frame(). Bytes exceeding that are not sent.
    char* recv_char_buffer = convert_frame_to_char(incoming_frame);
    
    // Multiply out the probabilities to some degree of precision
    int prob_prec = 1000;
    int corrupt_prob = (int) prob_prec * glb_corrupt_prob;

    // Pick a random number
    int random_num = rand() % prob_prec;

    // Determine whether to corrupt bytesnum_corrupt_bytes
    random_num = rand() % prob_prec;
    if (random_num < corrupt_prob) {
        // Corrupt a byte at the chosen random index
        int random_index = rand() % (MAX_FRAME_SIZE);
        recv_char_buffer[random_index] =
            ~recv_char_buffer[random_index];
    }

    Frame* corrupted_frame = convert_char_to_frame(recv_char_buffer); 
    Host* dst_host = &glb_hosts_array[dst_id];

    ll_append_node(&dst_host->incoming_frames_head,
                    (void*) corrupted_frame);

    free(incoming_frame);
    free(recv_char_buffer); 
}


/* 
* This function takes the frames from the outgoing_frames_head of each hosts and sends them to the appropriate 
* destination via the ports. At the ingress port we throttle the frames with the rate provided in the config file. 
* If the traffic(number of incoming frames) exceeds the bandwidth(the rate at which we accept), then the frames will be queued. 
* If the traffic still exceeds the capacity of the queue, then we drop the frames. 
*/
void process_and_send() {
    int* incoming_frames_accept = calloc(glb_num_hosts, sizeof(int)); 

    //service the queues at the ingress port
    for (int i = 0; i < glb_num_hosts; i++) { 
        Ingress* ingress_port = &glb_ingress_ports_array[i]; 
        while (ingress_port->buffer_queue_size > 0 && incoming_frames_accept[i] < glb_sysconfig.recv_accept_rate) {
            LLnode* ll_buffered_node = ll_pop_node(&ingress_port->buffer_queue_head);
            ingress_port->buffer_queue_size -= 1; 

            Frame* incoming_frame = ll_buffered_node->value;
            
            frame_sanity_check(incoming_frame);
            // ******** SECTION FOR TESTING ****************
            //Count data and ack frames accepted
            // if (incoming_frame->isAck) {
            //     host_in_ackframe_num[incoming_frame->dst_id] += 1; 
            //     host_out_dataframes_num_since_ack_received[incoming_frame->dst_id] = 0; 
            // } else {
            //     host_in_dataframe_num[incoming_frame->dst_id] += 1; 
            // }
            // ******** END OF TESTING SECTION ************
            send_frame(incoming_frame, glb_sysconfig.corrupt_prob);
            incoming_frames_accept[i] += 1; 

            free(ll_buffered_node);
        }
    }

    //service the hosts
    while (1) {
        int num_empty_output_queues = 0; 
        int host_order[glb_num_hosts]; 

        get_rand_seq(glb_num_hosts, host_order); 

        int i;
        for (i = 0; i < glb_num_hosts; i++) {
            int sender_id = host_order[i]; 
            Host* host = &glb_hosts_array[sender_id]; 

            if (ll_get_length(host->outgoing_frames_head) > 0) {
                LLnode* ll_outframe_node = ll_pop_node(&host->outgoing_frames_head);
                Frame* outgoing_frame = ll_outframe_node->value;

                frame_sanity_check(outgoing_frame); 
                // ******** SECTION FOR TESTING ****************
                // Count ack and data frames sent
                // if (outgoing_frame->isAck) {
                //     host_out_ackframe_num[outgoing_frame->src_id] += 1; 
                // } else {
                //     host_out_dataframe_num[outgoing_frame->src_id] += 1; 
                //     host_out_dataframes_num_since_ack_received[outgoing_frame->src_id] += 1; 
                // }
                // *********************************************
                int recv_id = outgoing_frame->dst_id; 

                Ingress* recv_ingress_port = &glb_ingress_ports_array[recv_id]; 
                
                if (incoming_frames_accept[recv_id] >= glb_sysconfig.recv_accept_rate) {
                    if (recv_ingress_port->buffer_queue_size < INGRESS_PORT_QUEUE_CAPACITY) {
                        
                        ll_append_node(&recv_ingress_port->buffer_queue_head, outgoing_frame); 
                        recv_ingress_port->buffer_queue_size += 1; 
                    }
                } else { 
                    // ******** SECTION FOR TESTING ****************
                    // Count data and ack frames accepted 
                    // if (outgoing_frame->isAck) {
                    //     host_in_ackframe_num[outgoing_frame->dst_id] += 1; 
                    //     host_out_dataframes_num_since_ack_received[outgoing_frame->dst_id] = 0; 
                    // } else {
                    //     host_in_dataframe_num[outgoing_frame->dst_id] += 1; 
                    // }
                    // *********************************************
                    
                    send_frame(outgoing_frame, glb_sysconfig.corrupt_prob);
                    incoming_frames_accept[recv_id] += 1; 
                }

                free(ll_outframe_node);

            } else {
                num_empty_output_queues += 1; 
            }
        }
        if (num_empty_output_queues == glb_num_hosts) {
            free(incoming_frames_accept); 
            return;
        }
    }
}

//*********************** TESTING SECTION **************************//
// void TESTING(Frame* outgoing_frame) {
//     if (glb_sysconfig.test_case_id == 1) { //Sending 1 packet and expecting receiver to print it out.
//         send_frame(outgoing_frame, glb_sysconfig.corrupt_prob); 
//     } else if (glb_sysconfig.test_case_id == 2) { //Corrupt single byte for the first dataframe.
//         int host_src_id = 0;
//         if (host_out_dataframe_num[host_src_id] == 1) {
//             send_frame(outgoing_frame, 1.0); 
//         } else {
//             send_frame(outgoing_frame, glb_sysconfig.corrupt_prob); 
//         }
//     } else if (glb_sysconfig.test_case_id == 3) { //Sending multiple packets and expecting receiver to print it out with no exact order. Only relative order matters(not being tested).
//         send_frame(outgoing_frame, glb_sysconfig.corrupt_prob); 
//     } else if (glb_sysconfig.test_case_id == 4) { //Drop ACK once.
//         int host_src_id = 0;
//         int host_dst_id = 1; 
//         if (host_out_dataframe_num[host_src_id] == 1 && host_out_ackframe_num[host_dst_id] == 1) {
//             printf("Garbage output to ensure ACK is implemented\n");
//         } else {
//             if (host_out_dataframe_num[host_src_id] == 3) {
//                 printf("Garbage output to fail the test because packet should not have "
//                     "been retransmitted so many times.\n");
//             }
//             send_frame(outgoing_frame, glb_sysconfig.corrupt_prob);
//         }
//     } else {
//         printf("Test case does not exist: %d\n", glb_sysconfig.test_case_id); 
//         exit(1); 
//     }
// }
//********************** END OF TESTING SECTION *********************//