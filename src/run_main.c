#include "run_main.h"



// Parse the command line arguments
void parse_args(int argc, char* argv[]) {
    int error = 0; 
    int i = 0; 

    for (i = 1; i < argc - 1;) {
        if (strcmp(argv[i], "-t") == 0) {
            sscanf(argv[i + 1], "%d", &glb_sysconfig.test_case_id);
            i += 2;
        } else if (strcmp(argv[i], "-p") == 0) {
            sscanf(argv[i + 1], "%s", glb_sysconfig.config_path);
            i += 2;
        } else {
            error = 1;
            break;
        } 
    }

    if (glb_sysconfig.test_case_id < 0) {
        fprintf(stderr,"Test case id must be greater than 0\n"); 
        error = 1; 
    } 
    if (access(glb_sysconfig.config_path, F_OK) != 0) {
        fprintf(stderr,"Config file does not exist: %s\n", glb_sysconfig.config_path); 
        error = 1; 
    }
    
    if (error) {
        exit(1); 
    }
}


void run_config() {
    config_t cfg;
    config_setting_t *setting;

    config_init(&cfg);

    if(!config_read_file(&cfg, glb_sysconfig.config_path)) {
        fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg),
                config_error_line(&cfg), config_error_text(&cfg));
        config_destroy(&cfg);
        exit(1);
    }

    setting = config_lookup(&cfg, "hosts");
    if (setting != NULL) {
        config_setting_lookup_int(setting, "quantity", &glb_num_hosts);
    }

    config_lookup_float(&cfg, "corruption_probability", &glb_sysconfig.corrupt_prob); 
    config_lookup_int(&cfg, "sliding_window_size", &glb_sysconfig.window_size); 
    config_lookup_int(&cfg, "receiver_packet_acceptance_rate", &glb_sysconfig.recv_accept_rate); 

    config_destroy(&cfg);

    int error = 0; 
    if (glb_sysconfig.recv_accept_rate <= 0) {
        fprintf(stderr,"The packet acceptance rate of the receiver was not set correctly.\n"
                "USAGE: The packet acceptance rate of the receiver must be > 0\n"); 
                
        error = 1; 
    } 
    if (glb_num_hosts < 2) {
        fprintf(stderr,"Host configuration was not set properly.\n"
                "USAGE: # hosts >= 2\n"); 
        error = 1; 
    }
    if (glb_sysconfig.corrupt_prob < 0 || glb_sysconfig.corrupt_prob > 1) {
        fprintf(stderr,"Corruption probability configuration was not set correctly.\n"
                "USAGE: Corruption prob >= 0.0 and <= 1.0\n"); 
        error = 1; 
    }
    if (glb_sysconfig.window_size <= 0) {
        fprintf(stderr,"Window size configuration was not set correctly.\n"
                "USAGE: Window size > 0\n"); 
        error = 1; 
    }

    if (error) {
        exit(1); 
    }
}

void init() {
    int i; 

    glb_io_reader = malloc(sizeof(Reader)); 
    glb_hosts_array = malloc(glb_num_hosts * sizeof(Host));
    glb_egress_ports_array = malloc(glb_num_hosts * sizeof(Egress));
    glb_ingress_ports_array = malloc(glb_num_hosts * sizeof(Ingress));
    assert(glb_io_reader); 
    assert(glb_hosts_array);
    assert(glb_egress_ports_array);
    assert(glb_ingress_ports_array);

    // Init the IO reader
    init_reader(); 

    // Init the global arrays

    init_ingress_ports(); 
    init_egress_ports();

    fprintf(stderr, "Host window size=%d\n", glb_sysconfig.window_size); 
    fprintf(stderr, "Messages will be corrupted with probability=%f\n",
            glb_sysconfig.corrupt_prob);
    fprintf(stderr, "Available host id(s):\n");
    // Init sender objects, assign ids
    for (i = 0; i < glb_num_hosts; i++) {
        init_host(&glb_hosts_array[i], i);
        fprintf(stderr, "   host_id=%d\n", i);
    }
}

// Check input for new commands
void handle_input() {
    int input_res = check_input();
    if (input_res < 0) {
        fprintf(stderr, "Input read error\n"); 
        exit(1); 
    }
    if (input_res > 0) {
        glb_exit_main = 1; 
    }
}

// Exit the program only if the user inputted exit and 
// all the senders are not active
void graceful_exit() {
    if (glb_exit_main) {
        int i; 
        for (i = 0; i < glb_num_hosts; i++) {
            if ((&glb_hosts_array[i])->active) {
                return; 
            }
        }
        fprintf(stderr, "The program has been successfully closed.\n"); 
        free(glb_hosts_array);
        free(glb_io_reader); 
        exit(0); 
    }
}