#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "../include/config.h"
#include <errno.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>           
#include <sys/mman.h>        
#include <semaphore.h>       
#include <unistd.h>
#include <string.h>
#include "../include/local.h"


#define SHM_SIZE 1024
#define MSG_SIZE 1024
#define MAX_BUFFER_SIZE 1024
#define PREPARATION_TEAM_COUNT 6
#define BAKERS_TEAM_COUNT 3

// Message Queue related info
int mid;
MESSAGE msg_rcv;
MESSAGE requested_missing_items;



/*Configuration*/
Config config;
char config_file_name[30];


/*Shared memory ID and semaphore id for each basic items (milk yasset ....)*/


int shm_wheat_id;
char *shm_wheat_ptr;
int shm_yeast_id;
char *shm_yeast_ptr;
int shm_butter_id;
char *shm_butter_ptr;
int shm_milk_id;
char *shm_milk_ptr;
int shm_sugar_id;
char *shm_sugar_ptr;
int shm_salt_id;
char *shm_salt_ptr;
int shm_sweet_items_id;
char *shm_sweet_items_ptr;
int shm_cheese_id;
char *shm_cheese_ptr;
int shm_salami_id;
char *shm_salami_ptr;

int sem_wheat_id;
int sem_yeast_id;
int sem_butter_id;
int sem_milk_id;
int sem_sugar_id;
int sem_salt_id;
int sem_sweet_items_id;
int sem_cheese_id;
int sem_salami_id;

int number_of_frustrated_customers;
int nummber_of_complained_customers;
int number_of_missing_items_customers;


union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
}arg;

/*Create shared memory and semaphore for basic items */

void create_shm_sem_basic_items();
int create_shm(char proj_id, int size, char **ptr);
int create_sem(char proj_id);
char *basic_items_message=NULL;








/* close shm sem  */

void cleanup_shm(int shmid, char *ptr);
void cleanup_sem(int semid);
void cleanup_shm_sem_basic_items();


/*message basic items */
void create_basic_items_message();








/* Important for structure */
int preparation_teams[6];//in each cell have number of team members(ex: 2 paste preparation , 3 cake,... )
int bakers_teams[3];//in each cell have number of team members







/*Create teams members*/
void fork_chefs(pid_t chefs_pids[],pid_t paste_team_pids[],pid_t cake_team_pids[],pid_t sandwishes_team_pids[],pid_t sweets_team_pids[],pid_t sweet_patiss_team_pids[],pid_t savory_patiss_team_pids[]);
void fork_bakers(pid_t bakers_pids[],pid_t sweet_cake_bake_team_pids[],pid_t sweet_savory_patiss_bake_team_pids[],pid_t bread_bake_team_pids[]);
void fork_sallers(pid_t sallers_pids[], pid_t customers_pids[]);
void fork_suppliers(pid_t suppliers_pids[]);
// Maybe customers
void fork_customers(pid_t customers_pids[], pid_t sallers_pids[]);






/*Divide teams members ( Chefs and Bakers )*/
void divde_prepartion_team_members(int chef_number);
void divde_bakers_team_members(int baker_number);


/*Printing*/
void print_array(const pid_t array[], int size);
void printConfig(const Config *cfg);





/*Terniate game*/
void kill_teams(pid_t chefs_pids[], pid_t baker_pids[], pid_t sallers_pids[], pid_t suppliers_pids[],pid_t customers_pids[]);
void kill_process(pid_t pid);








