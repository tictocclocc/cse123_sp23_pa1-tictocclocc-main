#ifndef __HOST_H__
#define __HOST_H__

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

void init_host(Host*, int);
void run_hosts();

#endif
