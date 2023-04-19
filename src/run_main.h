#ifndef __RUN_MAIN_H__
#define __RUN_MAIN_H__

#include "common.h"
#include "util.h"
#include "host.h"
#include "input.h"
#include "switch.h"

#include <assert.h>
#include <math.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libconfig.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

void run_config(); 
void parse_args(int, char**); 
void init(); 
void handle_input(); 
void graceful_exit(); 

#endif