#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
  
    
    
    int bread_catagories_number;
    int sandwiches_number;
    int cake_flavors_number;
    int sweets_flavors_number;
    int sweet_patisseries_number;
    int savory_patisseries_number;
   
   
    int chefs_number;
    int bakers_number;
    int sallers_number;
    int suppliers_number;
    
    
    int wheat_amount_max;
    int yeast_amount_max;
    int butter_amount_max;
    int milk_amount_max;
    int sugar_amount_max; 
    int salt_amount_max;
    int sweet_items_amount_max;
    int cheese_amount_max;
    int salami_amount_max;
    
    
    int bread_catagories_price_min;  
    int sandwiches_price_min;
    int cake_flavors_price_min;
    int sweets_flavors_price_min;   
    int sweet_patisseries_price_min;
    int savory_patisseries_price_min;
    
    
    int bread_catagories_price_max;  
    int sandwiches_price_max;
    int cake_flavors_price_max;
    int sweets_flavors_price_max;   
    int sweet_patisseries_price_max;
    int savory_patisseries_price_max;
    
    
    int frustrated_customers_number;
    int complained_customers_number;
    int requested_missing_items_customers_number;
    int daily_profit_threshold;
    int max_time;
    
} Config;

int load_config(const char *filename, Config *config);

#endif
