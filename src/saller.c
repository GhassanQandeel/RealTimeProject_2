#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <signal.h>
#include "../include/local.h"
#include "../include/config.h"
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#define MUTEX 0       // Controls access to read_count
#define READ_COUNT 1  // Tracks number of readers
#define WRITE_LOCK 2  // Ensures exclusive write access


int mid;
MESSAGE msg_rcv;
MESSAGE msg_snd;
Config config;
// Global variables for Bread Categories
int *bread_catagories_shm_id;
int *bread_catagories_sem_id;
char **bread_catagories_shm_ptr;

// Global variables for Sandwiches
int *sandwiches_shm_id;
int *sandwiches_sem_id;
char **sandwiches_shm_ptr;

// Global variables for Cake Flavors
int *cake_flavors_shm_id;
int *cake_flavors_sem_id;
char **cake_flavors_shm_ptr;

// Global variables for Sweets Flavors
int *sweets_flavors_shm_id;
int *sweets_flavors_sem_id;
char **sweets_flavors_shm_ptr;

// Global variables for Sweet Patisseries
int *sweet_patisseries_shm_id;
int *sweet_patisseries_sem_id;
char **sweet_patisseries_shm_ptr;

// Global variables for Savory Patisseries
int *savory_patisseries_shm_id;
int *savory_patisseries_sem_id;
char **savory_patisseries_shm_ptr;

void recieve_message(int mid, int saller_id); 
void send_item();
void do_work();
int modify_shared_int(int sem_id, char *shm_ptr, int value_to_add);
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

