#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <signal.h>


#define MUTEX 0       // Controls access to read_count
#define READ_COUNT 1  // Tracks number of readers
#define WRITE_LOCK 2  // Ensures exclusive write access



void parse_ids(const char *buffer);
void attach_shm_basic_items();
void deattach_shm(int shmid, char *ptr);
void deattach_all_shm();
void modify_shared_int(int sem_id, char *shm_ptr, int value_to_add);
void positive_random_updater();

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
void sigusr1_handler(int signum) {
    printf("Received SIGUSR1 signal. Exiting...\n");
    deattach_all_shm();
    exit(0);
}
int main(int argc, char **argv) {
    // catch SIGUSR1 signal
    signal(SIGUSR1, sigusr1_handler);
    pid_t pid = getpid();
    
    parse_ids(argv[2]);
    
    attach_shm_basic_items();
    
    
    positive_random_updater();
    
    printf("From Supplier :Current Process ID: %d\n", pid);
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

    // Salt
    shm_salt_ptr = (char *)shmat(shm_salt_id, NULL, 0);
    if (shm_salt_ptr == (char *)-1) {
        perror("shmat salt failed");
        exit(1);
    }

    // Sweet Items
    shm_sweet_items_ptr = (char *)shmat(shm_sweet_items_id, NULL, 0);
    if (shm_sweet_items_ptr == (char *)-1) {
        perror("shmat sweet items failed");
        exit(1);
    }

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
    deattach_shm(shm_salt_id, shm_salt_ptr);
    deattach_shm(shm_sweet_items_id, shm_sweet_items_ptr);
    deattach_shm(shm_cheese_id, shm_cheese_ptr);
    deattach_shm(shm_salami_id, shm_salami_ptr);
     
}

void modify_shared_int(int sem_id, char *shm_ptr, int value_to_add) {
    static int read_count = 0; // Track number of readers inside this function
    printf("[DEBUG] File path : Supplier  \n");
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
}


void positive_random_updater() {
    srand(time(0));
    
    // Array of all possible shared memory configurations
    struct {
        int sem_id;
        char* shm_ptr;
        const char* name;
    } memories[] = {
        {sem_wheat_id, shm_wheat_ptr, "WHEAT"},
        {sem_yeast_id, shm_yeast_ptr, "YEAST"},
        {sem_butter_id, shm_butter_ptr, "BUTTER"},
        {sem_milk_id, shm_milk_ptr, "MILK"},
        {sem_sugar_id, shm_sugar_ptr, "SUGAR"},
        {sem_salt_id, shm_salt_ptr, "SALT"},
        {sem_sweet_items_id, shm_sweet_items_ptr, "SWEET_ITEMS"},
        {sem_cheese_id, shm_cheese_ptr, "CHEESE"},
        {sem_salami_id, shm_salami_ptr, "SALAMI"}
    };
    const int num_memories = sizeof(memories) / sizeof(memories[0]);

    while (true) {
        // Determine how many memories to update (1 to all)
        int num_to_update = (rand() % num_memories) + 1;
        
        bool update_mask[num_memories];
            for (int i = 0; i < num_memories; i++) {
            update_mask[i] = false;
        }
        for (int i = 0; i < num_to_update; i++) {
            int idx;
            do {
                idx = rand() % num_memories;
            } while (update_mask[idx]); // Ensure we don't select the same one twice
            update_mask[idx] = true;
        }

        // Determine value to add (1-10)
        int value = (rand() % 6) + 1;
        
        // Update all selected memories
        for (int i = 0; i < num_memories; i++) {
            if (update_mask[i]) {
                modify_shared_int(memories[i].sem_id, memories[i].shm_ptr, value);
                printf("Added %d to %s. New value: %d\n", 
                       value, memories[i].name, *((int*)memories[i].shm_ptr));
            }
        }

        // Determine sleep time (1-10 seconds)
        int sleep_time = (rand() % 5) + 1;
        sleep(sleep_time);
    }
}