int main(int argc, char **argv) {
        
  
	
	strcpy(config_file_name, argv[1]);
	
	if (argc < 2) {
	       fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
	        return EXIT_FAILURE;
	    }

	
	if (load_config(config_file_name, &config) == 0) {
	    //printConfig(&config);
	   // printf("Success to load configuration.\n");
	} else {
	    fprintf(stderr, "Failed to load configuration.\n");
	}

    key_t key;
    if ((key = ftok(".", SEED)) == -1) {    
    perror("Key generation");
    printf("Error number: %d\n", errno);
    return 1;
    }

    mid = msgget(key, IPC_CREAT | 0660);
	if (mid == -1){
        perror("msgget");
        exit(1);
    }
	

	pid_t chefs_pids[config.chefs_number];
	pid_t bakers_pids[config.bakers_number];
    pid_t sallers_pids[config.sallers_number];
        pid_t suppliers_pids[config.suppliers_number];
        pid_t customers_pids[config.customer_number];
        
        pid_t paste_team_pids[config.chefs_number];
	    pid_t cake_team_pids[config.chefs_number];
	    pid_t sandwishes_team_pids[config.chefs_number];
	    pid_t sweets_team_pids[config.chefs_number];
	    pid_t sweet_patiss_team_pids[config.chefs_number];
	    pid_t savory_patiss_team_pids[config.chefs_number];

	    pid_t sweet_cake_bake_team_pids[config.bakers_number];
	    pid_t sweet_savory_patiss_bake_team_pids[config.bakers_number];
	    pid_t bread_bake_team_pids[config.bakers_number];
        
        
        divde_prepartion_team_members(config.chefs_number);
        divde_bakers_team_members(config.bakers_number);
        
        
        create_shm_sem_basic_items();
        create_basic_items_message();
        //printf("From parent Combined IDs: %s\n", basic_items_message);
        fork_chefs(chefs_pids, paste_team_pids, cake_team_pids, sandwishes_team_pids, sweets_team_pids,sweet_patiss_team_pids,savory_patiss_team_pids);
        fork_customers(customers_pids,sallers_pids);
        fork_sallers(sallers_pids, customers_pids);

        while (1) {
            if (msgrcv(mid, &msg_rcv, sizeof(msg_rcv.buffer), 2, 0) == -1) { // -1 is the equivalent to requsting an item
            perror("msgrcv");
            continue;
            }
            printf("----------------------------------------------------------------------------------\n");
            printf("Customer%d is %s, bakery noted\n",msg_rcv.msg_fm, msg_rcv.buffer); // Print the received message
            if (strcmp(msg_rcv.buffer, "Frustrated") == 0) {
                number_of_frustrated_customers++;
            } else if (strcmp(msg_rcv.buffer, "Complained") == 0) {
                nummber_of_complained_customers++;
            } else if (strcmp(msg_rcv.buffer, "Requested Missing Items") == 0) {
                number_of_missing_items_customers++;
            }
            // Print the number of customers
            printf("Number of frustrated customers: %d\n", number_of_frustrated_customers);
            printf("Number of complained customers: %d\n", nummber_of_complained_customers);
            printf("Number of customers requested missing items: %d\n", number_of_missing_items_customers);
        }

        


        
	//fork_bakers( bakers_pids,sweet_cake_bake_team_pids,sweet_savory_patiss_bake_team_pids,bread_bake_team_pids);
	//fork_sallers(sallers_pids);
	//fork_suppliers(suppliers_pids);
       
       
       
       
       
        /*Create basic items shm _sem _and message to pass it */
       
         
        
        /*
    	// Create shared memory segment
    	if ((shmid = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    	}

    	// Attach shared memory segment
    	if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    	}

    	// Create semaphore set with 1 semaphore
    	if ((semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666)) < 0) {
        perror("semget");
        exit(1);
    	}

    	// Initialize semaphore to 1 (binary semaphore)
    	union semun arg;
    	arg.val = 1;
    	if (semctl(semid, 0, SETVAL, arg) < 0) {
        perror("semctl");
        exit(1);
    	}
	

	
        shmdt(shm);
    	shmctl(shmid, IPC_RMID, NULL);
    	semctl(semid, 0, IPC_RMID);
        */
       
        
        
         
        
        sleep(15);
        cleanup_shm_sem_basic_items();
        kill_teams(chefs_pids , bakers_pids,sallers_pids,suppliers_pids, customers_pids);
        
        
        return 0;
  }
  
  
  
  
  
  
  



   
  
   

