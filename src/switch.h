#ifndef __SWITCH_H__
#define __SWITCH_H__

#include "common.h"
#include "util.h"

#include <math.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

void init_ingress_ports(); 
void init_egress_ports(); 
void init_test_setup(); 
void get_rand_seq(int, int*); 
void send_frame(Frame*, double);
void process_and_send(); 


#endif