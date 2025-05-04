#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <string.h>
#include "../include/config.h"


#define MUTEX 0       // Controls access to read_count
#define READ_COUNT 1  // Tracks number of readers
#define WRITE_LOCK 2  // Ensures exclusive write access

void parse_ids(const char *buffer);
void attach_shm_basic_items();

void deattach_shm(int shmid, char *ptr);
void deattach_all_shm();

int modify_shared_int(int sem_id, char *shm_ptr, int value_to_add);
void do_work(int* bread_catagories_sem_id , char** bread_catagories_shm_ptr,int* sandwiches_sem_id,char** sandwiches_shm_ptr );

void decode_shm_sem_message(const char* message, int* shm_ids, int* sem_ids, int max_count);
void attach_shm_segments(int* shm_ids, char** shm_ptrs, int count);
void detach_shm_segments(char** shm_ptrs, int count);
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

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


char config_file_name[30];
Config config;
int *bread_catagories_sem_id;  // Dynamically allocated
char **bread_catagories_shm_ptr;  // Dynamically allocated
int *bread_catagories_shm_id;

int *sandwiches_shm_id;
int *sandwiches_sem_id;
char **sandwiches_shm_ptr;


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

void sigusr1_handler(int signum) {
    printf("Received SIGUSR1 signal. Exiting...\n");
    deattach_all_shm();
    detach_shm_segments(bread_catagories_shm_ptr, config.bread_catagories_number);
    detach_shm_segments(sandwiches_shm_ptr, config.sandwiches_number);
    //Free malloc 
    free(bread_catagories_shm_ptr);
    free(bread_catagories_sem_id);
    free(bread_catagories_shm_id);
    free(sandwiches_shm_ptr);
    free(sandwiches_sem_id);
    free(sandwiches_shm_id);
    exit(0);
}
int main(int argc, char **argv) {

    	strcpy(config_file_name, argv[1]);
	
    	if (argc < 2) {
	       fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
	        return EXIT_FAILURE;
	    }

	// catch sigusr1 signal
    signal(SIGUSR1, sigusr1_handler);
	if (load_config(config_file_name, &config) == 0) {
	    //printConfig(&config);
	    printf("Success to load configuration.From sandwsiches\n");
	} else {
	    fprintf(stderr, "Failed to load configuration.\n");
	}
	
    // Create malloc
    bread_catagories_shm_id = malloc(config.bread_catagories_number * sizeof(int));
    bread_catagories_shm_ptr = malloc(config.bread_catagories_number * sizeof(char *));
    bread_catagories_sem_id = malloc(config.bread_catagories_number * sizeof(int));

    // Create malloc
    sandwiches_shm_id = malloc(config.sandwiches_number * sizeof(int));
    sandwiches_shm_ptr = malloc(config.sandwiches_number * sizeof(char *));
    sandwiches_sem_id = malloc(config.sandwiches_number * sizeof(int));
	
	
	for (int i = 0; i < argc; i++) {
        printf("argv[%d] = %s\n", i, argv[i]);
    	}
	
	
	
	
    	parse_ids(argv[2]);
    	
  	    decode_shm_sem_message(argv[3], bread_catagories_shm_id, bread_catagories_sem_id, config.bread_catagories_number);
  	    decode_shm_sem_message(argv[4], sandwiches_shm_id, sandwiches_sem_id, config.sandwiches_number);
  	
    	attach_shm_basic_items();
    	attach_shm_segments(bread_catagories_shm_id,bread_catagories_shm_ptr, config.bread_catagories_number);
    	attach_shm_segments(sandwiches_shm_id,sandwiches_shm_ptr, config.sandwiches_number);
    	
    	/*
        printf("BREADS SHM :\n");
    	print_array(bread_catagories_shm_id,config.bread_catagories_number);
        printf("BREADS sem :\n");
        print_array(bread_catagories_sem_id,config.bread_catagories_number);
        printf("Sandwiches SHM :\n");
    	print_array(sandwiches_shm_id,config.sandwiches_number);
        printf("Sandwiches sem :\n");
        print_array(sandwiches_sem_id,config.sandwiches_number);
       */


        printf("From sandwischs :Current Process ID: %d\n", getpid());
        return 0;
}

void decode_shm_sem_message(const char* message, int* shm_ids, int* sem_ids, int max_count) {
    if (message == NULL || message[0] == '\0') {
        return;
    }

    const char* ptr = message;
    int count = 0;
    
    while (*ptr != '\0' && count < max_count) {
        // Parse shm_id
        shm_ids[count] = atoi(ptr);
        // Move past the number
        while (*ptr != '\0' && *ptr != ' ') ptr++;
        if (*ptr == ' ') ptr++;  // skip space
        
        // Parse sem_id
        sem_ids[count] = atoi(ptr);
        // Move past the number
        while (*ptr != '\0' && *ptr != ' ') ptr++;
        if (*ptr == ' ') ptr++;  // skip space
        
        count++;
    }
    
   
}


void parse_ids(const char *buffer) {
    sscanf(buffer,
        "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
        &shm_wheat_id, &sem_wheat_id,
        &shm_yeast_id, &sem_yeast_id,
        &shm_butter_id, &sem_butter_id,
        &shm_milk_id, &sem_milk_id,
        &shm_sugar_id, &sem_sugar_id,
        &shm_salt_id, &sem_salt_id,
        &shm_sweet_items_id, &sem_sweet_items_id,
        &shm_cheese_id, &sem_cheese_id,
        &shm_salami_id, &sem_salami_id
    );
}

