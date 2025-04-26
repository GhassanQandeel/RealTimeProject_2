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

#define SHM_SIZE 1024
#define MSG_SIZE 1024


#define PREPARATION_TEAM_COUNT 6
#define BAKERS_TEAM_COUNT 3



union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};




Config config;
char config_file_name[30];

int shmid, semid;

int preparation_teams[6];//in each cell have number of team members(ex: 2 paste preparation , 3 cake,... )
int bakers_teams[3];//in each cell have number of team members




void print_array(const pid_t array[], int size);
void printConfig(const Config *cfg);
void kill_process(pid_t pid);


void fork_chefs(pid_t chefs_pids[],pid_t paste_team_pids[],pid_t cake_team_pids[],pid_t sandwishes_team_pids[],pid_t sweets_team_pids[],pid_t sweet_patiss_team_pids[],pid_t savory_patiss_team_pids[]);

void fork_bakers(pid_t bakers_pids[],pid_t sweet_cake_bake_team_pids[],pid_t sweet_savory_patiss_bake_team_pids[],pid_t bread_bake_team_pids[]);

void fork_sallers(pid_t sallers_pids[]);
void fork_suppliers(pid_t suppliers_pids[]);


void kill_teams(pid_t chefs_pids[], pid_t baker_pids[], pid_t sallers_pids[], pid_t suppliers_pids[]);



void divde_prepartion_team_members(int chef_number);
void divde_bakers_team_members(int baker_number);











int main(int argc, char **argv) {
        char *shm;
  
	
	strcpy(config_file_name, argv[1]);
	
	if (argc < 2) {
	       fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
	        return EXIT_FAILURE;
	    }

	
	if (load_config(config_file_name, &config) == 0) {
	    printConfig(&config);
	    
	    
	} else {
	    fprintf(stderr, "Failed to load configuration.\n");
	}
	
	

        

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
	

	*/
	
	
	
	
	pid_t chefs_pids[config.chefs_number];
	pid_t bakers_pids[config.bakers_number];
        pid_t sallers_pids[config.sallers_number];
        pid_t suppliers_pids[config.suppliers_number];
        
        pid_t paste_team_pids[config.chefs_number];
	pid_t cake_team_pids[config.chefs_number];
	pid_t sandwishes_team_pids[config.chefs_number];
	pid_t sweets_team_pids[config.chefs_number];
	pid_t sweet_patiss_team_pids[config.chefs_number];
	pid_t savory_patiss_team_pids[config.chefs_number];

	pid_t sweet_cake_bake_team_pids[config.bakers_number];
	pid_t sweet_savory_patiss_bake_team_pids[config.bakers_number];
	pid_t bread_bake_team_pids[config.bakers_number];
        
        
        
	
        
        /*
        for (int i = 0; i < config.chefs_number; i++) {
        	wait(NULL);
    	}
    	printf("\nParent reading shared memory:\n");
        printf("Message in parent : %s\n",shm);
        
	
	
        

        
        shmdt(shm);
    	shmctl(shmid, IPC_RMID, NULL);
    	semctl(semid, 0, IPC_RMID);
        */
       
        
        
        
        divde_prepartion_team_members(config.chefs_number);
        divde_bakers_team_members(config.bakers_number);
        
       
        fork_chefs(chefs_pids, paste_team_pids, cake_team_pids, sandwishes_team_pids, sweets_team_pids,sweet_patiss_team_pids,savory_patiss_team_pids);
	fork_bakers( bakers_pids,sweet_cake_bake_team_pids,sweet_savory_patiss_bake_team_pids,bread_bake_team_pids);
	fork_sallers(sallers_pids);
	fork_suppliers(suppliers_pids);
       
        
         
         
        sleep(5);
         
       
        kill_teams(chefs_pids , bakers_pids,sallers_pids,suppliers_pids);
        
        
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

        if (counter == 0) {
        	execlp("../bin/paste_pre", "../bin/paste_pre", config_file_name, NULL);
        	perror("execlp failed for chef");
        	exit(EXIT_FAILURE);
        }
        if (counter == 1) {
        	execlp("../bin/cake_pre", "../bin/cake_pre", config_file_name, NULL);
        	perror("execlp failed for chef");
        	exit(EXIT_FAILURE);
        }
        if (counter == 2) {
        	execlp("../bin/sandwiches_pre", "../bin/sandwiches_pre", config_file_name, NULL);
        	perror("execlp failed for chef");
        	exit(EXIT_FAILURE);
        }
        if (counter == 3) {
        	execlp("../bin/sweets_pre", "../bin/sweets_pre", config_file_name, NULL);
        	perror("execlp failed for chef");
        	exit(EXIT_FAILURE);
        }
        if (counter == 4) {
        	execlp("../bin/sweet_patiss_pre", "../bin/sweet_patiss_pre", config_file_name, NULL);
        	perror("execlp failed for chef");
        	exit(EXIT_FAILURE);
        }
        if (counter == 5) {
        	execlp("../bin/savory_patiss_pre", "../bin/savory_patiss_pre", config_file_name, NULL);
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


void fork_sallers(pid_t sallers_pids[]) {
     // Fork processes for sallers
    for (int i = 0; i < config.sallers_number; i++) {
        if ((sallers_pids[i] = fork()) == 0) {
            // In the child process (saller)
            execlp("../bin/saller", "../bin/saller", config_file_name, NULL);
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







// Function to kill a process given its PID
void kill_process(pid_t pid) {
    if (kill(pid, SIGKILL) == 0) {
        printf("Successfully killed process with PID %d\n", pid);
    } else {
        perror("Error killing process");
    }
}

// Function to clean up by killing all processes
void kill_teams(pid_t chefs_pids[] , pid_t baker_pids[], pid_t sallers_pids[], pid_t suppliers_pids[]) {
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
    printf("frustrated_customers_number = %d\n", cfg->frustrated_customers_number);
    printf("complained_customers_number = %d\n", cfg->complained_customers_number);
    printf("requested_missing_items_customers_number = %d\n", cfg->requested_missing_items_customers_number);

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


















