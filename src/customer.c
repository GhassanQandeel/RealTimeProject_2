#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/config.h"
#include "../include/local.h"
#include <unistd.h>  
#include <fcntl.h>   
#include <sys/stat.h> 
#include <unistd.h>   

#include <signal.h>



int customer_id = 0; // Global variable to store customer ID
int min_time_wait_customer;
int max_time_wait_customer;
int seconds_to_wait;
#define MAX_FIFO_PATH 50
#define MAX_ORDER_LENGTH 50
char order[MAX_ORDER_LENGTH];
int mid; // Global variable to store the message queue ID
int main_order_choice;
int sub_order_choice;
time_t time_when_order_is_sent;

char *mood[4] = {"Happy", "Frustrated", "Complained", "Requested Missing Items"}; 

// Configuration
Config config;
MESSAGE msg_rcv;
MESSAGE msg_snd;

// Global arrays to store order and type information
char *main_order[6] = {"Bread", "Cake", "Sweet", "Sandwich", "Sweet Pastry", "Savory Pastry"};
char *Bread_Types[3] = {"White", "Brown", "Wholemeal"};
char *Cake_Types[3] = {"Chocolate", "Vanilla", "Red Velvet"};
char *Sweet_Types[3] = {"Chocolate", "Vanilla", "Strawberry"};
char *Sandwich_Types[3] = {"Cheese", "Salami", "Cheese & Salami"};
char *Sweet_Pastry_Types[3] = {"Croissant", "Danish", "Muffin"};
char *Savory_Pastry_Types[3] = {"Quiche", "Puff", "Samosa"};

