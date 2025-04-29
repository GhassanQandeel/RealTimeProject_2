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


#define MUTEX 0       // Controls access to read_count
#define READ_COUNT 1  // Tracks number of readers
#define WRITE_LOCK 2  // Ensures exclusive write access




#define PREPARATION_TEAM_COUNT 6
#define BAKERS_TEAM_COUNT 3



/*basic Items message buffer */
#define MAX_BUFFER_SIZE 1024



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

/*Chef production shm and sem */
int shm_paste_id;
char *shm_paste_ptr;

int shm_cake_paste_id;
char *shm_cake_paste_ptr;

int shm_sandwiches_id;
char *shm_sandwiches_ptr;

int shm_sweets_paste_id;
char *shm_sweets_paste_ptr;

int shm_sweet_patiss_paste_id;
char *shm_sweet_patiss_paste_ptr;

int shm_savory_patiss_paste_id;
char *shm_savory_patiss_paste_ptr;


int sem_paste_id;
int sem_cake_paste_id;
int sem_sandwiches_id;
int sem_sweets_paste_id;
int sem_sweet_patiss_paste_id;
int sem_savory_patiss_paste_id;

char *chef_production_message[6];
/**/
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
void write_shared_int(int sem_id, char *shm_ptr, int new_value);

void initialize_shm_and_sem(int *shm_ids,int *sem_ids,char **shm_ptrs, int count, char base_key);
void initialize_all_shm_and_sem_for_sale(
	int *bread_catagories_shm_id,int *bread_catagories_sem_id,char **bread_catagories_shm_ptr,
	int *sandwiches_shm_id,int *sandwiches_sem_id,char **sandwiches_shm_ptr,
	int *cake_flavors_shm_id,int *cake_flavors_sem_id,char **cake_flavors_shm_ptr,
	int *sweets_flavors_shm_id,int *sweets_flavors_sem_id,char **sweets_flavors_shm_ptr,
	int *sweet_patisseries_shm_id,int *sweet_patisseries_sem_id,char **sweet_patisseries_shm_ptr,
	int *savory_patisseries_shm_id,int *savory_patisseries_sem_id,char **savory_patisseries_shm_ptr);
	
	
void create_all_shm_and_sem_for_sale_message(
	int *bread_catagories_shm_id,int *bread_catagories_sem_id,
	int *sandwiches_shm_id,int *sandwiches_sem_id,
	int *cake_flavors_shm_id,int *cake_flavors_sem_id,
	int *sweets_flavors_shm_id,int *sweets_flavors_sem_id,
	int *sweet_patisseries_shm_id,int *sweet_patisseries_sem_id,
	int *savory_patisseries_shm_id,int *savory_patisseries_sem_id);	

void cleanup_shm_sem_for_sale(int *bread_catagories_shm_id,int *bread_catagories_sem_id,char **bread_catagories_shm_ptr,
	int *sandwiches_shm_id,int *sandwiches_sem_id,char **sandwiches_shm_ptr,
	int *cake_flavors_shm_id,int *cake_flavors_sem_id,char **cake_flavors_shm_ptr,
	int *sweets_flavors_shm_id,int *sweets_flavors_sem_id,char **sweets_flavors_shm_ptr,
	int *sweet_patisseries_shm_id,int *sweet_patisseries_sem_id,char **sweet_patisseries_shm_ptr,
	int *savory_patisseries_shm_id,int *savory_patisseries_sem_id,char **savory_patisseries_shm_ptr);

/* close shm sem  */

void cleanup_shm(int shmid, char *ptr);
void cleanup_sem(int semid);
void cleanup_shm_sem_basic_items();
void cleanup_shm_sem_bakery(int shm_ids[], int sem_ids[], char *shm_ptrs[], int count);

/*message basic items */
void create_basic_items_message();
void create_production_items_message();
char* create_shm_sem_message(int* shm_ids, int* sem_ids, int count);
char *bread_catagories_shm_sem_message;
char *sandwiches_shm_sem_message;
char *cake_flavors_shm_sem_message;
char *sweets_flavors_shm_sem_message;
char *sweet_patisseries_shm_sem_message;
char *savory_patisseries_shm_sem_message;






