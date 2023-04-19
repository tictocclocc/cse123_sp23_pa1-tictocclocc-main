#include "common.h"
#include "input.h"
#include "util.h"
#include "run_main.h"

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

/********************************
* DO NOT CHANGE THE MAIN FUNCTION
*********************************/
int main(int argc, char* argv[]) {

    glb_exit_main = 0; 

    srand(time(NULL)); 

    // Prepare the glb_sysconfig object
    glb_sysconfig.window_size = 0; 
    glb_sysconfig.corrupt_prob = 0.00;
    glb_sysconfig.recv_accept_rate = 0; 
    glb_sysconfig.test_case_id = 0; 
    strcpy(glb_sysconfig.config_path, "./test_suite/sw4_config"); 
    // Prepare other variables and seed the pseudo random number generator
    glb_num_hosts = -1; 

    // Parse out command line arguments 
    parse_args(argc, argv);

    // Read from the config file
    run_config();  
    
    // Initialize hosts, ports, and IO reader
    init(); 

    //Initialize testing fields
    init_test_setup(); 

    // Run the program until you get the exit signal
    while (1) {
        if (!glb_exit_main) {
            handle_input(); 
        }
        run_hosts(); 

        process_and_send(); 

        graceful_exit(); 

    }

    return 0;
}