void fork_chefs(pid_t chefs_pids[],pid_t paste_team_pids[],pid_t cake_team_pids[],pid_t sandwishes_team_pids[],pid_t sweets_team_pids[],pid_t sweet_patiss_team_pids[],pid_t savory_patiss_team_pids[]) {
   // Fork processes for chefs
   
   
   int counter=0;
   int j=0;
   for (int i = 0; i < config.chefs_number; i++) {
    chefs_pids[i] = fork();

    if (chefs_pids[i] == 0) {
        

        if (j == preparation_teams[0] && counter == 0) { j = 0; }
        if (j == preparation_teams[1] && counter == 1) { j = 0; }
        if (j == preparation_teams[2] && counter == 2) { j = 0; }
        if (j == preparation_teams[3] && counter == 3) { j = 0; }
        if (j == preparation_teams[4] && counter == 4) { j = 0; }
        if (j == preparation_teams[5] && counter == 5) { j = 0; }
        // ../bin/paste_pre
        if (counter == 0) {
        	execlp("bin/paste_pre", "bin/paste_pre", config_file_name,basic_items_message,NULL);
        	perror("execlp failed for chef");
        	exit(EXIT_FAILURE);
        }
        // ../bin/cake_pre
        if (counter == 1) {
        	execlp("bin/cake_pre", "bin/cake_pre", config_file_name,basic_items_message, NULL);
        	perror("execlp failed for chef");
        	exit(EXIT_FAILURE);
        }
        // ../bin/sandwiches_pre
        if (counter == 2) {
        	execlp("bin/sandwiches_pre", "bin/sandwiches_pre", config_file_name,basic_items_message, NULL);
        	perror("execlp failed for chef");
        	exit(EXIT_FAILURE);
        }
        // ../bin/sweets_pre
        if (counter == 3) {
        	execlp("bin/sweets_pre", "bin/sweets_pre", config_file_name,basic_items_message,NULL);
        	perror("execlp failed for chef");
        	exit(EXIT_FAILURE);
        }
        // ../bin/sweet_patiss_pre
        if (counter == 4) {
        	execlp("bin/sweet_patiss_pre", "bin/sweet_patiss_pre", config_file_name,basic_items_message, NULL);
        	perror("execlp failed for chef");
        	exit(EXIT_FAILURE);
        }
        // ../bin/savory_patiss_pre
        if (counter == 5) {
        	execlp("bin/savory_patiss_pre", "bin/savory_patiss_pre", config_file_name,basic_items_message, NULL);
        	perror("execlp failed for chef");
        	exit(EXIT_FAILURE);
        }
        
        

    } else if (chefs_pids[i] > 0) {
       

        if (counter == 0) {
            paste_team_pids[j] = chefs_pids[i];
        } else if (counter == 1) {
            cake_team_pids[j] = chefs_pids[i];
        } else if (counter == 2) {
            sandwishes_team_pids[j] = chefs_pids[i];
        } else if (counter == 3) {
            sweets_team_pids[j] = chefs_pids[i];
        } else if (counter == 4) {
            sweet_patiss_team_pids[j] = chefs_pids[i];
        } else if (counter == 5) {
            savory_patiss_team_pids[j] = chefs_pids[i];
        }

        j++;

        if (j == preparation_teams[counter]) {
      
            j = 0;
            counter++;
        }

    } else {
        // ----- FORK ERROR -----
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
}

        
        
}

void fork_bakers(pid_t bakers_pids[], pid_t sweet_cake_bake_team_pids[], pid_t sweet_savory_patiss_bake_team_pids[], pid_t bread_bake_team_pids[]) {
    int counter = 0;
    int j = 0;
    for (int i = 0; i < config.bakers_number; i++) {
        bakers_pids[i] = fork();

        if (bakers_pids[i] == 0) {
            // ----- CHILD PROCESS -----
            
            if (j == bakers_teams[0] && counter == 0) { j = 0; }
            if (j == bakers_teams[1] && counter == 1) { j = 0; }
            if (j == bakers_teams[2] && counter == 2) { j = 0; }

            if (counter == 0) {
                execlp("../bin/sweet_cake_bake", "../bin/sweet_cake_bake", config_file_name, NULL);
                perror("execlp failed for baker (sweet_cake_bake)");
                exit(EXIT_FAILURE);
            }
            if (counter == 1) {
                execlp("../bin/sweet_savory_patiss_bake", "../bin/sweet_savory_patiss_bake", config_file_name, NULL);
                perror("execlp failed for baker (sweet_savory_patiss_bake)");
                exit(EXIT_FAILURE);
            }
            if (counter == 2) {
                execlp("../bin/bread_bake", "../bin/bread_bake", config_file_name, NULL);
                perror("execlp failed for baker (bread_bake)");
                exit(EXIT_FAILURE);
            }

        } else if (bakers_pids[i] > 0) {
            // ----- PARENT PROCESS -----
            if (counter == 0) {
                sweet_cake_bake_team_pids[j] = bakers_pids[i];
            } else if (counter == 1) {
                sweet_savory_patiss_bake_team_pids[j] = bakers_pids[i];
            } else if (counter == 2) {
                bread_bake_team_pids[j] = bakers_pids[i];
            }

            j++;

            if (j == bakers_teams[counter]) {
                j = 0;
                counter++;
            }

        } else {
            // ----- FORK ERROR -----
            perror("fork failed for baker");
            exit(EXIT_FAILURE);
        }
    }
}


void fork_sallers(pid_t sallers_pids[], pid_t customers_pids[]) {
     // Fork processes for sallers
    for (int i = 0; i < config.sallers_number; i++) {
       
        if ((sallers_pids[i] = fork()) == 0) {
            // In the child process (saller)

            char seller_id_str[10];
            snprintf(seller_id_str, sizeof(seller_id_str), "%d", i);  // Convert PID to string
            // format mid to string
            char mid_str[10];
            snprintf(mid_str, sizeof(mid_str), "%d", mid);  // Convert PID to string
            execlp("bin/saller", "bin/saller", config_file_name, seller_id_str, mid_str, NULL);
            perror("execlp failed for saller");
            exit(EXIT_FAILURE);
        }
    }

}

void fork_suppliers(pid_t suppliers_pids[]) {
    // Fork processes for suppliers
    for (int i = 0; i < config.suppliers_number; i++) {
        if ((suppliers_pids[i] = fork()) == 0) {
            // In the child process (supplier)
            execlp("../bin/supplier", "../bin/supplier", config_file_name, NULL);
            perror("execlp failed for supplier");
            exit(EXIT_FAILURE);
        }
    }
}

void fork_customers(pid_t customers_pids[], pid_t sallers_pids[]){
    // Fork processes for customers
    for (int i = 0; i < config.customer_number; i++) {
        if ((customers_pids[i] = fork()) == 0) {
            char customer_pid_str[10];  // Buffer to hold PID as string
            snprintf(customer_pid_str, sizeof(customer_pid_str), "%d", i);  // Convert PID to string
            time_t creation_time = time(NULL);  // Get current timestamp
            char time_str[20];
            snprintf(time_str, sizeof(time_str), "%ld", creation_time);  // Convert to string
            // In the child process (customer)
            char mid_str[10];
            snprintf(mid_str, sizeof(mid_str), "%d", mid);  // Convert PID to string
            // ../bin/customer
            execlp("bin/customer", "bin/customer", config_file_name, customer_pid_str, time_str, mid_str, NULL);
            perror("execlp failed for customer");
            exit(EXIT_FAILURE);
        }
    }
}







// Function to kill a process given its PID
void kill_process(pid_t pid) {
    if (kill(pid, SIGKILL) == 0) {
        printf("Successfully killed process with PID %d\n", pid);
    } else {
        perror("Error killing process");
    }
}

// Function to clean up by killing all processes
void kill_teams(pid_t chefs_pids[] , pid_t baker_pids[], pid_t sallers_pids[], pid_t suppliers_pids[], pid_t customers_pids[]) {
    // Kill chefs processes
    for (int i = 0; i < config.chefs_number; i++) {
        kill_process(chefs_pids[i]);
    }

    // Kill bakers processes
    for (int i = 0; i < config.bakers_number; i++) {
        kill_process(baker_pids[i]);
    }

    // Kill sallers processes
    for (int i = 0; i < config.sallers_number; i++) {
        kill_process(sallers_pids[i]);
    }

    // Kill suppliers processes
    for (int i = 0; i < config.suppliers_number; i++) {
        kill_process(suppliers_pids[i]);
    }
    // Kill customers processes
    for (int i = 0; i < config.customer_number; i++) {
        kill_process(customers_pids[i]);
    }
}
void divde_prepartion_team_members(int chef_number){


    if (chef_number < PREPARATION_TEAM_COUNT) {
        printf("Not enough chefs! Need at least %d chefs.\n", PREPARATION_TEAM_COUNT);
        return;
    }

    // Step 1: Give each team 1 member
    for (int i = 0; i < PREPARATION_TEAM_COUNT; i++) {
        preparation_teams[i] = 1;
    }

    chef_number -= PREPARATION_TEAM_COUNT; // We already assigned 1 member to each

    // Step 2: Distribute remaining members
    int i = 0;
    while (chef_number > 0) {
        preparation_teams[i]++;
        chef_number--;
        i = (i + 1) % PREPARATION_TEAM_COUNT; // Cycle through teams
    }

    // Step 3: Print result
    printf("Team chefs distribution:\n");
    for (int i = 0; i < PREPARATION_TEAM_COUNT; i++) {
        printf("Team %d: %d members\n", i + 1, preparation_teams[i]);
    }
}

void divde_bakers_team_members(int baker_number){


    if (baker_number < BAKERS_TEAM_COUNT) {
        printf("Not enough chefs! Need at least %d chefs.\n", BAKERS_TEAM_COUNT);
        return;
    }

    // Step 1: Give each team 1 member
    for (int i = 0; i < BAKERS_TEAM_COUNT; i++) {
        bakers_teams[i] = 1;
    }

    baker_number -= BAKERS_TEAM_COUNT; // We already assigned 1 member to each

    // Step 2: Distribute remaining members
    int i = 0;
    while (baker_number > 0) {
        bakers_teams[i]++;
        baker_number--;
        i = (i + 1) % BAKERS_TEAM_COUNT; // Cycle through teams
    }

    // Step 3: Print result
    printf("Team bakers distribution:\n");
    for (int i = 0; i < BAKERS_TEAM_COUNT; i++) {
        printf("Team %d: %d members\n", i + 1, bakers_teams[i]);
    }
}


// Helper to create shared memory
int create_shm(char proj_id, int size, char **ptr) {
    key_t key = ftok(".", proj_id);
    if (key == -1) {
        perror("ftok failed");
        exit(1);
    }
    int shmid = shmget(key, size, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget failed");
        exit(1);
    }
    *ptr = (char *)shmat(shmid, NULL, 0);
    if (*ptr == (char *)-1) {
        perror("shmat failed");
        exit(1);
    }
    return shmid;
}

// Helper to create semaphore
int create_sem(char proj_id) {
    key_t key = ftok(".", proj_id);
    if (key == -1) {
        perror("ftok failed");
        exit(1);
    }
    int semid = semget(key, 3, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget failed");
        exit(1);
    }
    // Initialize all semaphores
	//#define MUTEX 0       // Controls access to read_count
	//#define READ_COUNT 1  // Tracks number of readers
	//#define WRITE_LOCK 2  // Ensures exclusive write access					
	unsigned short values[3] = {1, 0, 1}; // 1 for mutual exclusion ,1 for read counter , 1 for write lock 
	arg.array = values;
	if (semctl(semid, 0, SETALL, arg) == -1) {
    	perror("semctl SETALL failed");
    	exit(EXIT_FAILURE);
    	} 
    
    
    
    return semid;
}

// Main function
void create_shm_sem_basic_items() {
    shm_wheat_id = create_shm('A', 64, &shm_wheat_ptr);
    sem_wheat_id = create_sem('A');

    shm_yeast_id = create_shm('B', 64, &shm_yeast_ptr);
    sem_yeast_id = create_sem('B');

    shm_butter_id = create_shm('C', 64, &shm_butter_ptr);
    sem_butter_id = create_sem('C');

    shm_milk_id = create_shm('D', 64, &shm_milk_ptr);
    sem_milk_id = create_sem('D');

    shm_sugar_id = create_shm('E', 64, &shm_sugar_ptr);
    sem_sugar_id = create_sem('E');

    shm_salt_id = create_shm('F', 64, &shm_salt_ptr);
    sem_salt_id = create_sem('F');

    shm_sweet_items_id = create_shm('G', 64, &shm_sweet_items_ptr);
    sem_sweet_items_id = create_sem('G');

    shm_cheese_id = create_shm('H', 64, &shm_cheese_ptr);
    sem_cheese_id = create_sem('H');

    shm_salami_id = create_shm('I', 64, &shm_salami_ptr);
    sem_salami_id = create_sem('I');
}



// Helper to cleanup shared memory
void cleanup_shm(int shmid, char *ptr) {
    if (shmdt(ptr) == -1) {
        perror("shmdt failed");
    }
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl IPC_RMID failed");
    }
}

