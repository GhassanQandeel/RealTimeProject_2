#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <string.h>
#include "../include/config.h"
#include <signal.h>


#define MUTEX 0       // Controls access to read_count
#define READ_COUNT 1  // Tracks number of readers
#define WRITE_LOCK 2  // Ensures exclusive write access

void parse_ids(const char *buffer);
void attach_all_shm();

void deattach_shm(int shmid, char *ptr);
void deattach_all_shm();

int modify_shared_int(int sem_id, char *shm_ptr, int value_to_add);
void decode_shm_sem_message(const char* message, int* shm_ids, int* sem_ids, int max_count);
void attach_shm_segments(int* shm_ids, char** shm_ptrs, int count);
void detach_shm_segments(char** shm_ptrs, int count);
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

void do_work();
int shm_sweet_patiss_paste_id;
char *shm_sweet_patiss_paste_ptr;


int sem_sweet_patiss_paste_id;



int shm_savory_patiss_paste_id;
char *shm_savory_patiss_paste_ptr;

int sem_savory_patiss_paste_id;

int *sweet_patisseries_shm_id;
char **sweet_patisseries_shm_ptr;
int *sweet_patisseries_sem_id;

int *savory_patisseries_shm_id;
char **savory_patisseries_shm_ptr;
int *savory_patisseries_sem_id;


char config_file_name[30];
Config config;


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
    detach_shm_segments(sweet_patisseries_shm_ptr, config.sweet_patisseries_number);
    detach_shm_segments(savory_patisseries_shm_ptr, config.savory_patisseries_number);
    exit(0);
}
int main(int argc, char **argv) {
    // catch SIGUSR1 signal
    signal(SIGUSR1, sigusr1_handler);

    	strcpy(config_file_name, argv[1]);
	
    	if (argc < 2) {
	       fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
	        return EXIT_FAILURE;
	    }

	
	if (load_config(config_file_name, &config) == 0) {
	    //printConfig(&config);
	    printf("Success to load configuration.From savory,sweet\n");
	} else {
	    fprintf(stderr, "Failed to load configuration.\n");
	}
	
	
    // Create malloc
    sweet_patisseries_shm_id = malloc(config.sweet_patisseries_number * sizeof(int));
    sweet_patisseries_shm_ptr = malloc(config.sweet_patisseries_number * sizeof(char *));
    sweet_patisseries_sem_id = malloc(config.sweet_patisseries_number * sizeof(int));

    // Create malloc 
    savory_patisseries_shm_id = malloc(config.savory_patisseries_number * sizeof(int));
    savory_patisseries_shm_ptr = malloc(config.savory_patisseries_number * sizeof(char *));
    savory_patisseries_sem_id = malloc(config.savory_patisseries_number * sizeof(int));


	
	
	
	
    sscanf(argv[2], "%d %d", &shm_sweet_patiss_paste_id, &sem_sweet_patiss_paste_id);
    sscanf(argv[3], "%d %d", &shm_savory_patiss_paste_id, &sem_savory_patiss_paste_id);

	
	
    
    decode_shm_sem_message(argv[4], sweet_patisseries_shm_id, sweet_patisseries_sem_id, config.sweet_patisseries_number);
    decode_shm_sem_message(argv[5], savory_patisseries_shm_id, savory_patisseries_sem_id, config.savory_patisseries_number);

    attach_all_shm();
    attach_shm_segments(sweet_patisseries_shm_id,sweet_patisseries_shm_ptr, config.sweet_patisseries_number);
    attach_shm_segments(savory_patisseries_shm_id,savory_patisseries_shm_ptr, config.savory_patisseries_number);
    
    
    while(1){
        do_work();
        sleep(1);
    }
    printf("From patiss bake :Current Process ID: %d\n", getpid());
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



void attach_all_shm() {
    shm_sweet_patiss_paste_ptr= (char *)shmat(shm_sweet_patiss_paste_id, NULL, 0);
    if (shm_sweet_patiss_paste_ptr == (char *)-1) {
        perror("shmat spp failed");
        exit(1);
    }
    shm_savory_patiss_paste_ptr = (char *)shmat(shm_savory_patiss_paste_id, NULL, 0);
    if (shm_savory_patiss_paste_ptr == (char *)-1) {
        perror("shmat savory patiss failed");
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
    deattach_shm(shm_sweet_patiss_paste_id, shm_sweet_patiss_paste_ptr); 
    deattach_shm(shm_savory_patiss_paste_id, shm_savory_patiss_paste_ptr); 

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

void do_work(){
      //random time to  achive the code 
      int processing_time = (rand() % 6) + 1;
    
      //random amount to take (between 1-3 units)
      int amount = (rand() % 3) + 1;
      
      
      if (modify_shared_int(sem_sweet_patiss_paste_id, shm_sweet_patiss_paste_ptr, -amount) <= 0) {
          sleep(2);
      }

      if (modify_shared_int(sem_savory_patiss_paste_id, shm_savory_patiss_paste_ptr, -amount) <= 0) {
        sleep(2);
    }
      
      
      
  
      // Simulate processing time
      sleep(processing_time);
  
  
  
      for (int i = 0; i < config.sweet_patisseries_number; i++)
      {
        
       
        modify_shared_int(sweet_patisseries_sem_id[i], sweet_patisseries_shm_ptr[i], amount*2);
          
      
      }
      for (int i = 0; i < config.savory_patisseries_number; i++)
      {
       
        modify_shared_int(savory_patisseries_sem_id[i], savory_patisseries_shm_ptr[i], amount*2);
          
      }
}