/* Important for structure */
int preparation_teams[6];//in each cell have number of team members(ex: 2 paste preparation , 3 cake,... )
int bakers_teams[3];//in each cell have number of team members







/*Create teams members*/
void fork_chefs(pid_t chefs_pids[],pid_t paste_team_pids[],pid_t cake_team_pids[],pid_t sandwishes_team_pids[],pid_t sweets_team_pids[],pid_t sweet_patiss_team_pids[],pid_t savory_patiss_team_pids[]);
void fork_bakers(pid_t bakers_pids[],pid_t sweet_cake_bake_team_pids[],pid_t sweet_savory_patiss_bake_team_pids[],pid_t bread_bake_team_pids[]);
void fork_sallers(pid_t sallers_pids[]);
void fork_suppliers(pid_t suppliers_pids[]);






/*Divide teams members ( Chefs and Bakers )*/
void divde_prepartion_team_members(int chef_number);
void divde_bakers_team_members(int baker_number);


/*Printing*/
void print_array(const pid_t array[], int size);
void printConfig(const Config *cfg);





/*Terniate game*/
void kill_teams(pid_t chefs_pids[], pid_t baker_pids[], pid_t sallers_pids[], pid_t suppliers_pids[]);
void kill_process(pid_t pid);








int main(int argc, char **argv) {
        
  
	
	strcpy(config_file_name, argv[1]);
	
	if (argc < 2) {
	       fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
	        return EXIT_FAILURE;
	    }

	
	if (load_config(config_file_name, &config) == 0) {
	    //printConfig(&config);
	    printf("Success to load configuration.\n");
	} else {
	    fprintf(stderr, "Failed to load configuration.\n");
	}
	
	

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
	
	// For Bread Categories
	int bread_catagories_shm_id[config.bread_catagories_number];
	int bread_catagories_sem_id[config.bread_catagories_number];
	char *bread_catagories_shm_ptr[config.bread_catagories_number];

	// For Sandwiches
	int sandwiches_shm_id[config.sandwiches_number];
	int sandwiches_sem_id[config.sandwiches_number];
	char *sandwiches_shm_ptr[config.sandwiches_number];

	// For Cake Flavors
	int cake_flavors_shm_id[config.cake_flavors_number];
	int cake_flavors_sem_id[config.cake_flavors_number];
	char *cake_flavors_shm_ptr[config.cake_flavors_number];

	// For Sweets Flavors
	int sweets_flavors_shm_id[config.sweets_flavors_number];
	int sweets_flavors_sem_id[config.sweets_flavors_number];
	char *sweets_flavors_shm_ptr[config.sweets_flavors_number];

	// For Sweet Patisseries
	int sweet_patisseries_shm_id[config.sweet_patisseries_number];
	int sweet_patisseries_sem_id[config.sweet_patisseries_number];
	char *sweet_patisseries_shm_ptr[config.sweet_patisseries_number];

	// For Savory Patisseries
	int savory_patisseries_shm_id[config.savory_patisseries_number];
	int savory_patisseries_sem_id[config.savory_patisseries_number];
	char *savory_patisseries_shm_ptr[config.savory_patisseries_number];
	
	
	
	
   
        divde_prepartion_team_members(config.chefs_number);
        divde_bakers_team_members(config.bakers_number);
        
        initialize_all_shm_and_sem_for_sale(
	bread_catagories_shm_id,bread_catagories_sem_id,bread_catagories_shm_ptr,
	sandwiches_shm_id,sandwiches_sem_id,sandwiches_shm_ptr,
	cake_flavors_shm_id,cake_flavors_sem_id,cake_flavors_shm_ptr,
	sweets_flavors_shm_id,sweets_flavors_sem_id,sweets_flavors_shm_ptr,
	sweet_patisseries_shm_id,sweet_patisseries_sem_id,sweet_patisseries_shm_ptr,
	savory_patisseries_shm_id,savory_patisseries_sem_id,savory_patisseries_shm_ptr);
        create_shm_sem_basic_items();
        
        
        
    	
    	
		
	create_all_shm_and_sem_for_sale_message(
	bread_catagories_shm_id,bread_catagories_sem_id,
	sandwiches_shm_id,sandwiches_sem_id,
	cake_flavors_shm_id,cake_flavors_sem_id,
	sweets_flavors_shm_id,sweets_flavors_sem_id,
	sweet_patisseries_shm_id,sweet_patisseries_sem_id,
	savory_patisseries_shm_id,savory_patisseries_sem_id);	
	

        create_basic_items_message();
        create_production_items_message();
        
        
        //write_shared_int(sem_wheat_id, shm_wheat_ptr, 15);
        fork_chefs(chefs_pids, paste_team_pids, cake_team_pids, sandwishes_team_pids, sweets_team_pids,sweet_patiss_team_pids,savory_patiss_team_pids);
	//fork_bakers( bakers_pids,sweet_cake_bake_team_pids,sweet_savory_patiss_bake_team_pids,bread_bake_team_pids);
	//fork_sallers(sallers_pids);
	//fork_suppliers(suppliers_pids);
       
       
       
       
        
        /*Create basic items shm _sem _and message to pass it */
       
         
       
        printf("bread :%s\n\n",bread_catagories_shm_sem_message);
       
        printf("sandwiches :%s\n\n",sandwiches_shm_sem_message);
        /*
        printf("cake :%s\n\n",cake_flavors_shm_sem_message);
        printf("sweets :%s\n\n",sweets_flavors_shm_sem_message);
        printf("sweet patiss :%s\n\n",sweet_patisseries_shm_sem_message);
        printf("savory patiss :%s\n\n",savory_patisseries_shm_sem_message);
        */

         
   	
        sleep(10);
        
        
        cleanup_shm_sem_basic_items();
        cleanup_shm_sem_for_sale(bread_catagories_shm_id,bread_catagories_sem_id,bread_catagories_shm_ptr,
	sandwiches_shm_id,sandwiches_sem_id,sandwiches_shm_ptr,
	cake_flavors_shm_id,cake_flavors_sem_id,cake_flavors_shm_ptr,
	sweets_flavors_shm_id,sweets_flavors_sem_id,sweets_flavors_shm_ptr,
	sweet_patisseries_shm_id,sweet_patisseries_sem_id,sweet_patisseries_shm_ptr,
	savory_patisseries_shm_id,savory_patisseries_sem_id,savory_patisseries_shm_ptr);
	
		              
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
        	execlp("../bin/paste_pre", "../bin/paste_pre", config_file_name,basic_items_message,chef_production_message[0],NULL);
        	perror("execlp failed for chef");
        	exit(EXIT_FAILURE);
        }
        if (counter == 1) {
        	execlp("../bin/cake_pre", "../bin/cake_pre", config_file_name,basic_items_message,chef_production_message[1], NULL);
        	perror("execlp failed for chef");
        	exit(EXIT_FAILURE);
        }
        if (counter == 2) {
        	execlp("../bin/sandwiches_pre",
        	 "../bin/sandwiches_pre",
        	  config_file_name,
        	  basic_items_message,
        	  bread_catagories_shm_sem_message,
        	  sandwiches_shm_sem_message, NULL);
        	perror("execlp failed for chef");
        	exit(EXIT_FAILURE);
        }
        if (counter == 3) {
        	execlp("../bin/sweets_pre", "../bin/sweets_pre", config_file_name,basic_items_message,chef_production_message[3],NULL);
        	perror("execlp failed for chef");
        	exit(EXIT_FAILURE);
        }
        if (counter == 4) {
        	execlp("../bin/sweet_patiss_pre", "../bin/sweet_patiss_pre", config_file_name,basic_items_message,chef_production_message[4], NULL);
        	perror("execlp failed for chef");
        	exit(EXIT_FAILURE);
        }
        if (counter == 5) {
        	execlp("../bin/savory_patiss_pre", "../bin/savory_patiss_pre", config_file_name,basic_items_message,chef_production_message[5], NULL);
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
                execlp("../bin/sweet_cake_bake",
                 "../bin/sweet_cake_bake",
                  config_file_name,
                  chef_production_message[1],
                  chef_production_message[3],
                  cake_flavors_shm_sem_message,
                  sweets_flavors_shm_sem_message, NULL);
                perror("execlp failed for baker (sweet_cake_bake)");
                exit(EXIT_FAILURE);
            }
            if (counter == 1) {
                execlp("../bin/sweet_savory_patiss_bake",
                 "../bin/sweet_savory_patiss_bake",
                  config_file_name,
                  chef_production_message[4],
                  chef_production_message[5],
                  sweet_patisseries_shm_sem_message,
                  savory_patisseries_shm_sem_message
                  ,NULL);
                  
                perror("execlp failed for baker (sweet_savory_patiss_bake)");
                exit(EXIT_FAILURE);
            }
            if (counter == 2) {
                execlp("../bin/bread_bake",
                 "../bin/bread_bake",
                 config_file_name,
                 chef_production_message[0],
                 bread_catagories_shm_sem_message
                 ,NULL);
                
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
            execlp("../bin/supplier", "../bin/supplier", config_file_name,basic_items_message, NULL);
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
    
    /*
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
    }*/
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


    shm_paste_id = create_shm('J', 64, &shm_paste_ptr);
    sem_paste_id = create_sem('J');

    shm_cake_paste_id = create_shm('K', 64, &shm_cake_paste_ptr);
    sem_cake_paste_id = create_sem('K');

    shm_sandwiches_id = create_shm('L', 64, &shm_sandwiches_ptr);
    sem_sandwiches_id = create_sem('L');

    shm_sweets_paste_id = create_shm('M', 64, &shm_sweets_paste_ptr);
    sem_sweets_paste_id = create_sem('M');

    shm_sweet_patiss_paste_id = create_shm('N', 64, &shm_sweet_patiss_paste_ptr);
    sem_sweet_patiss_paste_id = create_sem('N');

    shm_savory_patiss_paste_id = create_shm('O', 64, &shm_savory_patiss_paste_ptr);
    sem_savory_patiss_paste_id = create_sem('O');
    
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
    /*chef production*/
    cleanup_shm(shm_paste_id, shm_paste_ptr);
    cleanup_sem(sem_paste_id);

    cleanup_shm(shm_cake_paste_id, shm_cake_paste_ptr);
    cleanup_sem(sem_cake_paste_id);

    cleanup_shm(shm_sandwiches_id, shm_sandwiches_ptr);
    cleanup_sem(sem_sandwiches_id);

    cleanup_shm(shm_sweets_paste_id, shm_sweets_paste_ptr);
    cleanup_sem(sem_sweets_paste_id);

    cleanup_shm(shm_sweet_patiss_paste_id, shm_sweet_patiss_paste_ptr);
    cleanup_sem(sem_sweet_patiss_paste_id);

    cleanup_shm(shm_savory_patiss_paste_id, shm_savory_patiss_paste_ptr);
    cleanup_sem(sem_savory_patiss_paste_id);
}
void cleanup_shm_sem_bakery(int shm_ids[], int sem_ids[], char *shm_ptrs[], int count) {
    for (int i = 0; i < count; i++) {
        // Cleanup shared memory if pointer exists
        if (shm_ptrs && shm_ptrs[i] != NULL) {
            cleanup_shm(shm_ids[i], shm_ptrs[i]);
            shm_ptrs[i] = NULL;  // Prevent dangling pointer
        }
        
        // Cleanup semaphore if ID is valid
        if (sem_ids && sem_ids[i] >= 0) {  // Assuming negative IDs are invalid
            cleanup_sem(sem_ids[i]);
        }
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

char* create_shm_sem_message(int* shm_ids, int* sem_ids, int count) {
    char* message = (char *)malloc(MAX_BUFFER_SIZE);
    if (message == NULL) {
        perror("malloc failed");
        exit(1);
    }

    // Initialize an empty string
    message[0] = '\0';

    char buffer[32]; // Temporary buffer for each ID pair

    for (int i = 0; i < count; i++) {
        // Format "shm_id sem_id " and append to message
        snprintf(buffer, sizeof(buffer), "%d %d ", shm_ids[i], sem_ids[i]);
        strcat(message, buffer);
    }

    // Remove the trailing space if there are any entries
    if (count > 0) {
        message[strlen(message) - 1] = '\0';
    }

    return message;
}
void initialize_all_shm_and_sem_for_sale(
	int *bread_catagories_shm_id,int *bread_catagories_sem_id,char **bread_catagories_shm_ptr,
	int *sandwiches_shm_id,int *sandwiches_sem_id,char **sandwiches_shm_ptr,
	int *cake_flavors_shm_id,int *cake_flavors_sem_id,char **cake_flavors_shm_ptr,
	int *sweets_flavors_shm_id,int *sweets_flavors_sem_id,char **sweets_flavors_shm_ptr,
	int *sweet_patisseries_shm_id,int *sweet_patisseries_sem_id,char **sweet_patisseries_shm_ptr,
	int *savory_patisseries_shm_id,int *savory_patisseries_sem_id,char **savory_patisseries_shm_ptr){
	
	
	initialize_shm_and_sem(
        bread_catagories_shm_id,
        bread_catagories_sem_id,
        bread_catagories_shm_ptr,
        config.bread_catagories_number,
        'A' // starting base key for bread categories
    	);

    	initialize_shm_and_sem(
        sandwiches_shm_id,
        sandwiches_sem_id,
        sandwiches_shm_ptr,
        config.sandwiches_number,
        'K' // starting base key for sandwiches
    	);

    	initialize_shm_and_sem(
        cake_flavors_shm_id,
        cake_flavors_sem_id,
        cake_flavors_shm_ptr,
        config.cake_flavors_number,
        'U' // starting base key for cake flavors
    	);

    	initialize_shm_and_sem(
        sweets_flavors_shm_id,
        sweets_flavors_sem_id,
        sweets_flavors_shm_ptr,
        config.sweets_flavors_number,
        'e' // starting base key for sweets flavors (lowercase to avoid overlap)
    	);

    	initialize_shm_and_sem(
        sweet_patisseries_shm_id,
        sweet_patisseries_sem_id,
        sweet_patisseries_shm_ptr,
        config.sweet_patisseries_number,
        'o' // starting base key for sweet patisseries
    	);

    	initialize_shm_and_sem(
        savory_patisseries_shm_id,
        savory_patisseries_sem_id,
        savory_patisseries_shm_ptr,
        config.savory_patisseries_number,
        'y' // starting base key for savory patisseries
    	);
	}
	
void create_all_shm_and_sem_for_sale_message(
	int *bread_catagories_shm_id,int *bread_catagories_sem_id,
	int *sandwiches_shm_id,int *sandwiches_sem_id,
	int *cake_flavors_shm_id,int *cake_flavors_sem_id,
	int *sweets_flavors_shm_id,int *sweets_flavors_sem_id,
	int *sweet_patisseries_shm_id,int *sweet_patisseries_sem_id,
	int *savory_patisseries_shm_id,int *savory_patisseries_sem_id){
	
	
    	// For Bread Categories
	bread_catagories_shm_sem_message = create_shm_sem_message(
	    bread_catagories_shm_id, 
	    bread_catagories_sem_id, 
	    config.bread_catagories_number
	);

	// For Sandwiches
	sandwiches_shm_sem_message = create_shm_sem_message(
	    sandwiches_shm_id, 
	    sandwiches_sem_id, 
	    config.sandwiches_number
	);

	// For Cake Flavors
	cake_flavors_shm_sem_message = create_shm_sem_message(
	    cake_flavors_shm_id, 
	    cake_flavors_sem_id, 
	    config.cake_flavors_number
	);

	// For Sweets Flavors
	sweets_flavors_shm_sem_message = create_shm_sem_message(
	    sweets_flavors_shm_id, 
	    sweets_flavors_sem_id, 
	    config.sweets_flavors_number
	);

	// For Sweet Patisseries
	sweet_patisseries_shm_sem_message = create_shm_sem_message(
	    sweet_patisseries_shm_id, 
	    sweet_patisseries_sem_id, 
	    config.sweet_patisseries_number
	);

	// For Savory Patisseries
	savory_patisseries_shm_sem_message = create_shm_sem_message(
	    savory_patisseries_shm_id, 
	    savory_patisseries_sem_id, 
	    config.savory_patisseries_number
	);
	
	}	


void cleanup_shm_sem_for_sale(
        int *bread_catagories_shm_id,int *bread_catagories_sem_id,char **bread_catagories_shm_ptr,
	int *sandwiches_shm_id,int *sandwiches_sem_id,char **sandwiches_shm_ptr,
	int *cake_flavors_shm_id,int *cake_flavors_sem_id,char **cake_flavors_shm_ptr,
	int *sweets_flavors_shm_id,int *sweets_flavors_sem_id,char **sweets_flavors_shm_ptr,
	int *sweet_patisseries_shm_id,int *sweet_patisseries_sem_id,char **sweet_patisseries_shm_ptr,
	int *savory_patisseries_shm_id,int *savory_patisseries_sem_id,char **savory_patisseries_shm_ptr){
	// Clean up Bread Categories
	cleanup_shm_sem_bakery(bread_catagories_shm_id, bread_catagories_sem_id, 
		              bread_catagories_shm_ptr, config.bread_catagories_number);

	// Clean up Sandwiches
	cleanup_shm_sem_bakery(sandwiches_shm_id, sandwiches_sem_id, 
		              sandwiches_shm_ptr, config.sandwiches_number);

	// Clean up Cake Flavors
	cleanup_shm_sem_bakery(cake_flavors_shm_id, cake_flavors_sem_id, 
		              cake_flavors_shm_ptr, config.cake_flavors_number);

	// Clean up Sweets Flavors
	cleanup_shm_sem_bakery(sweets_flavors_shm_id, sweets_flavors_sem_id, 
		              sweets_flavors_shm_ptr, config.sweets_flavors_number);

	// Clean up Sweet Patisseries
	cleanup_shm_sem_bakery(sweet_patisseries_shm_id, sweet_patisseries_sem_id, 
		              sweet_patisseries_shm_ptr, config.sweet_patisseries_number);

	// Clean up Savory Patisseries
	cleanup_shm_sem_bakery(savory_patisseries_shm_id, savory_patisseries_sem_id, 
		              savory_patisseries_shm_ptr, config.savory_patisseries_number);
	}


void create_production_items_message() {
   // Allocate memory for each item
    for (int i = 0; i < 6; i++) {
        chef_production_message[i] = (char *)malloc(MAX_BUFFER_SIZE);
        if (chef_production_message[i] == NULL) {
            perror("malloc failed");
            exit(1);
        }
    }

    // Fill each cell with shm and sem id
    snprintf(chef_production_message[0], MAX_BUFFER_SIZE, "%d %d", shm_paste_id, sem_paste_id);
    snprintf(chef_production_message[1], MAX_BUFFER_SIZE, "%d %d", shm_cake_paste_id, sem_cake_paste_id);
    snprintf(chef_production_message[2], MAX_BUFFER_SIZE, "%d %d", shm_sandwiches_id, sem_sandwiches_id);
    snprintf(chef_production_message[3], MAX_BUFFER_SIZE, "%d %d", shm_sweets_paste_id, sem_sweets_paste_id);
    snprintf(chef_production_message[4], MAX_BUFFER_SIZE, "%d %d", shm_sweet_patiss_paste_id, sem_sweet_patiss_paste_id);
    snprintf(chef_production_message[5], MAX_BUFFER_SIZE, "%d %d", shm_savory_patiss_paste_id, sem_savory_patiss_paste_id);

}




  
void write_shared_int(int sem_id, char *shm_ptr, int new_value) {
    printf("[DEBUG] File path: parent bakery \n");
    printf("[DEBUG] Starting write_shared_int()...\n");

    // Acquire write lock
    struct sembuf op_wait_write = {WRITE_LOCK, -1, SEM_UNDO};
    printf("[DEBUG] Locking WRITE_LOCK for writing\n");
    semop(sem_id, &op_wait_write, 1);

    // Write new value into shared memory
    *((int *)shm_ptr) = new_value;
    printf("[DEBUG] Wrote new value to shared memory: %d\n", new_value);

    // Release write lock
    struct sembuf op_release_write = {WRITE_LOCK, 1, SEM_UNDO};
    printf("[DEBUG] Releasing WRITE_LOCK after writing\n");
    semop(sem_id, &op_release_write, 1);

    printf("[DEBUG] Finished write_shared_int()\n\n");
}
void initialize_shm_and_sem(
    int *shm_ids, 
    int *sem_ids, 
    char **shm_ptrs, 
    int count, 
    char base_key
) {
    for (int i = 0; i < count; i++) {
        char key = base_key + i;
        shm_ids[i] = create_shm(key, 64, &shm_ptrs[i]);
        sem_ids[i] = create_sem(key);
    }
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


