// Helper to cleanup semaphore
void cleanup_sem(int semid) {
    if (semctl(semid, 0, IPC_RMID) == -1) {
        perror("semctl IPC_RMID failed");
    }
}

// Main cleanup function
void cleanup_shm_sem_basic_items() {
    cleanup_shm(shm_wheat_id, shm_wheat_ptr);
    cleanup_sem(sem_wheat_id);

    cleanup_shm(shm_yeast_id, shm_yeast_ptr);
    cleanup_sem(sem_yeast_id);

    cleanup_shm(shm_butter_id, shm_butter_ptr);
    cleanup_sem(sem_butter_id);

    cleanup_shm(shm_milk_id, shm_milk_ptr);
    cleanup_sem(sem_milk_id);

    cleanup_shm(shm_sugar_id, shm_sugar_ptr);
    cleanup_sem(sem_sugar_id);

    cleanup_shm(shm_salt_id, shm_salt_ptr);
    cleanup_sem(sem_salt_id);

    cleanup_shm(shm_sweet_items_id, shm_sweet_items_ptr);
    cleanup_sem(sem_sweet_items_id);

    cleanup_shm(shm_cheese_id, shm_cheese_ptr);
    cleanup_sem(sem_cheese_id);

    cleanup_shm(shm_salami_id, shm_salami_ptr);
    cleanup_sem(sem_salami_id);
}











