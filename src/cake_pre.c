#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <string.h>



#define MUTEX 0       // Controls access to read_count
#define READ_COUNT 1  // Tracks number of readers
#define WRITE_LOCK 2  // Ensures exclusive write access
#include <signal.h>





void parse_ids(const char *buffer);
void attach_shm_basic_items();
void deattach_shm(int shmid, char *ptr);
void deattach_all_shm();
int modify_shared_int(int sem_id, char *shm_ptr, int value_to_add);
void do_work();



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



int shm_cake_paste_id;
char *shm_cake_paste_ptr;
int sem_cake_paste_id;


void sigusr1_handler(int signum) {
    printf("Received SIGUSR1 signal. Exiting...\n");
    deattach_all_shm();
    exit(0);
}
int main(int argc, char **argv) {

    
    parse_ids(argv[2]);
    sscanf(argv[3], "%d %d", &shm_cake_paste_id, &sem_cake_paste_id);
    
    // catch sigusr1 signal
    signal(SIGUSR1, sigusr1_handler);
    
    attach_shm_basic_items();
    
    sleep(10);
    while(1){
    do_work();
    }
    
    return 0;
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
    // Wheat
    shm_wheat_ptr = (char *)shmat(shm_wheat_id, NULL, 0);
    if (shm_wheat_ptr == (char *)-1) {
        perror("shmat wheat failed");
        exit(1);
    }

    // Yeast
    shm_yeast_ptr = (char *)shmat(shm_yeast_id, NULL, 0);
    if (shm_yeast_ptr == (char *)-1) {
        perror("shmat yeast failed");
        exit(1);
    }

    // Butter
    shm_butter_ptr = (char *)shmat(shm_butter_id, NULL, 0);
    if (shm_butter_ptr == (char *)-1) {
        perror("shmat butter failed");
        exit(1);
    }

    // Milk
    shm_milk_ptr = (char *)shmat(shm_milk_id, NULL, 0);
    if (shm_milk_ptr == (char *)-1) {
        perror("shmat milk failed");
        exit(1);
    }

    // Sugar
    shm_sugar_ptr = (char *)shmat(shm_sugar_id, NULL, 0);
    if (shm_sugar_ptr == (char *)-1) {
        perror("shmat sugar failed");
        exit(1);
    }
    
    shm_cake_paste_ptr= (char *)shmat(shm_cake_paste_id, NULL, 0);
    if (shm_cake_paste_ptr == (char *)-1) {
        perror("shmat salami failed");
        exit(1);
    }
    
}

void deattach_shm(int shmid, char *ptr) {
    if (shmdt(ptr) == -1) {
        perror("shmdt failed");
    }
    
}
void deattach_all_shm() {
    deattach_shm(shm_wheat_id, shm_wheat_ptr);
    deattach_shm(shm_yeast_id, shm_yeast_ptr);
    deattach_shm(shm_butter_id, shm_butter_ptr);
    deattach_shm(shm_milk_id, shm_milk_ptr);
    deattach_shm(shm_sugar_id, shm_sugar_ptr);
    deattach_shm(shm_cake_paste_id, shm_cake_paste_ptr); 
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

void do_work() {
    //random time to  achive the code 
    int processing_time = (rand() % 3) + 1;
    
    //random amount to take (between 1-3 units)
    int amount = (rand() % 3) + 1;
    
    
    if (modify_shared_int(sem_wheat_id, shm_wheat_ptr, -amount) <= 0) {
        sleep(200);
    }
    
    // Yeast check
    if (modify_shared_int(sem_yeast_id, shm_yeast_ptr, -amount) <= 0) {
        sleep(200);
    }
    
    // Butter check
    if (modify_shared_int(sem_butter_id, shm_butter_ptr, -amount) <= 0) {
        sleep(200);
    }
    
    // Milk check
    if (modify_shared_int(sem_milk_id, shm_milk_ptr, -amount) <= 0) {
        sleep(200);
    }
    
    // Salt check
    if (modify_shared_int(sem_sugar_id, shm_sugar_ptr, -amount) <= 0) {
        sleep(200);
    }


    // Simulate processing time
    sleep(processing_time);
    
    // Add to paste (increase)
    int paste_int=modify_shared_int(sem_cake_paste_id, shm_cake_paste_ptr, amount*2);
    }