// Function to generate a random number within a given range
int random_number(int min, int max);
void choice(int customer_id);
void request_item();
const char* generate_mood(int seconds_number , int seconds_to_wait);
void recieve_message(int mid, int customer_id);
void send_mood_to_bakery(const char* mood, int customer_id); 
void pay_price();
void sigusr1_handler(int signum) {
    printf("Received SIGUSR1 signal. Exiting...\n");
    exit(0);
}
int main(int argc, char **argv) { // Added the main function
   
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <config_file> <child_pid> <time_str> <mid>\n", argv[0]);
        return EXIT_FAILURE;
    }
    // catch sigusr1 signal
    signal(SIGUSR1, sigusr1_handler);

    const char *config_file = argv[1];
    // Load the configuration file
    if (load_config(config_file, &config) == 0) {
	    //printConfig(&config);
	   // printf("Success to load configuration.\n");
	} else {
	    fprintf(stderr, "Failed to load configuration.\n");
	}

    customer_id = atoi(argv[2]);  // Convert PID back to integer
    mid = atoi(argv[4]);  // Convert PID back to integer

    choice(customer_id);
    
    time_t creation_time = (time_t)atol(argv[3]);
    min_time_wait_customer = config.min_time_wait_customer;
    max_time_wait_customer = config.max_time_wait_customer;
    //printf("Customer %d: Min time to wait: %d seconds, Max time to wait: %d seconds\n", customer_id, min_time_wait_customer, max_time_wait_customer);
    seconds_to_wait = random_number(min_time_wait_customer, max_time_wait_customer);
    // Time of the serve 
    request_item();
    time_when_order_is_sent = time(NULL);
    recieve_message(mid, customer_id);

    return 0; // End of the program
}
int random_number(int min, int max) {
    // Seed the random number generator with the current time
    unsigned int seed;
    FILE *urandom = fopen("/dev/urandom", "rb");
    fread(&seed, sizeof(seed), 1, urandom);
    fclose(urandom);
    srand(seed);

    // Generate and return the random number
    return (rand() % (max - min + 1)) + min;
}
void choice(int customer_id){
    // Generate a random number to select an order from main_order
    main_order_choice = random_number(0, 5);

    // Use a switch statement to determine the selected order
    switch (main_order_choice) {
        case 0: // If the order is "Bread"
            int bread_choice = random_number(0, 2); // Randomly select a bread type
            sub_order_choice = bread_choice; // Assign the selected bread type to sub_order_choice
            //printf("customer%d have chosen: %s: %s\n", customer_id, Bread_Types[bread_choice], main_order[main_order_choice]); // Print the selected bread type
            strcpy(order, Bread_Types[bread_choice]); // Assign the selected bread type to order

            strncat(order, " ", MAX_ORDER_LENGTH - strlen(order) - 1); // Concatenate the order with the selected bread type
            strncat(order, main_order[main_order_choice], MAX_ORDER_LENGTH - strlen(order) - 1); // Concatenate the order with the selected bread type
            break;
        case 1: // If the order is "Cake"
            int cake_choice = random_number(0, 2); // Randomly select a cake type
            sub_order_choice = cake_choice;
            //printf("customer%d have chosen: %s: %s\n", customer_id, Cake_Types[cake_choice], main_order[main_order_choice]); // Print the selected cake type
            strcpy(order, Cake_Types[cake_choice]); // Assign the selected bread type to order
            strncat(order, " ", MAX_ORDER_LENGTH - strlen(order) - 1); // Concatenate the order with the selected bread type
            strncat(order, main_order[main_order_choice], MAX_ORDER_LENGTH - strlen(order) - 1); // Concatenate the order with the selected bread type
            break;
        case 2: // If the order is "Sweet"
            int sweet_choice = random_number(0, 2); // Randomly select a sweet type
            sub_order_choice = sweet_choice;
            //printf("customer%d have chosen: %s: %s\n", customer_id, Sweet_Types[sweet_choice], main_order[main_order_choice]); // Print the selected sweet type
            strcpy(order, Sweet_Types[sweet_choice]); // Assign the selected bread type to order
            strncat(order, " ", MAX_ORDER_LENGTH - strlen(order) - 1); // Concatenate the order with the selected bread type           
            break;
        case 3: // If the order is "Sandwich"
            int sandwich_choice = random_number(0, 2); // Randomly select a sandwich type
            sub_order_choice = sandwich_choice;
            //printf("customer%d have chosen: %s: %s\n", customer_id, Sandwich_Types[sandwich_choice], main_order[main_order_choice]); // Print the selected sandwich type
            strcpy(order, Sandwich_Types[sandwich_choice]); // Assign the selected bread type to order
            strncat(order, " ", MAX_ORDER_LENGTH - strlen(order) - 1); // Concatenate the order with the selected bread type
            strncat(order, main_order[main_order_choice], MAX_ORDER_LENGTH - strlen(order) - 1); // Concatenate the order with the selected bread type
            break;
        case 4: // If the order is "Sweet Pastry"
            int sweet_pastry_choice = random_number(0, 2); // Randomly select a sweet pastry type
            sub_order_choice = sweet_pastry_choice;
            //printf("customer%d have chosen: %s: %s\n", customer_id, Sweet_Pastry_Types[sweet_pastry_choice], main_order[main_order_choice]); // Print the selected sweet pastry type
            strcpy(order, Sweet_Pastry_Types[sweet_pastry_choice]); // Assign the selected bread type to order
            strncat(order, " ", MAX_ORDER_LENGTH - strlen(order) - 1); // Concatenate the order with the selected bread type
            strncat(order, main_order[main_order_choice], MAX_ORDER_LENGTH - strlen(order) - 1); // Concatenate the order with the selected bread type
            break;
        case 5: // If the order is "Savory Pastry"
            int savory_pastry_choice = random_number(0, 2); // Randomly select a savory pastry type
            sub_order_choice = savory_pastry_choice;
            //printf("customer%d have chosen: %s: %s\n", customer_id, Savory_Pastry_Types[savory_pastry_choice], main_order[main_order_choice]); // Print the selected savory pastry type
            strcpy(order, Savory_Pastry_Types[savory_pastry_choice]); // Assign the selected bread type to order
            strncat(order, " ", MAX_ORDER_LENGTH - strlen(order) - 1); // Concatenate the order with the selected bread type
            strncat(order, main_order[main_order_choice], MAX_ORDER_LENGTH - strlen(order) - 1); // Concatenate the order with the selected bread type
            break;
    }
}
void request_item() {
   // Send a message using mid
    int wait_time = 1 * customer_id; // Randomly select a wait time
    sleep(wait_time); // Simulate waiting time
    msg_snd.message_type = config.customer_number; // Set the message type to the 1 (Requesting item)
    msg_snd.msg_fm = customer_id; // Set the message sender to the customer ID
    snprintf(msg_snd.buffer, sizeof(msg_snd.buffer), "%s", order); // Copy the order to the message buffer
    // Send the message to the message queue
    if (msgsnd(mid, &msg_snd, sizeof(msg_snd.buffer), 0) == -1) {
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }
    printf("----------------------------------------------------------------------------------\n");
    printf("Customer %d: Order sent: %s\n", customer_id, msg_snd.buffer); // Print the sent order
    pay_price();
}
void recieve_message(int mid, int customer_id) {
    if (msgrcv(mid, &msg_rcv, sizeof(msg_rcv.buffer), customer_id, 0) == -1) {
        perror("msgrcv");
    }
    printf("----------------------------------------------------------------------------------\n");
    printf("Customer %d received message: %s, from seller%d with type:%d\n", customer_id, msg_rcv.buffer, msg_rcv.msg_fm, msg_rcv.message_type); // Print the received message
    // Get the seconds that the customer waited
    time_t now = time(NULL);
    int elapsed_seconds = (int)difftime(now, time_when_order_is_sent);
    const char* customer_mood = generate_mood(elapsed_seconds, seconds_to_wait);
    send_mood_to_bakery(customer_mood, customer_id);
    printf("Customer %d: Mood: %s\n", customer_id, customer_mood);
}
const char* generate_mood(int seconds_number, int seconds_to_wait) {
    const char* mood;
    int complained = random_number(0, 3);
    // Mood generation based on the waiting time
    if (seconds_number < (0.4 * seconds_to_wait)) {
            mood = "Happy";
    } 
    else if (seconds_number >= (0.4 * seconds_to_wait) && seconds_number < (0.8 * seconds_to_wait)) {
        if (complained != 3)
            mood = "Frustrated";
        else 
            mood = "Requested Missing Items";
    } 
    else if (seconds_number >= (0.8 * seconds_to_wait) && seconds_number < seconds_to_wait) {
        if (complained != 3)
            mood = "Complained";
        else
            mood = "Requested Missing Items";
    }
    return mood;
}
void send_mood_to_bakery(const char* mood, int customer_id){
    // Send a message using mid
    msg_snd.message_type = config.customer_number + 1; // Set the message type to the 1 (Requesting item)
    msg_snd.msg_fm = customer_id; // Set the message sender to the customer ID
    snprintf(msg_snd.buffer, sizeof(msg_snd.buffer), "%s", mood); // Copy the order to the message buffer
    // Send the message to the message queue
    if (msgsnd(mid, &msg_snd, sizeof(msg_snd.buffer), 0) == -1) {
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }
    printf("----------------------------------------------------------------------------------\n");
    printf("Customer%d mood is sent: %s\n", customer_id, msg_snd.buffer); // Print the sent order
}
void pay_price(){
 // Create a 6 *3 in array that contains prices
    int prices[6][3];
    // Fill the array with random prices
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 3; j++) {
            prices[i][j] = random_number(1, 10);
        }
    }
    int price = prices[main_order_choice][sub_order_choice];
    // convert the price to string 
    char price_str[10];
    snprintf(price_str, sizeof(price_str), "%d", price);
    msg_snd.message_type = config.customer_number + 2; // Set the message type to the 1 (Requesting item)
    msg_snd.msg_fm = customer_id; // Set the message sender to the customer ID
    snprintf(msg_snd.buffer, sizeof(msg_snd.buffer), "%s", price_str); // Copy the order to the message buffer
    // Send the message to the message queue
    if (msgsnd(mid, &msg_snd, sizeof(msg_snd.buffer), 0) == -1) {
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }
    printf("----------------------------------------------------------------------------------\n");
    printf("Customer%d payed: %s\n", customer_id, msg_snd.buffer); // Print the sent order
    
}
