#ifndef __SENDER_H__
#define __SENDER_H__

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

void handle_input_cmds(Host*); 
void handle_timedout_frames(Host*); 
void handle_incoming_acks(Host*);
void handle_outgoing_frames(Host*); 
#endif