void printConfig(const Config *cfg) {
    printf("\n# --- Bakery Simulation Configuration ---\n\n");

    // Product Categories
    printf("bread_catagories_number = %d\n", cfg->bread_catagories_number);
    printf("sandwiches_number = %d\n", cfg->sandwiches_number);
    printf("cake_flavors_number = %d\n", cfg->cake_flavors_number);
    printf("sweets_flavors_number = %d\n", cfg->sweets_flavors_number);
    printf("sweet_patisseries_number = %d\n", cfg->sweet_patisseries_number);
    printf("savory_patisseries_number = %d\n", cfg->savory_patisseries_number);

    // Personnel
    printf("chefs_number = %d\n", cfg->chefs_number);
    printf("bakers_number = %d\n", cfg->bakers_number);
    printf("sallers_number = %d\n", cfg->sallers_number);
    printf("suppliers_number = %d\n", cfg->suppliers_number);

    // Ingredient Limits
    printf("wheat_amount_max = %d\n", cfg->wheat_amount_max);
    printf("yeast_amount_max = %d\n", cfg->yeast_amount_max);
    printf("butter_amount_max = %d\n", cfg->butter_amount_max);
    printf("milk_amount_max = %d\n", cfg->milk_amount_max);
    printf("sugar_amount_max = %d\n", cfg->sugar_amount_max);
    printf("salt_amount_max = %d\n", cfg->salt_amount_max);
    printf("sweet_items_amount_max = %d\n", cfg->sweet_items_amount_max);
    printf("cheese_amount_max = %d\n", cfg->cheese_amount_max);
    printf("salami_amount_max = %d\n", cfg->salami_amount_max);

    // Prices - Min
    printf("bread_catagories_price_min = %d\n", cfg->bread_catagories_price_min);
    printf("sandwiches_price_min = %d\n", cfg->sandwiches_price_min);
    printf("cake_flavors_price_min = %d\n", cfg->cake_flavors_price_min);
    printf("sweets_flavors_price_min = %d\n", cfg->sweets_flavors_price_min);
    printf("sweet_patisseries_price_min = %d\n", cfg->sweet_patisseries_price_min);
    printf("savory_patisseries_price_min = %d\n", cfg->savory_patisseries_price_min);

    // Prices - Max
    printf("bread_catagories_price_max = %d\n", cfg->bread_catagories_price_max);
    printf("sandwiches_price_max = %d\n", cfg->sandwiches_price_max);
    printf("cake_flavors_price_max = %d\n", cfg->cake_flavors_price_max);
    printf("sweets_flavors_price_max = %d\n", cfg->sweets_flavors_price_max);
    printf("sweet_patisseries_price_max = %d\n", cfg->sweet_patisseries_price_max);
    printf("savory_patisseries_price_max = %d\n", cfg->savory_patisseries_price_max);

    // Customer Feedback
    printf("frustrated_customers_number_threshold = %d\n", cfg->frustrated_customers_number_threshold);
    printf("complained_customers_number_threshold = %d\n", cfg->complained_customers_number_threshold);
    printf("requested_missing_items_customers_number_threshold = %d\n", cfg->requested_missing_items_customers_number_threshold);
    printf("min_number_of_customers = %d\n", cfg->min_number_of_customers);
    printf("max_number_of_customers = %d\n", cfg->max_number_of_customers);
    printf("min_time_wait_customer = %d\n", cfg->min_time_wait_customer);
    printf("max_time_wait_customer = %d\n", cfg->max_time_wait_customer);


    // Thresholds
    printf("daily_profit_threshold = %d\n", cfg->daily_profit_threshold);
    printf("max_time = %d\n", cfg->max_time);

    printf("\n# --- End of Configuration ---\n");
}

