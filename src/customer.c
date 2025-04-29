#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/config.h"

int customer_id = 0; // Global variable to store customer ID
int min_time_wait_customer;
int max_time_wait_customer;
int seconds_to_wait;

char *mood[4] = {"Happy", "Frustrated", "Complained", "Requested Missing Items"}; 

// Configuration
Config config;

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
const char* generate_mood(int seconds_number , int seconds_to_wait);
const char* request_missing_item(int seconds_number , int seconds_to_wait);
int main(int argc, char **argv) { // Added the main function
   
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <config_file> <child_pid>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *config_file = argv[1];
    // Load the configuration file
    if (load_config(config_file, &config) == 0) {
	    //printConfig(&config);
	    printf("Success to load configuration.\n");
	} else {
	    fprintf(stderr, "Failed to load configuration.\n");
	}

    customer_id = atoi(argv[2]);  // Convert PID back to integer
    time_t creation_time = (time_t)atol(argv[3]);
    min_time_wait_customer = config.min_time_wait_customer;
    max_time_wait_customer = config.max_time_wait_customer;
    printf("Customer %d: Min time to wait: %d seconds, Max time to wait: %d seconds\n", customer_id, min_time_wait_customer, max_time_wait_customer);
    seconds_to_wait = random_number(min_time_wait_customer, max_time_wait_customer);
    // Time of the serve 
    time_t now = time(NULL);
    // Get the seconds that the customer waited
    int elapsed_seconds = (int)difftime(now, creation_time);
    const char* customer_mood = generate_mood(3, seconds_to_wait);
    printf("Customer %d: Mood: %s\n", customer_id, customer_mood);
    // Handle max time exceeded
    while(1){
        now = time(NULL);
        elapsed_seconds = (int)difftime(now, creation_time);
        if (elapsed_seconds >= seconds_to_wait) {
            printf("Customer: Maximum time (%ds) elapsed - terminating\n", seconds_to_wait);
            exit(EXIT_SUCCESS);
        }
    }

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
    int main_order_choice = random_number(0, 5);

    // Use a switch statement to determine the selected order
    switch (main_order_choice) {
        case 0: // If the order is "Bread"
            int bread_choice = random_number(0, 2); // Randomly select a bread type
            printf("customer%d have chosen: %s: %s\n", customer_id, Bread_Types[bread_choice], main_order[main_order_choice]); // Print the selected bread type
            break;
        case 1: // If the order is "Cake"
            int cake_choice = random_number(0, 2); // Randomly select a cake type
            printf("customer%d have chosen: %s: %s\n", customer_id, Cake_Types[cake_choice], main_order[main_order_choice]); // Print the selected cake type
            break;
        case 2: // If the order is "Sweet"
            int sweet_choice = random_number(0, 2); // Randomly select a sweet type
            printf("customer%d have chosen: %s: %s\n", customer_id, Sweet_Types[sweet_choice], main_order[main_order_choice]); // Print the selected sweet type
            break;
        case 3: // If the order is "Sandwich"
            int sandwich_choice = random_number(0, 2); // Randomly select a sandwich type
            printf("customer%d have chosen: %s: %s\n", customer_id, Sandwich_Types[sandwich_choice], main_order[main_order_choice]); // Print the selected sandwich type
            break;
        case 4: // If the order is "Sweet Pastry"
            int sweet_pastry_choice = random_number(0, 2); // Randomly select a sweet pastry type
            printf("customer%d have chosen: %s: %s\n", customer_id, Sweet_Pastry_Types[sweet_pastry_choice], main_order[main_order_choice]); // Print the selected sweet pastry type
            break;
        case 5: // If the order is "Savory Pastry"
            int savory_pastry_choice = random_number(0, 2); // Randomly select a savory pastry type
            printf("customer%d have chosen: %s: %s\n", customer_id, Savory_Pastry_Types[savory_pastry_choice], main_order[main_order_choice]); // Print the selected savory pastry type
            break;
    }
}
const char* generate_mood(int seconds_number, int seconds_to_wait) {
    const char* mood;
    int complained = random_number(0, 3);
    // Mood generation based on the waiting time
    if (seconds_number < (0.4 * seconds_to_wait)) {
            mood = "Happy";
    } 
    else if (seconds_number >= (0.4 * seconds_to_wait) && seconds_number < (0.8 * seconds_to_wait)) {
            mood = "Frustrated";
    } 
    else if (seconds_number >= (0.8 * seconds_to_wait) && seconds_number < seconds_to_wait) {
            mood = "Complained";
    }
    return mood;
}
const char* request_missing_item(int seconds_number , int seconds_to_wait){
    int requested_missing_items = random_number(0, 3);
    if (requested_missing_items == 3) {
        return "Requested Missing Items";
    } else {
        return "No Request";
    }
}
