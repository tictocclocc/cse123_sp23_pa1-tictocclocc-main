#include "util.h"

// Linked list functions
int ll_get_length(LLnode* head) {
    LLnode* tmp;
    int count = 1;
    if (head == NULL)
        return 0;
    else {
        tmp = head->next;
        while (tmp != head) {
            count++;
            tmp = tmp->next;
        }
        return count;
    }
}

void ll_append_node(LLnode** head_ptr, void* value) {
    LLnode* prev_last_node;
    LLnode* new_node;
    LLnode* head;

    if (head_ptr == NULL) {
        return;
    }

    // Init the value pntr
    head = (*head_ptr);
    new_node = (LLnode*) malloc(sizeof(LLnode));
    new_node->value = value;

    // The list is empty, no node is currently present
    if (head == NULL) {
        (*head_ptr) = new_node;
        new_node->prev = new_node;
        new_node->next = new_node;
    } else {
        // Node exists by itself
        prev_last_node = head->prev;
        head->prev = new_node;
        prev_last_node->next = new_node;
        new_node->next = head;
        new_node->prev = prev_last_node;
    }
}

LLnode* ll_pop_node(LLnode** head_ptr) {
    LLnode* last_node;
    LLnode* new_head;
    LLnode* prev_head;

    prev_head = (*head_ptr);
    if (prev_head == NULL) {
        return NULL;
    }
    last_node = prev_head->prev;
    new_head = prev_head->next;

    // We are about to set the head ptr to nothing because there is only one
    // thing in list
    if (last_node == prev_head) {
        (*head_ptr) = NULL;
        prev_head->next = NULL;
        prev_head->prev = NULL;
        return prev_head;
    } else {
        (*head_ptr) = new_head;
        last_node->next = new_head;
        new_head->prev = last_node;

        prev_head->next = NULL;
        prev_head->prev = NULL;
        return prev_head;
    }
}

void ll_destroy_node(LLnode* node) {
    if (node->type == llt_string) {
        free((char*) node->value);
    }
    free(node);
}

// Compute the difference in usec for two timeval objects
long timeval_usecdiff(struct timeval* start_time, struct timeval* finish_time) {
    long usec;
    usec = (finish_time->tv_sec - start_time->tv_sec) * 1000000;
    usec += (finish_time->tv_usec - start_time->tv_usec);
    return usec;
}


// compute new time after adding some number of microseconds to it
// number of microseconds assumed to be < 1 million
void timeval_usecplus(struct timeval* curr_timeval, long usec) {
    long new_us = curr_timeval->tv_usec + usec;
    long new_s = curr_timeval->tv_sec;
    if (new_us >= 1000000) { // if the number of microseconds is >= a million, carry-over
        new_us = (new_us % 1000000);
        new_s += 1;
    }
    curr_timeval->tv_usec = new_us;
    curr_timeval->tv_sec = new_s;
}


// Print out messages entered by the user
void print_cmd(Cmd* cmd) {
    fprintf(stderr, "src=%d, dst=%d, message=%s\n", cmd->src_id, cmd->dst_id,
            cmd->message);
}

// Print out frame contents
void print_frame(Frame* frame) {
    fprintf(stderr, "Frame isAck %d\n", frame->isAck);
    fprintf(stderr, "Frame dst_id: %d\n", frame->dst_id);
    fprintf(stderr, "Frame src_id: %d\n", frame->src_id);
    fprintf(stderr, "Frame data: %s\n", frame->data);
}

char* convert_frame_to_char(Frame* frame) {
    char* char_buffer = malloc(sizeof(Frame));
    memcpy(char_buffer, frame, sizeof(Frame));
    return char_buffer;
}

Frame* convert_char_to_frame(char* char_buf) {
    Frame* frame = malloc(sizeof(Frame));
    memcpy(frame, char_buf, sizeof(Frame));
    return frame;
}

// helper function for wraparound case 
int seq_bit_diff(uint8_t x1, uint8_t x2) { 
    int diff = x2 - x1;
    if (diff > 127) {
        diff -= (MAX_SEQ_BIT + 1);
    }
    else if (diff < -127) {
        diff += (MAX_SEQ_BIT + 1);
    }
    return diff;
}

//sanity check the frame
void frame_sanity_check(Frame* frame) {
    if (frame->dst_id >= glb_num_hosts) {
        printf("Invalid frame: Wrong dest_id: %d\n", frame->dst_id); 
        exit(-1); 
    } else if (frame->src_id >= glb_num_hosts) {
        printf("Invalid frame: Wrong src_id: %d\n", frame->src_id); 
        exit(-1); 
    } else if (frame->src_id == frame->dst_id) {
        printf("Cannot send message from/to the same host: %d.\n", frame->src_id); 
        exit(-1); 
    }
}

// function to compute crc
uint8_t compute_crc8(char* char_buf){
    uint8_t generator=0x07;
    uint8_t remainder=char_buf[0];
    for(int i=1;i<64;i++){
        char byte=char_buf[i];
        for(int j=7;j>=0;j--){
            if(((0x80&remainder))){
                remainder=(remainder<<1);
                remainder|=((byte>>j)&1);
                remainder^=generator;
            }else{
                remainder=(remainder<<1);
                remainder|=((byte>>j)&1);
            }        
        }
    }
    return remainder;
}

int min(int a, int b) {
    if (a <= b) {
        return a;
    }
    else {
        return b; 
    }
}

int max(int a, int b) {
    if (a >= b) {
        return a;
    }
    else {
        return b; 
    }
}