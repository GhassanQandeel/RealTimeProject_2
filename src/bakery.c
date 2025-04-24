#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "../include/config.h"
#include <errno.h>
#include <time.h>


Config config;


void printConfig(const Config *cfg);

int main(int argc, char **argv) {
  


	if (argc < 2) {
	       fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
	        return EXIT_FAILURE;
	    }


	if (load_config(argv[1], &config) == 0) {
	    printConfig(&config);
	} else {
	    fprintf(stderr, "Failed to load configuration.\n");
	}

    
  }


void printConfig(const Config *cfg) {
    printf("\n# --- Bakery Simulation Configuration ---\n\n");

    // Product Categories
    printf("bread_catagories_number = %d\n", cfg->bread_catagories_number);
    printf("sandwiches_number = %d\n", cfg->sandwiches_number);
    printf("cake_flavors_number = %d\n", cfg->cake_flavors_number);
    printf("sweets_flavors_number = %d\n", cfg->sweets_flavors_number);
    printf("sweet_patisseries_number = %d\n", cfg->sweet_patisseries_number);
    printf("savory_patisseries_number = %d\n", cfg->savory_patisseries_number);

    // Personnel
    printf("chefs_number = %d\n", cfg->chefs_number);
    printf("bakers_number = %d\n", cfg->bakers_number);
    printf("sallers_number = %d\n", cfg->sallers_number);
    printf("suppliers_number = %d\n", cfg->suppliers_number);

    // Ingredient Limits
    printf("wheat_amount_max = %d\n", cfg->wheat_amount_max);
    printf("yeast_amount_max = %d\n", cfg->yeast_amount_max);
    printf("butter_amount_max = %d\n", cfg->butter_amount_max);
    printf("milk_amount_max = %d\n", cfg->milk_amount_max);
    printf("sugar_amount_max = %d\n", cfg->sugar_amount_max);
    printf("salt_amount_max = %d\n", cfg->salt_amount_max);
    printf("sweet_items_amount_max = %d\n", cfg->sweet_items_amount_max);
    printf("cheese_amount_max = %d\n", cfg->cheese_amount_max);
    printf("salami_amount_max = %d\n", cfg->salami_amount_max);

    // Prices - Min
    printf("bread_catagories_price_min = %d\n", cfg->bread_catagories_price_min);
    printf("sandwiches_price_min = %d\n", cfg->sandwiches_price_min);
    printf("cake_flavors_price_min = %d\n", cfg->cake_flavors_price_min);
    printf("sweets_flavors_price_min = %d\n", cfg->sweets_flavors_price_min);
    printf("sweet_patisseries_price_min = %d\n", cfg->sweet_patisseries_price_min);
    printf("savory_patisseries_price_min = %d\n", cfg->savory_patisseries_price_min);

    // Prices - Max
    printf("bread_catagories_price_max = %d\n", cfg->bread_catagories_price_max);
    printf("sandwiches_price_max = %d\n", cfg->sandwiches_price_max);
    printf("cake_flavors_price_max = %d\n", cfg->cake_flavors_price_max);
    printf("sweets_flavors_price_max = %d\n", cfg->sweets_flavors_price_max);
    printf("sweet_patisseries_price_max = %d\n", cfg->sweet_patisseries_price_max);
    printf("savory_patisseries_price_max = %d\n", cfg->savory_patisseries_price_max);

    // Customer Feedback
    printf("frustrated_customers_number = %d\n", cfg->frustrated_customers_number);
    printf("complained_customers_number = %d\n", cfg->complained_customers_number);
    printf("requested_missing_items_customers_number = %d\n", cfg->requested_missing_items_customers_number);

    // Thresholds
    printf("daily_profit_threshold = %d\n", cfg->daily_profit_threshold);
    printf("max_time = %d\n", cfg->max_time);

    printf("\n# --- End of Configuration ---\n");
}