void sigusr1_handler(int signum);
int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: seller <config> <seller_id> <mid>\n");
        exit(EXIT_FAILURE);
    }
    // catch sigusr1 signal
    signal(SIGUSR1, sigusr1_handler);
    const char *config_file = argv[1];
    int seller_id = atoi(argv[2]);
    mid = atoi(argv[3]);
    // Allocate memory for Bread Categories
    bread_catagories_shm_id = malloc(config.bread_catagories_number * sizeof(int));
    bread_catagories_sem_id = malloc(config.bread_catagories_number * sizeof(int));
    bread_catagories_shm_ptr = malloc(config.bread_catagories_number * sizeof(char *));

    // Allocate memory for Sandwiches
    sandwiches_shm_id = malloc(config.sandwiches_number * sizeof(int));
    sandwiches_sem_id = malloc(config.sandwiches_number * sizeof(int));
    sandwiches_shm_ptr = malloc(config.sandwiches_number * sizeof(char *));

    // Allocate memory for Cake Flavors
    cake_flavors_shm_id = malloc(config.cake_flavors_number * sizeof(int));
    cake_flavors_sem_id = malloc(config.cake_flavors_number * sizeof(int));
    cake_flavors_shm_ptr = malloc(config.cake_flavors_number * sizeof(char *));

    // Allocate memory for Sweets Flavors
    sweets_flavors_shm_id = malloc(config.sweets_flavors_number * sizeof(int));
    sweets_flavors_sem_id = malloc(config.sweets_flavors_number * sizeof(int));
    sweets_flavors_shm_ptr = malloc(config.sweets_flavors_number * sizeof(char *));

    // Allocate memory for Sweet Patisseries
    sweet_patisseries_shm_id = malloc(config.sweet_patisseries_number * sizeof(int));
    sweet_patisseries_sem_id = malloc(config.sweet_patisseries_number * sizeof(int));
    sweet_patisseries_shm_ptr = malloc(config.sweet_patisseries_number * sizeof(char *));

    // Allocate memory for Savory Patisseries
    savory_patisseries_shm_id = malloc(config.savory_patisseries_number * sizeof(int));
    savory_patisseries_sem_id = malloc(config.savory_patisseries_number * sizeof(int));
    savory_patisseries_shm_ptr = malloc(config.savory_patisseries_number * sizeof(char *)); 

    decode_shm_sem_message(argv[4], bread_catagories_shm_id, bread_catagories_sem_id, config.bread_catagories_number);
    decode_shm_sem_message(argv[5], sandwiches_shm_id, sandwiches_sem_id, config.sandwiches_number);
    decode_shm_sem_message(argv[6], cake_flavors_shm_id, cake_flavors_sem_id, config.cake_flavors_number);
    decode_shm_sem_message(argv[7], sweets_flavors_shm_id, sweets_flavors_sem_id, config.sweets_flavors_number);
    decode_shm_sem_message(argv[8], sweet_patisseries_shm_id, sweet_patisseries_sem_id, config.sweet_patisseries_number);
    decode_shm_sem_message(argv[9], savory_patisseries_shm_id, savory_patisseries_sem_id, config.savory_patisseries_number);

    attach_shm_segments(bread_catagories_shm_id, bread_catagories_shm_ptr, config.bread_catagories_number);
    attach_shm_segments(sandwiches_shm_id, sandwiches_shm_ptr, config.sandwiches_number);
    attach_shm_segments(cake_flavors_shm_id, cake_flavors_shm_ptr, config.cake_flavors_number);
    attach_shm_segments(sweets_flavors_shm_id, sweets_flavors_shm_ptr, config.sweets_flavors_number);
    attach_shm_segments(sweet_patisseries_shm_id, sweet_patisseries_shm_ptr, config.sweet_patisseries_number);
    attach_shm_segments(savory_patisseries_shm_id, savory_patisseries_shm_ptr, config.savory_patisseries_number);


    // use the recieve message function to get the message from the queue
    // Recieve message from the queue
    while(1){
    recieve_message(mid, seller_id);
    do_work ();
    send_item();
    }

    return 0;
   
}
void recieve_message(int mid, int saller_id) {
    if (msgrcv(mid, &msg_rcv, sizeof(msg_rcv.buffer), config.customer_number, 0) == -1) { // unique int to request an item
        perror("msgrcv");
        exit(EXIT_FAILURE);
    }
    printf("----------------------------------------------------------------------------------\n");
    printf("Seller %d received message: %s, from customer%d with type:%d\n", saller_id, msg_rcv.buffer, msg_rcv.msg_fm, msg_rcv.message_type); // Print the received message
}
void send_item() {
    msg_snd.message_type = (int)msg_rcv.msg_fm; // Set the message type to the 1 (Requesting item)
    snprintf(msg_snd.buffer, sizeof(msg_snd.buffer), "%s", msg_rcv.buffer); // Copy the order to the message buffer
    printf("----------------------------------------------------------------------------------\n");
    // Send the message to the message queue
    if (msgsnd(mid, &msg_snd, sizeof(msg_snd.buffer), 0) == -1) {
        perror("msgsndseller");
        exit(EXIT_FAILURE);
    }
    printf("Order served: %s, for customer%d\n", msg_snd.buffer, msg_rcv.msg_fm); // Print the sent order
}
void sigusr1_handler(int signum) {
    printf("Received SIGUSR1 signal. Exiting...\n");
    
    // Cleanup - detach from shared memory
    
    detach_shm_segments(bread_catagories_shm_ptr, config.bread_catagories_number);
    detach_shm_segments(sandwiches_shm_ptr, config.sandwiches_number);
    detach_shm_segments(cake_flavors_shm_ptr, config.cake_flavors_number);
    detach_shm_segments(sweets_flavors_shm_ptr, config.sweets_flavors_number);
    detach_shm_segments(sweet_patisseries_shm_ptr, config.sweet_patisseries_number);
    detach_shm_segments(savory_patisseries_shm_ptr, config.savory_patisseries_number);
    // Free memory for Bread Categories
    free(bread_catagories_shm_id);
    free(bread_catagories_sem_id);
    free(bread_catagories_shm_ptr);

    // Free memory for Sandwiches
    free(sandwiches_shm_id);
    free(sandwiches_sem_id);
    free(sandwiches_shm_ptr);

    // Free memory for Cake Flavors
    free(cake_flavors_shm_id);
    free(cake_flavors_sem_id);
    free(cake_flavors_shm_ptr);

    // Free memory for Sweets Flavors
    free(sweets_flavors_shm_id);
    free(sweets_flavors_sem_id);
    free(sweets_flavors_shm_ptr);

    // Free memory for Sweet Patisseries
    free(sweet_patisseries_shm_id);
    free(sweet_patisseries_sem_id);
    free(sweet_patisseries_shm_ptr);

    // Free memory for Savory Patisseries
    free(savory_patisseries_shm_id);
    free(savory_patisseries_sem_id);
    free(savory_patisseries_shm_ptr);

    exit(0);
}
void do_work (){
    sleep(7);
    // Modify each shared memory segment by subtracting a random amount
    for (int i = 0; i < config.bread_catagories_number; i++) {
        int random_value = -(rand() % 5 + 1); // Random negative number between -1 and -5
        modify_shared_int(bread_catagories_sem_id[i], bread_catagories_shm_ptr[i], random_value);
    }

    for (int i = 0; i < config.sandwiches_number; i++) {
        int random_value = -(rand() % 5 + 1);
        modify_shared_int(sandwiches_sem_id[i], sandwiches_shm_ptr[i], random_value);
    }

    for (int i = 0; i < config.cake_flavors_number; i++) {
        int random_value = -(rand() % 5 + 1);
        modify_shared_int(cake_flavors_sem_id[i], cake_flavors_shm_ptr[i], random_value);
    }

    for (int i = 0; i < config.sweets_flavors_number; i++) {
        int random_value = -(rand() % 5 + 1);
        modify_shared_int(sweets_flavors_sem_id[i], sweets_flavors_shm_ptr[i], random_value);
    }

    for (int i = 0; i < config.sweet_patisseries_number; i++) {
        int random_value = -(rand() % 5 + 1);
        modify_shared_int(sweet_patisseries_sem_id[i], sweet_patisseries_shm_ptr[i], random_value);
    }

    for (int i = 0; i < config.savory_patisseries_number; i++) {
        int random_value = -(rand() % 5 + 1);
        modify_shared_int(savory_patisseries_sem_id[i], savory_patisseries_shm_ptr[i], random_value);
    }


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