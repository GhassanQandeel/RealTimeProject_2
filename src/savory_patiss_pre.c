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


void parse_ids(const char *buffer);
void attach_shm_basic_items();
void deattach_shm(int shmid, char *ptr);
void deattach_all_shm();
void modify_shared_int(int sem_id, char *shm_ptr, int value_to_add);
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



int shm_savory_patiss_paste_id;
char *shm_savory_patiss_paste_ptr;

int sem_savory_patiss_paste_id;


int main(int argc, char **argv) {

    pid_t pid = getpid();


    parse_ids(argv[2]);
    sscanf(argv[3], "%d %d", &shm_savory_patiss_paste_id, &sem_savory_patiss_paste_id);
    attach_shm_basic_items();
    
   
    deattach_all_shm();
    printf("From savory :Current Process ID: %d\n", pid);
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
    shm_savory_patiss_paste_ptr = (char *)shmat(shm_savory_patiss_paste_id, NULL, 0);
    if (shm_savory_patiss_paste_ptr == (char *)-1) {
        perror("shmat savory patiss failed");
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
    deattach_shm(shm_savory_patiss_paste_id, shm_savory_patiss_paste_ptr); 
}
void modify_shared_int(int sem_id, char *shm_ptr, int value_to_add) {
    static int read_count = 0; // Track number of readers inside this function

    printf("[DEBUG] File path: savory pati \n");
    printf("[DEBUG] Starting modify_shared_int()...\n");

    // --- Start Reader-Writer synchronization (like your reader) ---

    // Acquire mutex to protect read_count
    struct sembuf op_wait_mutex = {MUTEX, -1, SEM_UNDO};
    printf("[DEBUG] Locking MUTEX to protect read_count\n");
    semop(sem_id, &op_wait_mutex, 1);

    read_count++;
    printf("[DEBUG] Incremented read_count: %d\n", read_count);
    if (read_count == 1) {
        // First reader locks write lock
        struct sembuf op_wait_write_lock = {WRITE_LOCK, -1, SEM_UNDO};
        printf("[DEBUG] First reader locking WRITE_LOCK\n");
        semop(sem_id, &op_wait_write_lock, 1);
    }

    // Release mutex
    struct sembuf op_release_mutex = {MUTEX, 1, SEM_UNDO};
    printf("[DEBUG] Releasing MUTEX\n");
    semop(sem_id, &op_release_mutex, 1);

    // --- Critical Section: Reading value ---
    int current_value = *((int *)shm_ptr);  // Read int from shared memory
    printf("[DEBUG] Read value from shared memory: %d\n", current_value);

    // --- End Reading ---

    // Acquire mutex again to safely modify read_count
    printf("[DEBUG] Locking MUTEX again to modify read_count\n");
    semop(sem_id, &op_wait_mutex, 1);

    read_count--;
    printf("[DEBUG] Decremented read_count: %d\n", read_count);
    if (read_count == 0) {
        // Last reader releases write lock
        struct sembuf op_release_write_lock = {WRITE_LOCK, 1, SEM_UNDO};
        printf("[DEBUG] Last reader releasing WRITE_LOCK\n");
        semop(sem_id, &op_release_write_lock, 1);
    }

    printf("[DEBUG] Releasing MUTEX after modifying read_count\n");
    semop(sem_id, &op_release_mutex, 1);

    // --- Now become a Writer to modify shared memory ---

    // Acquire write lock
    struct sembuf op_wait_write_lock2 = {WRITE_LOCK, -1, SEM_UNDO};
    printf("[DEBUG] Locking WRITE_LOCK for writing\n");
    semop(sem_id, &op_wait_write_lock2, 1);

    // Modify value
    current_value += value_to_add;
    *((int *)shm_ptr) = current_value;
    printf("[DEBUG] Modified value and wrote back to shared memory: %d\n", current_value);

    // Release write lock
    struct sembuf op_release_write_lock2 = {WRITE_LOCK, 1, SEM_UNDO};
    printf("[DEBUG] Releasing WRITE_LOCK after writing\n");
    semop(sem_id, &op_release_write_lock2, 1);

    printf("[DEBUG] Finished modify_shared_int()\n\n");
}


