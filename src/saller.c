#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "../include/local.h"
#include "../include/config.h"

int mid;
MESSAGE msg_rcv;
MESSAGE msg_snd;
Config config;
void recieve_message(int mid, int saller_id); 
void send_item();
void sigusr1_handler(int signum) {
    printf("Received SIGUSR1 signal. Exiting...\n");
    exit(0);
}
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
    // use the recieve message function to get the message from the queue
    // Recieve message from the queue
    while(1){
    recieve_message(mid, seller_id);
    sleep(5); // Processing time
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