void print_array(const int array[], int size) {
    printf("[");
    for (int i = 0; i < size; i++) {
        printf("%d", array[i]);
        if (i < size - 1) {
            printf(", ");
        }
    }
    printf("]\n");
}

void create_basic_items_message() {
    basic_items_message = (char *)malloc(MAX_BUFFER_SIZE);
    if (basic_items_message == NULL) {
        perror("malloc failed");
        exit(1);
    }
    snprintf(basic_items_message, MAX_BUFFER_SIZE,
        "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
        shm_wheat_id, sem_wheat_id,
        shm_yeast_id, sem_yeast_id,
        shm_butter_id, sem_butter_id,
        shm_milk_id, sem_milk_id,
        shm_sugar_id, sem_sugar_id,
        shm_salt_id, sem_salt_id,
        shm_sweet_items_id, sem_sweet_items_id,
        shm_cheese_id, sem_cheese_id,
        shm_salami_id, sem_salami_id
    );
}



 /*
 
 	printf("\n#################################\n");
        print_array(chefs_pids, config.chefs_number) ;
        print_array(paste_team_pids, config.chefs_number) ;
        print_array(cake_team_pids, config.chefs_number) ;
        print_array(sandwishes_team_pids, config.chefs_number) ;
        print_array(sweets_team_pids, config.chefs_number) ;
        print_array(sweet_patiss_team_pids, config.chefs_number) ;
        print_array(savory_patiss_team_pids, config.chefs_number) ;
	printf("\n#################################\n");

*/
