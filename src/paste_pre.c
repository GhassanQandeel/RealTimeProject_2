#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <string.h>

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};
int shmid, semid;

int main(int argc, char **argv) {

    pid_t pid = getpid();

/*
    char *shm;
    struct sembuf sb;
    
    shmid=atoi(argv[2]);
    semid=atoi(argv[3]);
    
    
    // Attach shared memory segment
    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat in child");
        exit(1);
    }

    printf("Child %d starting\n", pid);

    // Lock semaphore
    sb.sem_num = 0;
    sb.sem_op = -1;  // Lock
    sb.sem_flg = 0;
    if (semop(semid, &sb, 1) < 0) {
        perror("semop lock");
        exit(1);
    }

    // Critical section - write to part of shared memory
    snprintf(shm, 1024, "Message from child %d", pid);
    printf("Child %d wrote to shared memory.\n", pid);
    sleep(1);  // Simulate work

    // Unlock semaphore
    sb.sem_op = 1;  // Unlock
    if (semop(semid, &sb, 1) < 0) {
        perror("semop unlock");
        exit(1);
    }

    // Detach shared memory
    shmdt(shm);
*/


    printf("From paste :Current Process ID: %d\n", pid);
    return 0;
}

