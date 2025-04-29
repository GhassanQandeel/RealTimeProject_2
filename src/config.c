#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/config.h"

int load_config(const char *filename, Config *config) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Could not open configuration file");
        return -1;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n') continue;

        char key[64];
        int value;
        if (sscanf(line, "%63[^=]=%d", key, &value) == 2) {
            // Product categories
            if (strcmp(key, "bread_catagories_number") == 0) config->bread_catagories_number = value;
            else if (strcmp(key, "sandwiches_number") == 0) config->sandwiches_number = value;
            else if (strcmp(key, "cake_flavors_number") == 0) config->cake_flavors_number = value;
            else if (strcmp(key, "sweets_flavors_number") == 0) config->sweets_flavors_number = value;
            else if (strcmp(key, "sweet_patisseries_number") == 0) config->sweet_patisseries_number = value;
            else if (strcmp(key, "savory_patisseries_number") == 0) config->savory_patisseries_number = value;

            // Personnel
            else if (strcmp(key, "chefs_number") == 0) config->chefs_number = value;
            else if (strcmp(key, "bakers_number") == 0) config->bakers_number = value;
            else if (strcmp(key, "sallers_number") == 0) config->sallers_number = value;
            else if (strcmp(key, "suppliers_number") == 0) config->suppliers_number = value;
            else if (strcmp(key, "customer_number") == 0) config->customer_number = value;

            // Ingredients max
            else if (strcmp(key, "wheat_amount_max") == 0) config->wheat_amount_max = value;
            else if (strcmp(key, "yeast_amount_max") == 0) config->yeast_amount_max = value;
            else if (strcmp(key, "butter_amount_max") == 0) config->butter_amount_max = value;
            else if (strcmp(key, "milk_amount_max") == 0) config->milk_amount_max = value;
            else if (strcmp(key, "sugar_amount_max") == 0) config->sugar_amount_max = value;
            else if (strcmp(key, "salt_amount_max") == 0) config->salt_amount_max = value;
            else if (strcmp(key, "sweet_items_amount_max") == 0) config->sweet_items_amount_max = value;
            else if (strcmp(key, "cheese_amount_max") == 0) config->cheese_amount_max = value;
            else if (strcmp(key, "salami_amount_max") == 0) config->salami_amount_max = value;

            // Min prices
            else if (strcmp(key, "bread_catagories_price_min") == 0) config->bread_catagories_price_min = value;
            else if (strcmp(key, "sandwiches_price_min") == 0) config->sandwiches_price_min = value;
            else if (strcmp(key, "cake_flavors_price_min") == 0) config->cake_flavors_price_min = value;
            else if (strcmp(key, "sweets_flavors_price_min") == 0) config->sweets_flavors_price_min = value;
            else if (strcmp(key, "sweet_patisseries_price_min") == 0) config->sweet_patisseries_price_min = value;
            else if (strcmp(key, "savory_patisseries_price_min") == 0) config->savory_patisseries_price_min = value;

            // Max prices
            else if (strcmp(key, "bread_catagories_price_max") == 0) config->bread_catagories_price_max = value;
            else if (strcmp(key, "sandwiches_price_max") == 0) config->sandwiches_price_max = value;
            else if (strcmp(key, "cake_flavors_price_max") == 0) config->cake_flavors_price_max = value;
            else if (strcmp(key, "sweets_flavors_price_max") == 0) config->sweets_flavors_price_max = value;
            else if (strcmp(key, "sweet_patisseries_price_max") == 0) config->sweet_patisseries_price_max = value;
            else if (strcmp(key, "savory_patisseries_price_max") == 0) config->savory_patisseries_price_max = value;

            // Customer behavior
            else if (strcmp(key, "frustrated_customers_number_threshold") == 0) config->frustrated_customers_number_threshold = value;
            else if (strcmp(key, "complained_customers_number_threshold") == 0) config->complained_customers_number_threshold = value;
            else if (strcmp(key, "requested_missing_items_customers_number_threshold") == 0) config->requested_missing_items_customers_number_threshold = value;
            else if (strcmp(key, "min_number_of_customers") == 0) config->min_number_of_customers = value;
            else if (strcmp(key, "max_number_of_customers") == 0) config->max_number_of_customers = value;
            else if (strcmp(key, "min_time_wait_customer") == 0) config->min_time_wait_customer = value;
            else if (strcmp(key, "max_time_wait_customer") == 0) config->max_time_wait_customer = value;            

            // Simulation thresholds
            else if (strcmp(key, "daily_profit_threshold") == 0) config->daily_profit_threshold = value;
            else if (strcmp(key, "max_time") == 0) config->max_time = value*60;
        }
    }

    fclose(file);
    return 0;
}

