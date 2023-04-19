#ifndef __RECEIVER_H__
#define __RECEIVER_H__

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


void handle_incoming_frames(Host*); 

#endif