void attach_shm_basic_items() {
    // Cheese
    shm_cheese_ptr = (char *)shmat(shm_cheese_id, NULL, 0);
    if (shm_cheese_ptr == (char *)-1) {
        perror("shmat cheese failed");
        exit(1);
    }

    // Salami
    shm_salami_ptr = (char *)shmat(shm_salami_id, NULL, 0);
    if (shm_salami_ptr == (char *)-1) {
        perror("shmat salami failed");
        exit(1);
    }
    
    
}


void attach_shm_segments(int* shm_ids, char** shm_ptrs, int count) {
    for (int i = 0; i < count; i++) {
        shm_ptrs[i] = (char *)shmat(shm_ids[i], NULL, 0);
        if (shm_ptrs[i] == (char *)-1) {
            perror("shmat failed");
            
            // Cleanup any already attached segments
            for (int j = 0; j < i; j++) {
                shmdt(shm_ptrs[j]);
            }
            
           
        }
    }
}

void detach_shm_segments(char** shm_ptrs, int count) {
    for (int i = 0; i < count; i++) {
        if (shm_ptrs[i] != NULL && shm_ptrs[i] != (char *)-1) {
            if (shmdt(shm_ptrs[i]) == -1) {
                perror("shmdt failed");
                // Continue trying to detach others even if one fails
            }
        }
    }
}


void deattach_shm(int shmid, char *ptr) {
    if (shmdt(ptr) == -1) {
        perror("shmdt failed");
    }
   
}
void deattach_all_shm() {

    deattach_shm(shm_cheese_id, shm_cheese_ptr);
    deattach_shm(shm_salami_id, shm_salami_ptr);
    
    
}
int modify_shared_int(int sem_id, char *shm_ptr, int value_to_add) {
    static int read_count = 0; // Track number of readers inside this function
    printf("[DEBUG] File path:  paste \n");
    printf("[DEBUG] Starting modify_shared_int()...\n");

    // --- Start Reader-Writer synchronization (like your reader) ---

    // Acquire mutex to protect read_count
    struct sembuf op_wait_mutex = {MUTEX, -1, SEM_UNDO};
    semop(sem_id, &op_wait_mutex, 1);

    read_count++;
    
    if (read_count == 1) {
        // First reader locks write lock
        struct sembuf op_wait_write_lock = {WRITE_LOCK, -1, SEM_UNDO};
        
        semop(sem_id, &op_wait_write_lock, 1);
    }

    // Release mutex
    struct sembuf op_release_mutex = {MUTEX, 1, SEM_UNDO};
    
    semop(sem_id, &op_release_mutex, 1);

    // --- Critical Section: Reading value ---
    int current_value = *((int *)shm_ptr);  // Read int from shared memory
    printf("[DEBUG] Read value from shared memory: %d\n", current_value);
    // --- End Reading ---

    // Acquire mutex again to safely modify read_count
    
    semop(sem_id, &op_wait_mutex, 1);

    read_count--;
    
    if (read_count == 0) {
        // Last reader releases write lock
        struct sembuf op_release_write_lock = {WRITE_LOCK, 1, SEM_UNDO};
        semop(sem_id, &op_release_write_lock, 1);
    }

    semop(sem_id, &op_release_mutex, 1);

    // --- Now become a Writer to modify shared memory ---

    // Acquire write lock
    struct sembuf op_wait_write_lock2 = {WRITE_LOCK, -1, SEM_UNDO};
    semop(sem_id, &op_wait_write_lock2, 1);

    // Modify value
    current_value += value_to_add;
    *((int *)shm_ptr) = current_value;
    printf("[DEBUG] Modified value and wrote back to shared memory: %d\n", current_value);

    // Release write lock
    struct sembuf op_release_write_lock2 = {WRITE_LOCK, 1, SEM_UNDO};
    semop(sem_id, &op_release_write_lock2, 1);
    printf("[DEBUG] Finished modify_shared_int()\n\n");
    return current_value;

}

void do_work(int* bread_catagories_sem_id , char** bread_catagories_shm_ptr,int* sandwiches_sem_id,char** sandwiches_shm_ptr ) {
    //random time to  achive the code 
    int processing_time = (rand() % 6) + 1;
    
    //random amount to take (between 1-3 units)
    int amount = (rand() % 3) + 1;
    
    
    if (modify_shared_int(sem_cheese_id, shm_cheese_ptr, -amount) <= 0) {
        sleep(200);
    }
    
    // Yeast check
    if (modify_shared_int(sem_salami_id, shm_salami_ptr, -amount) <= 0) {
        sleep(200);
    }
    for (int i = 0; i < config.bread_catagories_number; i++)
    {
        if (modify_shared_int(bread_catagories_sem_id[i], bread_catagories_shm_ptr[i], -amount) <= 0) {
            sleep(200);
        }
        
    }
    

    // Simulate processing time
    sleep(processing_time);
    for (int i = 0; i < config.sandwiches_number; i++)
    {
        int paste_int=modify_shared_int(sandwiches_sem_id[i], sandwiches_shm_ptr[i], amount*2);
    }
    }