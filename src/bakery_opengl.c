/*This code is a simulation of a bakery management system using OpenGL and GLUT. It visualizes the operations of a bakery, including preparation, sales, and customer interactions. Here's a breakdown of the code and how it plots the simulation:

### **Code Overview**
1. **Shared Memory and Semaphores**:
    - The code uses shared memory and semaphores to manage resources and products in the bakery. Shared memory is used to store quantities of ingredients and products, while semaphores ensure synchronization between processes.

2. **Bakery State**:
    - The `BakeryState` structure holds the state of the bakery, including:
      - Number of staff (chefs, bakers, sellers).
      - Number of customers and their states (entering, browsing, leaving).
      - Products and their freshness.
      - Resources (e.g., wheat, sugar, milk).
      - Profit and customer satisfaction metrics.

3. **OpenGL Visualization**:
    - The bakery is divided into two areas:
      - **Preparation Area**: Contains preparation and baking tables for chefs and bakers.
      - **Sales Area**: Contains display tables for products and customers interacting with sellers.

4. **Simulation Logic**:
    - Customers are added periodically and move through different states (entering, browsing, leaving).
    - Products are added to display tables based on shared memory values.
    - Staff members (chefs, bakers, sellers) are visualized at their respective stations.
    - Resources are updated and displayed as progress bars.

5. **Termination Conditions**:
    - The simulation stops if certain conditions are met, such as too many frustrated customers or a high profit.

---

### **How It Plots the Simulation**
1. **Initialization**:
    - The `init()` function sets up the OpenGL environment with a 2D orthographic projection and a light gray background.

2. **Main Display Function**:
    - The `display()` function is the core of the visualization. It:
      - Draws dividers between the preparation and sales areas.
      - Calls helper functions to draw tables, staff, customers, products, resources, and status.

3. **Drawing Functions**:
    - **Tables**: `drawTable()` draws preparation, baking, and display tables with labels.
    - **Staff**: `drawStaffMember()` visualizes chefs, bakers, and sellers with different colors and accessories (e.g., chef hats).
    - **Products**: `drawProduct()` visualizes products (e.g., bread, cakes) as colored rectangles with freshness indicators.
    - **Customers**: `drawCustomer()` visualizes customers with different moods (neutral, happy, frustrated, angry) using colors and facial expressions.
    - **Resources**: `drawResources()` displays progress bars for resources like wheat, sugar, and milk.
    - **Status**: `drawStatus()` shows profit, customer metrics, and simulation status.

4. **Simulation Updates**:
    - The `update()` function is called periodically (every 16ms) to:
      - Add new customers and products.
      - Update customer movements and interactions.
      - Update product freshness and resource levels.
      - Check termination conditions.

5. **Interaction with Shared Memory**:
    - Functions like `modify_shared_int()` read and update shared memory values for resources and products, ensuring synchronization with semaphores.

---

### **Visualization Layout**
- **Preparation Area**:
  - Contains three preparation tables (e.g., Dough Prep, Baking Station, Decoration) and three baking tables (e.g., Bread Baking, Cake Baking).
  - Staff members (chefs and bakers) are positioned near these tables.

- **Sales Area**:
  - Contains six display tables for different product types (e.g., Paste, Cakes, Sandwiches, Sweets).
  - Customers move around these tables and interact with sellers.

- **Resources and Status**:
  - Resource levels are displayed as progress bars on the top-left.
  - Profit, customer metrics, and simulation status are displayed on the top-right.

---

### **Key Features**
- **Dynamic Visualization**:
  - Customers and products are added dynamically, and their states are updated in real-time.
  - Freshness of products is visualized with color-coded bars.

- **Synchronization**:
  - Shared memory and semaphores ensure that the simulation reflects the current state of resources and products.

- **Termination Conditions**:
  - The simulation stops when certain conditions are met, such as high profit or too many frustrated customers.

This code provides a comprehensive simulation of a bakery's operations, visualized in a 2D OpenGL environment.
*/


#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "../include/config.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>


/* Configuration */
Config config;
char config_file_name[30];
#define MUTEX 0
#define READ_COUNT 1
#define WRITE_LOCK 2

/* Shared memory and semaphore IDs */
int shm_wheat_id, sem_wheat_id;
int shm_yeast_id, sem_yeast_id;
int shm_butter_id, sem_butter_id;
int shm_milk_id, sem_milk_id;
int shm_sugar_id, sem_sugar_id;
int shm_salt_id, sem_salt_id;
int shm_sweet_items_id, sem_sweet_items_id;
int shm_cheese_id, sem_cheese_id;
int shm_salami_id, sem_salami_id;

/* Chef production */
int shm_paste_id, sem_paste_id;
int shm_cake_paste_id, sem_cake_paste_id;
int shm_sandwiches_id, sem_sandwiches_id;
int shm_sweets_paste_id, sem_sweets_paste_id;
int shm_sweet_patiss_paste_id, sem_sweet_patiss_paste_id;
int shm_savory_patiss_paste_id, sem_savory_patiss_paste_id;

/* Pointers to shared memory */
char *shm_wheat_ptr, *shm_yeast_ptr, *shm_butter_ptr, *shm_milk_ptr;
char *shm_sugar_ptr, *shm_salt_ptr, *shm_sweet_items_ptr, *shm_cheese_ptr, *shm_salami_ptr;
char *shm_paste_ptr, *shm_cake_paste_ptr, *shm_sandwiches_ptr;
char *shm_sweets_paste_ptr, *shm_sweet_patiss_paste_ptr, *shm_savory_patiss_paste_ptr;



// Global variables for Bread Categories
int *bread_catagories_shm_id;
int *bread_catagories_sem_id;
char **bread_catagories_shm_ptr;

// Global variables for Sandwiches
int *sandwiches_shm_id;
int *sandwiches_sem_id;
char **sandwiches_shm_ptr;

// Global variables for Cake Flavors
int *cake_flavors_shm_id;
int *cake_flavors_sem_id;
char **cake_flavors_shm_ptr;

// Global variables for Sweets Flavors
int *sweets_flavors_shm_id;
int *sweets_flavors_sem_id;
char **sweets_flavors_shm_ptr;

// Global variables for Sweet Patisseries
int *sweet_patisseries_shm_id;
int *sweet_patisseries_sem_id;
char **sweet_patisseries_shm_ptr;

// Global variables for Savory Patisseries
int *savory_patisseries_shm_id;
int *savory_patisseries_sem_id;
char **savory_patisseries_shm_ptr;




/* Constants */
#define MAX_CUSTOMERS 20
#define MAX_PRODUCTS 50
#define MAX_STAFF 10

/* Product and staff types */
enum ProductType { BREAD, CAKE, SANDWICH, SWEET, SWEET_PATISS, SAVORY_PATISS, PRODUCT_TYPES };
enum StaffType { CHEF, BAKER, SELLER };

/* Structures */
typedef struct {
    float x, y;
    int active;
    int state; // 0=entering, 1=browsing, 2=leaving
    float progress;
    int will_complain;
    int will_buy;
    int mood; // 0=neutral, 1=happy, 2=frustrated, 3=angry
} Customer;

typedef struct {
    float x, y;
    int type;
    int freshness;
} Product;

typedef struct {
    float x, y;
    int type;
    int busy;
    float progress;
} Staff;

typedef struct {
    float x, y;
    float width, height;
    const char* label;
} Table;

/* Bakery state */
typedef struct {
    int bread_categories;
    int sandwich_types;
    int cake_flavors;
    int sweets_flavors;
    int patisserie_types;
    int chefs;
    int bakers;
    int sellers;
    int supply_employees;
    float resources[9]; // wheat, yeast, butter, milk, sugar, salt, sweet_items, cheese, salami
    int customers_served;
    int customers_frustrated;
    int customers_complained;
    int customers_missing_items;
    float profit;
    int simulation_running;
    Customer customers[MAX_CUSTOMERS];
    Product products[MAX_PRODUCTS];
    Staff staff[MAX_STAFF];
    float time_since_last_customer;
    float time_since_last_product;
} BakeryState;

BakeryState bakery;
/* Table definitions */
Table prep_tables[5] = {
    {150, 600, 150, 60, "Paste"},
    {350, 600, 150, 60, "Cake Paste"},
    {550, 600, 150, 60, "Sweet Paste"},
    {750, 600, 150, 60, "Sweet Patiss Paste"},
    {950, 600, 150, 60, "Savory Patiss Paste"}
};

Table baking_tables[3] = {
    {350, 400, 150, 60, "Bread Baking"},
    {550, 400, 150, 60, "Cake Sweets Baking"}, // Shifted under Sweet Paste table
    {750, 400, 150, 60, "Patiss Baking"}
};
Table display_tables[6] = {
    {10, 100, 200, 80, "Bread"},
    {250, 100, 200, 80, "Cakes"},
    {490, 100, 200, 80, "Sandwiches"},
    {730, 100, 200, 80, "Sweets"},
    {970, 100, 200, 80, "Sweet Patiss"},
    {1210, 100, 200, 80, "Savory Patiss"}
};


void drawPreparationProgress();
void drawFinalProducts();
/* Function prototypes */
void init();
void display();
void reshape(int w, int h);
void update(int value);
void drawDividers();
void drawPreparationArea();
void drawSalesArea();
void drawResources();
void drawStaff();
void drawCustomers();
void drawProductTables();
void drawProducts();
void drawStatus();
void drawText(float x, float y, const char* string);
void drawCustomer(float x, float y, int mood);
void drawStaffMember(float x, float y, int type, int busy);
void drawProduct(float x, float y, int type);
void addCustomer();
void addProduct();
void updateCustomer(Customer *c);
void updateProducts();
void updateStaff();
void drawTable(float x, float y, float width, float height, const char* label);

void parse_ids(const char *buffer);
void attach_shm_basic_items();
void deattach_shm(int shmid, char *ptr);
void deattach_all_shm();

void decode_shm_sem_message(const char* message, int* shm_ids, int* sem_ids, int max_count);
void attach_shm_segments(int* shm_ids, char** shm_ptrs, int count);
void detach_shm_segments(char** shm_ptrs, int count);

int main(int argc, char** argv) {
    // Verify command line arguments
    if (argc < 15) {
        fprintf(stderr, "Usage: %s <config_file> <basic_items> <paste> <cake> <sandwiches> <sweets> <sweet_patiss> <savory_patiss> <bread_shm> <sandwiches_shm> <cake_shm> <sweets_shm> <sweet_patiss_shm> <savory_patiss_shm>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Load configuration
    strcpy(config_file_name, argv[1]);
    if (load_config(config_file_name, &config) != 0) {
        fprintf(stderr, "Failed to load configuration\n");
        return EXIT_FAILURE;
    }

    // Parse shared memory and semaphore IDs
    parse_ids(argv[2]);  // Basic items
    
    // Chef production items
    sscanf(argv[3], "%d %d", &shm_paste_id, &sem_paste_id);
    sscanf(argv[4], "%d %d", &shm_cake_paste_id, &sem_cake_paste_id);
    sscanf(argv[5], "%d %d", &shm_sandwiches_id, &sem_sandwiches_id);
    sscanf(argv[6], "%d %d", &shm_sweets_paste_id, &sem_sweets_paste_id);
    sscanf(argv[7], "%d %d", &shm_sweet_patiss_paste_id, &sem_sweet_patiss_paste_id);
    sscanf(argv[8], "%d %d", &shm_savory_patiss_paste_id, &sem_savory_patiss_paste_id);
   // Allocate memory for Bread Categories
    bread_catagories_shm_id = malloc(config.bread_catagories_number * sizeof(int));
    bread_catagories_sem_id = malloc(config.bread_catagories_number * sizeof(int));
    bread_catagories_shm_ptr = malloc(config.bread_catagories_number * sizeof(char *));

    // Allocate memory for Sandwiches
    sandwiches_shm_id = malloc(config.sandwiches_number * sizeof(int));
    sandwiches_sem_id = malloc(config.sandwiches_number * sizeof(int));
    sandwiches_shm_ptr = malloc(config.sandwiches_number * sizeof(char *));

    // Allocate memory for Cake Flavors
    cake_flavors_shm_id = malloc(config.cake_flavors_number * sizeof(int));
    cake_flavors_sem_id = malloc(config.cake_flavors_number * sizeof(int));
    cake_flavors_shm_ptr = malloc(config.cake_flavors_number * sizeof(char *));

    // Allocate memory for Sweets Flavors
    sweets_flavors_shm_id = malloc(config.sweets_flavors_number * sizeof(int));
    sweets_flavors_sem_id = malloc(config.sweets_flavors_number * sizeof(int));
    sweets_flavors_shm_ptr = malloc(config.sweets_flavors_number * sizeof(char *));

    // Allocate memory for Sweet Patisseries
    sweet_patisseries_shm_id = malloc(config.sweet_patisseries_number * sizeof(int));
    sweet_patisseries_sem_id = malloc(config.sweet_patisseries_number * sizeof(int));
    sweet_patisseries_shm_ptr = malloc(config.sweet_patisseries_number * sizeof(char *));

    // Allocate memory for Savory Patisseries
    savory_patisseries_shm_id = malloc(config.savory_patisseries_number * sizeof(int));
    savory_patisseries_sem_id = malloc(config.savory_patisseries_number * sizeof(int));
    savory_patisseries_shm_ptr = malloc(config.savory_patisseries_number * sizeof(char *));
    decode_shm_sem_message(argv[9], bread_catagories_shm_id, bread_catagories_sem_id, config.bread_catagories_number);
    decode_shm_sem_message(argv[10], sandwiches_shm_id, sandwiches_sem_id, config.sandwiches_number);
    decode_shm_sem_message(argv[11], cake_flavors_shm_id, cake_flavors_sem_id, config.cake_flavors_number);
    decode_shm_sem_message(argv[12], sweets_flavors_shm_id, sweets_flavors_sem_id, config.sweets_flavors_number);
    decode_shm_sem_message(argv[13], sweet_patisseries_shm_id, sweet_patisseries_sem_id, config.sweet_patisseries_number);
    decode_shm_sem_message(argv[14], savory_patisseries_shm_id, savory_patisseries_sem_id, config.savory_patisseries_number);




    // Initialize bakery state
    memset(&bakery, 0, sizeof(BakeryState));
    bakery.simulation_running = 1;
    bakery.chefs = config.chefs_number;
    bakery.bakers = config.bakers_number;
    bakery.sellers = config.sallers_number;
    bakery.bread_categories = config.bread_catagories_number;
    bakery.sandwich_types = config.sandwiches_number;
    bakery.cake_flavors = config.cake_flavors_number;
    bakery.sweets_flavors = config.sweets_flavors_number;
    bakery.patisserie_types = config.sweet_patisseries_number + config.savory_patisseries_number;

    // Attach to shared memory segments
    attach_shm_basic_items();
    attach_shm_segments(bread_catagories_shm_id, bread_catagories_shm_ptr, config.bread_catagories_number);
    attach_shm_segments(sandwiches_shm_id, sandwiches_shm_ptr, config.sandwiches_number);
    attach_shm_segments(cake_flavors_shm_id, cake_flavors_shm_ptr, config.cake_flavors_number);
    attach_shm_segments(sweets_flavors_shm_id, sweets_flavors_shm_ptr, config.sweets_flavors_number);
    attach_shm_segments(sweet_patisseries_shm_id, sweet_patisseries_shm_ptr, config.sweet_patisseries_number);
    attach_shm_segments(savory_patisseries_shm_id, savory_patisseries_shm_ptr, config.savory_patisseries_number);
 

    // Initialize staff positions
    for (int i = 0; i < bakery.chefs; i++) {
        bakery.staff[i].type = CHEF;
        bakery.staff[i].x = prep_tables[0].x + prep_tables[0].width/2 + (i-1)*40;
        bakery.staff[i].y = prep_tables[0].y - 30;
    }
    for (int i = 0; i < bakery.bakers; i++) {
        bakery.staff[bakery.chefs+i].type = BAKER;
        bakery.staff[bakery.chefs+i].x = prep_tables[1].x + prep_tables[1].width/2 + (i-0.5)*40;
        bakery.staff[bakery.chefs+i].y = prep_tables[1].y - 30;
    }
    for (int i = 0; i < bakery.sellers; i++) {
        bakery.staff[bakery.chefs+bakery.bakers+i].type = SELLER;
        bakery.staff[bakery.chefs+bakery.bakers+i].x = 600 + (i-0.5)*80;
        bakery.staff[bakery.chefs+bakery.bakers+i].y = 350;
    }

    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1200, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Bakery Management Simulation");

    // Set callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(16, update, 0);

    // Initialize OpenGL
    init();

    // Main loop
    glutMainLoop();

    // Cleanup - detach from shared memory
    deattach_all_shm();
    detach_shm_segments(bread_catagories_shm_ptr, config.bread_catagories_number);
    detach_shm_segments(sandwiches_shm_ptr, config.sandwiches_number);
    detach_shm_segments(cake_flavors_shm_ptr, config.cake_flavors_number);
    detach_shm_segments(sweets_flavors_shm_ptr, config.sweets_flavors_number);
    detach_shm_segments(sweet_patisseries_shm_ptr, config.sweet_patisseries_number);
    detach_shm_segments(savory_patisseries_shm_ptr, config.savory_patisseries_number);
    // Free memory for Bread Categories
    free(bread_catagories_shm_id);
    free(bread_catagories_sem_id);
    free(bread_catagories_shm_ptr);

    // Free memory for Sandwiches
    free(sandwiches_shm_id);
    free(sandwiches_sem_id);
    free(sandwiches_shm_ptr);

    // Free memory for Cake Flavors
    free(cake_flavors_shm_id);
    free(cake_flavors_sem_id);
    free(cake_flavors_shm_ptr);

    // Free memory for Sweets Flavors
    free(sweets_flavors_shm_id);
    free(sweets_flavors_sem_id);
    free(sweets_flavors_shm_ptr);

    // Free memory for Sweet Patisseries
    free(sweet_patisseries_shm_id);
    free(sweet_patisseries_sem_id);
    free(sweet_patisseries_shm_ptr);

    // Free memory for Savory Patisseries
    free(savory_patisseries_shm_id);
    free(savory_patisseries_sem_id);
    free(savory_patisseries_shm_ptr);

    return EXIT_SUCCESS;
}
int modify_shared_int(int sem_id, char *shm_ptr, int value_to_add) {
    struct sembuf op_wait_mutex = {MUTEX, -1, SEM_UNDO};
    semop(sem_id, &op_wait_mutex, 1);

    static int read_count = 0;
    read_count++;

    if (read_count == 1) {
        struct sembuf op_wait_write_lock = {WRITE_LOCK, -1, SEM_UNDO};
        semop(sem_id, &op_wait_write_lock, 1);
    }

    struct sembuf op_release_mutex = {MUTEX, 1, SEM_UNDO};
    semop(sem_id, &op_release_mutex, 1);

    int current_value = *((int *)shm_ptr);

    semop(sem_id, &op_wait_mutex, 1);
    read_count--;

    if (read_count == 0) {
        struct sembuf op_release_write_lock = {WRITE_LOCK, 1, SEM_UNDO};
        semop(sem_id, &op_release_write_lock, 1);
    }

    semop(sem_id, &op_release_mutex, 1);
    return current_value;
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

void decode_shm_sem_message(const char* message, int* shm_ids, int* sem_ids, int max_count) {
    if (message == NULL || message[0] == '\0') {
        return;
    }

    const char* ptr = message;
    int count = 0;
    
    while (*ptr != '\0' && count < max_count) {
        // Parse shm_id
        shm_ids[count] = atoi(ptr);
        // Move past the number
        while (*ptr != '\0' && *ptr != ' ') ptr++;
        if (*ptr == ' ') ptr++;  // skip space
        
        // Parse sem_id
        sem_ids[count] = atoi(ptr);
        // Move past the number
        while (*ptr != '\0' && *ptr != ' ') ptr++;
        if (*ptr == ' ') ptr++;  // skip space
        
        count++;
    }
    
   
}


void attach_shm_segments(int* shm_ids, char** shm_ptrs, int count) {
    for (int i = 0; i < count; i++) {
        shm_ptrs[i] = (char *)shmat(shm_ids[i], NULL, 0);
        if (shm_ptrs[i] == (char *)-1) {
            perror("shmat failed");
            
            // Cleanup any already attached segments
            for (int j = 0; j < i; j++) {
                shmdt(shm_ptrs[j]);
            }
            
           
        }
    }
}

void detach_shm_segments(char** shm_ptrs, int count) {
    for (int i = 0; i < count; i++) {
        if (shm_ptrs[i] != NULL && shm_ptrs[i] != (char *)-1) {
            if (shmdt(shm_ptrs[i]) == -1) {
                perror("shmdt failed");
                // Continue trying to detach others even if one fails
            }
        }
    }
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

    // Paste
    shm_paste_ptr = (char *)shmat(shm_paste_id, NULL, 0);
    if (shm_paste_ptr == (char *)-1) {
        perror("shmat paste failed");
        exit(1);
    }

    // Cake Paste
    shm_cake_paste_ptr = (char *)shmat(shm_cake_paste_id, NULL, 0);
    if (shm_cake_paste_ptr == (char *)-1) {
        perror("shmat cake paste failed");
        exit(1);
    }

    // Sandwiches
    shm_sandwiches_ptr = (char *)shmat(shm_sandwiches_id, NULL, 0);
    if (shm_sandwiches_ptr == (char *)-1) {
        perror("shmat sandwiches failed");
        exit(1);
    }

    // Sweets Paste
    shm_sweets_paste_ptr = (char *)shmat(shm_sweets_paste_id, NULL, 0);
    if (shm_sweets_paste_ptr == (char *)-1) {
        perror("shmat sweets paste failed");
        exit(1);
    }

    // Sweet Patisserie Paste
    shm_sweet_patiss_paste_ptr = (char *)shmat(shm_sweet_patiss_paste_id, NULL, 0);
    if (shm_sweet_patiss_paste_ptr == (char *)-1) {
        perror("shmat sweet patisserie paste failed");
        exit(1);
    }

    // Savory Patisserie Paste
    shm_savory_patiss_paste_ptr = (char *)shmat(shm_savory_patiss_paste_id, NULL, 0);
    if (shm_savory_patiss_paste_ptr == (char *)-1) {
        perror("shmat savory patisserie paste failed");
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
    deattach_shm(shm_paste_id, shm_paste_ptr);
    deattach_shm(shm_cake_paste_id, shm_cake_paste_ptr);
    deattach_shm(shm_sandwiches_id, shm_sandwiches_ptr);
    deattach_shm(shm_sweets_paste_id, shm_sweets_paste_ptr);
    deattach_shm(shm_sweet_patiss_paste_id, shm_sweet_patiss_paste_ptr);
    deattach_shm(shm_savory_patiss_paste_id, shm_savory_patiss_paste_ptr);
}





void init() {
    glClearColor(0.96f, 0.96f, 0.94f, 1.0f); // Very light gray background
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 1200, 0, 800);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    drawDividers();
    drawPreparationArea();
    drawPreparationProgress();
    drawSalesArea();
    drawFinalProducts();
    drawResources();
    drawStatus();
    
    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
}

void drawDividers() {
    // Main divider between preparation and sales areas
    glColor3f(0.5f, 0.5f, 0.5f);
    glLineWidth(2.0);
    glBegin(GL_LINES);
    glVertex2f(50, 330); // Shifted further down
    glVertex2f(1150, 330); // Shifted further down
    glEnd();
    
    // Label for areas
    glColor3f(0.0f, 0.0f, 0.8f); // Cold blue color for text
    drawText(100, 340, "PREPARATION AREA"); // Above the line
    drawText(100, 300, "SALES AREA"); // Below the line
}

void drawPreparationArea() {
    // Draw preparation tables
    for (int i = 0; i < 5; i++) {
        drawTable(prep_tables[i].x, prep_tables[i].y, 
                 prep_tables[i].width, prep_tables[i].height, 
                 prep_tables[i].label);
    }
    
    // Draw baking tables
    for (int i = 0; i < 3; i++) {
        drawTable(baking_tables[i].x, baking_tables[i].y,
                 baking_tables[i].width, baking_tables[i].height,
                 baking_tables[i].label);
    }
    
    // Draw chefs
    for (int i = 0; i < bakery.chefs; i++) {
        int table_index = i % 5; // Cycle through the tables
        drawStaffMember(prep_tables[table_index].x + prep_tables[table_index].width / 2, 
                        prep_tables[table_index].y - 40, 
                        bakery.staff[i].type, bakery.staff[i].busy);
    }
    // Draw bakers
    for (int i = bakery.chefs; i < bakery.chefs + bakery.bakers; i++) {
        int table_index = (i - bakery.chefs) % 3; // Cycle through the baking tables
        drawStaffMember(baking_tables[table_index].x + baking_tables[table_index].width / 2, 
                        baking_tables[table_index].y - 40, 
                        bakery.staff[i].type, bakery.staff[i].busy);
    }

}

void drawSalesArea() {
    // Draw product display tables
    for (int i = 0; i < 6; i++) {
        drawTable(display_tables[i].x, display_tables[i].y,
                 display_tables[i].width, display_tables[i].height,
                 display_tables[i].label);
    }
    
    
    
    // Draw customers
    drawCustomers();
    
    // Draw sellers
    for (int i = bakery.chefs + bakery.bakers; 
         i < bakery.chefs + bakery.bakers + bakery.sellers; i++) {
        int table_index = (i - bakery.chefs - bakery.bakers) % 6; // Cycle through the display tables
        drawStaffMember(display_tables[table_index].x +60, // Right upper corner of the table
                        display_tables[table_index].y+1000, 
                        bakery.staff[i].type, bakery.staff[i].busy);
    }
}

void drawTable(float x, float y, float width, float height, const char* label) {
    // Table top
    glColor3f(0.82f, 0.70f, 0.55f); // Light wood color
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
    
    // Table edge
    glColor3f(0.62f, 0.50f, 0.35f); // Darker wood
    glLineWidth(2.0);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
    
    // Table legs
    glColor3f(0.52f, 0.40f, 0.25f); // Dark wood
    float leg_width = width * 0.05f;
    float leg_height = height * 0.7f;
    
    // Front legs
    glBegin(GL_QUADS);
    glVertex2f(x + width*0.1f, y - leg_height);
    glVertex2f(x + width*0.1f + leg_width, y - leg_height);
    glVertex2f(x + width*0.1f + leg_width, y);
    glVertex2f(x + width*0.1f, y);
    
    glVertex2f(x + width*0.9f - leg_width, y - leg_height);
    glVertex2f(x + width*0.9f, y - leg_height);
    glVertex2f(x + width*0.9f, y);
    glVertex2f(x + width*0.9f - leg_width, y);
    glEnd();
    
    // Table label
    glColor3f(0.2f, 0.2f, 0.2f);
    drawText(x + width/2 - 40, y + height + 15, label);
}

void drawProductTables() {
    for (int i = 0; i < PRODUCT_TYPES; i++) {
        drawTable(display_tables[i].x, display_tables[i].y,
                 display_tables[i].width, display_tables[i].height,
                 display_tables[i].label);
    }
}

void drawStaffMember(float x, float y, int type, int busy) {
    // Adjust position based on type
    if (type == CHEF) {
        y = prep_tables[0].y - 40; // Position chefs under preparation tables
    } else if (type == BAKER) {
        y = baking_tables[0].y - 40; // Position bakers under baking tables
    } else if (type == SELLER) {
        y = display_tables[0].y + 60; // Position sellers above display tables
    }

    // Calculate spacing for multiple staff members
    int staff_count = (type == CHEF) ? bakery.chefs : (type == BAKER) ? bakery.bakers : bakery.sellers;
    float spacing = 30.0f; // Space between staff members
    x += (busy - (staff_count / 2.0f)) * spacing;

    // Body
    switch(type) {
        case CHEF:
            glColor3f(0.9f, 0.2f, 0.2f); // Red for chefs
            break;
        case BAKER:
            glColor3f(0.2f, 0.2f, 0.8f); // Blue for bakers
            break;
        case SELLER:
            glColor3f(0.2f, 0.8f, 0.2f); // Green for sellers
            break;
    }
    
    // Body
    glBegin(GL_QUADS);
    glVertex2f(x - 12, y);
    glVertex2f(x + 12, y);
    glVertex2f(x + 12, y - 30);
    glVertex2f(x - 12, y - 30);
    glEnd();
    
    // Head
    glColor3f(1.0f, 0.85f, 0.7f); // Skin tone
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i += 20) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(x + cos(angle) * 10, y + 15 + sin(angle) * 10);
    }
    glEnd();
    
    // Hat/accessory based on type
    glColor3f(1.0f, 1.0f, 1.0f); // White for all hats
    if (type == CHEF) {
        // Chef's hat
        glBegin(GL_POLYGON);
        glVertex2f(x - 12, y + 20);
        glVertex2f(x + 12, y + 20);
        glVertex2f(x + 8, y + 35);
        glVertex2f(x - 8, y + 35);
        glEnd();
    } else if (type == BAKER) {
        // Baker's cap
        glBegin(GL_QUADS);
        glVertex2f(x - 10, y + 15);
        glVertex2f(x + 10, y + 15);
        glVertex2f(x + 10, y + 25);
        glVertex2f(x - 10, y + 25);
        glEnd();
    } else {
        // Seller's apron
        glBegin(GL_TRIANGLES);
        glVertex2f(x - 12, y);
        glVertex2f(x + 12, y);
        glVertex2f(x, y - 15);
        glEnd();
    }
    
    // Activity indicator
    if (busy) {
        glColor3f(1.0f, 0.8f, 0.0f);
        glPointSize(6.0);
        glBegin(GL_POINTS);
        glVertex2f(x + 15, y + 5);
        glEnd();
    }
}
void drawProduct(float x, float y, int type) {
    // Set color based on product type
    switch (type) {
        case BREAD:
            glColor3f(0.82f, 0.70f, 0.55f); // Light brown for bread
            break;
        case CAKE:
            glColor3f(0.96f, 0.76f, 0.76f); // Pink for cake
            break;
        case SANDWICH:
            glColor3f(0.94f, 0.90f, 0.55f); // Yellow for sandwich
            break;
        case SWEET:
            glColor3f(0.76f, 0.96f, 0.76f); // Light green for sweets
            break;
        case SWEET_PATISS:
            glColor3f(0.96f, 0.76f, 0.96f); // Light purple for sweet patisserie
            break;
        case SAVORY_PATISS:
            glColor3f(0.76f, 0.76f, 0.96f); // Light blue for savory patisserie
            break;
        default:
            glColor3f(0.8f, 0.8f, 0.8f); // Gray for unknown
            break;
    }

    // Draw the product (a simple rectangle)
    glBegin(GL_QUADS);
    glVertex2f(x - 8, y - 8);
    glVertex2f(x + 8, y - 8);
    glVertex2f(x + 8, y + 8);
    glVertex2f(x - 8, y + 8);
    glEnd();

    // Draw a border around the product
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x - 8, y - 8);
    glVertex2f(x + 8, y - 8);
    glVertex2f(x + 8, y + 8);
    glVertex2f(x - 8, y + 8);
    glEnd();

    // Optional: Add a small decoration based on type
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    switch (type) {
        case BREAD:
            glColor3f(0.62f, 0.50f, 0.35f); // Darker brown for bread texture
            glVertex2f(x - 4, y);
            glVertex2f(x + 4, y);
            break;
        case CAKE:
            glColor3f(0.96f, 0.86f, 0.86f); // Lighter pink for cake decoration
            glVertex2f(x, y + 4);
            glVertex2f(x, y - 4);
            break;
        case SANDWICH:
            glColor3f(0.8f, 0.6f, 0.2f); // Darker yellow for sandwich filling
            glVertex2f(x - 2, y);
            glVertex2f(x + 2, y);
            break;
        case SWEET:
            glColor3f(0.5f, 0.8f, 0.5f); // Darker green for sweet details
            glVertex2f(x - 3, y - 3);
            glVertex2f(x + 3, y + 3);
            break;
        case SWEET_PATISS:
            glColor3f(0.8f, 0.5f, 0.8f); // Darker purple for patisserie details
            glVertex2f(x - 3, y + 3);
            glVertex2f(x + 3, y - 3);
            break;
        case SAVORY_PATISS:
            glColor3f(0.5f, 0.5f, 0.8f); // Darker blue for patisserie details
            glVertex2f(x, y - 5);
            glVertex2f(x, y + 5);
            break;
    }
    glEnd();
}

// [Rest of the functions (drawCustomer, drawProduct, update, addCustomer, 
// addProduct, updateCustomer, updateProducts, updateStaff, drawResources, 
// drawStatus, drawText) remain similar to previous versions but adjusted 
// for the new layout]




void update(int value) {
    if (bakery.simulation_running) {
        // Update timers
        bakery.time_since_last_customer += 0.016f;
        bakery.time_since_last_product += 0.016f;
        
        // Add new customers and products
        if (bakery.time_since_last_customer > 2.0f && rand() % 30 == 0) {
            addCustomer();
            bakery.time_since_last_customer = 0;
        }
        
        if (bakery.time_since_last_product > 1.0f && rand() % 20 == 0) {
            addProduct();
            bakery.time_since_last_product = 0;
        }
        
        // Update all customers
        for (int i = 0; i < MAX_CUSTOMERS; i++) {
            if (bakery.customers[i].active) {
                updateCustomer(&bakery.customers[i]);
            }
        }
        
        updateProducts();
        
        // Random profit increase
        if (rand() % 30 == 0) {
            bakery.profit += (rand() % 100) / 10.0f;
            bakery.customers_served++;
        }
        
        // Update resources
        for (int i = 0; i < 8; i++) {
            bakery.resources[i] -= (rand() % 10) / 1000.0f;
            if (bakery.resources[i] < 0) bakery.resources[i] = 0;
            if (rand() % 500 == 0) bakery.resources[i] += 0.2f;
        }
        
        // Check termination conditions
        if (bakery.customers_frustrated > 10 || 
            bakery.customers_complained > 5 || 
            bakery.customers_missing_items > 8 ||
            bakery.profit > 500.0f) {
            bakery.simulation_running = 0;
        }
        
        glutPostRedisplay();
        glutTimerFunc(16, update, 0);
    }
}

void addCustomer() {
    for (int i = 0; i < MAX_CUSTOMERS; i++) {
        if (!bakery.customers[i].active) {
            bakery.customers[i].active = 1;
            bakery.customers[i].state = 0;
            bakery.customers[i].progress = 0;
            bakery.customers[i].x = 600; // Start at middle door
            bakery.customers[i].y = 50;
            bakery.customers[i].will_complain = (rand() % 10) == 0;
            bakery.customers[i].will_buy = (rand() % 10) != 0;
            bakery.customers[i].mood = 0;
            return;
        }
    }
}

void addProduct() {
    for (int i = 0; i < MAX_PRODUCTS; i++) {
        if (bakery.products[i].freshness <= 0) {  // Find an empty slot
            int type = rand() % PRODUCT_TYPES;
            bakery.products[i].type = type;
            bakery.products[i].freshness = 100;
            
            // Get the correct display table for this product type
            Table table = display_tables[type];
            
            // Calculate positions within table boundaries
            float padding = 15.0f;
            bakery.products[i].x = table.x + padding + 
                                 (rand() % (int)(table.width - 2*padding));
            bakery.products[i].y = table.y + padding + 
                                 (rand() % (int)(table.height - 2*padding));
            
            return;
        }
    }
}

void updateCustomer(Customer *c) {
    const float speed = 0.8f;
    const float browse_speed = 0.3f;
    
    switch (c->state) {
        case 0: // Entering
            c->y += speed;
            if (c->y >= 150) {
                c->state = 1;
                c->progress = 0;
                // Head toward a random table
                int table = rand() % PRODUCT_TYPES;
                c->x = prep_tables[table].x + prep_tables[table].width/2;
            }
            break;
            
        case 1: // Browsing
            c->progress += 0.005f;
            // Move randomly near the table
            c->x += (rand() % 100 - 50) * 0.01f * browse_speed;
            c->y += (rand() % 100 - 50) * 0.01f * browse_speed;
            
            // Keep within reasonable bounds
            c->x = fmax(100, fmin(1100, c->x));
            c->y = fmax(150, fmin(400, c->y));
            
            if (c->progress >= 1.0f) {
                c->state = 2;
                if (c->will_complain) {
                    bakery.customers_complained++;
                    bakery.profit -= 10.0f;
                    c->mood = 3; // Angry
                } else if (c->will_buy) {
                    c->mood = 1; // Happy
                    // Remove a product of the type they were browsing
                    for (int i = 0; i < MAX_PRODUCTS; i++) {
                        if (bakery.products[i].freshness > 0) {
                            // Find which table they're nearest to
                            for (int t = 0; t < PRODUCT_TYPES; t++) {
                                if (fabs(c->x - prep_tables[t].x - prep_tables[t].width/2) < prep_tables[t].width/2) {
                                    if (bakery.products[i].type == t) {
                                        bakery.products[i].freshness = 0;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                } else {
                    bakery.customers_frustrated++;
                    c->mood = 2; // Frustrated
                }
            }
            break;
            
        case 2: // Leaving
            c->y -= speed;
            if (c->y <= 50) {
                c->active = 0;
            }
            break;
    }
}

void updateProducts() {
    for (int i = 0; i < MAX_PRODUCTS; i++) {
        if (bakery.products[i].freshness > 0) {
            bakery.products[i].freshness--;
        }
    }
}


void drawProducts() {
    // Get product counts from shared memory
    int paste_count = modify_shared_int(sem_paste_id, shm_paste_ptr, 0);
    int cake_count = modify_shared_int(sem_cake_paste_id, shm_cake_paste_ptr, 0);
    int sandwich_count = modify_shared_int(sem_sandwiches_id, shm_sandwiches_ptr, 0);
    int sweets_count = modify_shared_int(sem_sweets_paste_id, shm_sweets_paste_ptr, 0);
    int sweet_patiss_count = modify_shared_int(sem_sweet_patiss_paste_id, shm_sweet_patiss_paste_ptr, 0);
    int savory_patiss_count = modify_shared_int(sem_savory_patiss_paste_id, shm_savory_patiss_paste_ptr, 0);

    // Clear existing products
    for (int i = 0; i < MAX_PRODUCTS; i++) {
        bakery.products[i].freshness = 0;
    }

    // Draw paste products (BREAD)
    int product_index = 0;
    for (int i = 0; i < paste_count && product_index < MAX_PRODUCTS; i++) {
        bakery.products[product_index].type = BREAD;
        bakery.products[product_index].freshness = 100;
        bakery.products[product_index].x = display_tables[0].x + 30 + (i % 3) * 40;
        bakery.products[product_index].y = display_tables[0].y + 20 + (i / 3) * 30;
        product_index++;
    }

    // Draw cake products (CAKE)
    for (int i = 0; i < cake_count && product_index < MAX_PRODUCTS; i++) {
        bakery.products[product_index].type = CAKE;
        bakery.products[product_index].freshness = 100;
        bakery.products[product_index].x = display_tables[1].x + 30 + (i % 3) * 40;
        bakery.products[product_index].y = display_tables[1].y + 20 + (i / 3) * 30;
        product_index++;
    }

    // Draw sandwich products (SANDWICH)
    for (int i = 0; i < sandwich_count && product_index < MAX_PRODUCTS; i++) {
        bakery.products[product_index].type = SANDWICH;
        bakery.products[product_index].freshness = 100;
        bakery.products[product_index].x = display_tables[2].x + 30 + (i % 3) * 40;
        bakery.products[product_index].y = display_tables[2].y + 20 + (i / 3) * 30;
        product_index++;
    }

    // Draw sweets products (SWEET)
    for (int i = 0; i < sweets_count && product_index < MAX_PRODUCTS; i++) {
        bakery.products[product_index].type = SWEET;
        bakery.products[product_index].freshness = 100;
        bakery.products[product_index].x = display_tables[3].x + 30 + (i % 3) * 40;
        bakery.products[product_index].y = display_tables[3].y + 20 + (i / 3) * 30;
        product_index++;
    }

    // Draw sweet patisserie products (SWEET_PATISS)
    for (int i = 0; i < sweet_patiss_count && product_index < MAX_PRODUCTS; i++) {
        bakery.products[product_index].type = SWEET_PATISS;
        bakery.products[product_index].freshness = 100;
        bakery.products[product_index].x = display_tables[4].x + 30 + (i % 3) * 40;
        bakery.products[product_index].y = display_tables[4].y + 20 + (i / 3) * 30;
        product_index++;
    }

    // Draw savory patisserie products (SAVORY_PATISS)
    for (int i = 0; i < savory_patiss_count && product_index < MAX_PRODUCTS; i++) {
        bakery.products[product_index].type = SAVORY_PATISS;
        bakery.products[product_index].freshness = 100;
        bakery.products[product_index].x = display_tables[5].x + 30 + (i % 3) * 40;
        bakery.products[product_index].y = display_tables[5].y + 20 + (i / 3) * 30;
        product_index++;
    }

    // Draw all active products
    for (int i = 0; i < MAX_PRODUCTS; i++) {
        if (bakery.products[i].freshness > 0) {
            drawProduct(bakery.products[i].x, bakery.products[i].y, bakery.products[i].type);
            
            // Draw freshness indicator
            float freshness = bakery.products[i].freshness / 100.0f;
            glColor3f(0.3f, 0.3f, 0.3f);
            glBegin(GL_LINE_LOOP);
            glVertex2f(bakery.products[i].x - 8, bakery.products[i].y + 12);
            glVertex2f(bakery.products[i].x + 8, bakery.products[i].y + 12);
            glVertex2f(bakery.products[i].x + 8, bakery.products[i].y + 14);
            glVertex2f(bakery.products[i].x - 8, bakery.products[i].y + 14);
            glEnd();
            
            if (freshness > 0.6f) {
                glColor3f(0.0f, 1.0f, 0.0f);  // Green for fresh
            } else if (freshness > 0.3f) {
                glColor3f(1.0f, 1.0f, 0.0f);  // Yellow for medium freshness
            } else {
                glColor3f(1.0f, 0.0f, 0.0f); // Red for about to expire
            }
            
            glBegin(GL_QUADS);
            glVertex2f(bakery.products[i].x - 7, bakery.products[i].y + 13);
            glVertex2f(bakery.products[i].x - 7 + freshness*14, bakery.products[i].y + 13);
            glVertex2f(bakery.products[i].x - 7 + freshness*14, bakery.products[i].y + 14);
            glVertex2f(bakery.products[i].x - 7, bakery.products[i].y + 14);
            glEnd();
        }
    }
}
void drawCustomer(float x, float y, int mood) {
    // Head
    switch(mood) {
        case 0: // Neutral
            glColor3f(1.0f, 0.8f, 0.6f);
            break;
        case 1: // Happy
            glColor3f(0.3f, 0.9f, 0.3f);
            break;
        case 2: // Frustrated
            glColor3f(0.9f, 0.9f, 0.3f);
            break;
        case 3: // Angry
            glColor3f(0.9f, 0.3f, 0.3f);
            break;
    }
    
    // Head
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i += 20) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(x + cos(angle) * 12, y + 25 + sin(angle) * 12);
    }
    glEnd();
    
    // Body
    glColor3f(0.2f, 0.4f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(x - 10, y);
    glVertex2f(x + 10, y);
    glVertex2f(x + 10, y - 25);
    glVertex2f(x - 10, y - 25);
    glEnd();
    
    // Eyes
    glColor3f(0.0f, 0.0f, 0.0f);
    glPointSize(4.0f);
    glBegin(GL_POINTS);
    glVertex2f(x - 5, y + 30);
    glVertex2f(x + 5, y + 30);
    glEnd();
    
    // Mouth/Lips
    if (mood == 1) { // Happy (smile)
        glColor3f(0.6f, 0.2f, 0.2f); // Red lips
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i < 180; i += 10) {
            float angle = i * 3.14159f / 180.0f;
            glVertex2f(x + cos(angle) * 8, y + 20 + sin(angle) * 3);
        }
        glEnd();
    } 
    else if (mood == 3) { // Angry (frown)
        glColor3f(0.6f, 0.2f, 0.2f); // Red lips
        glBegin(GL_LINE_STRIP);
        for (int i = 180; i < 360; i += 10) {
            float angle = i * 3.14159f / 180.0f;
            glVertex2f(x + cos(angle) * 8, y + 20 + sin(angle) * 3);
        }
        glEnd();
    } 
    else { // Neutral or frustrated
        glColor3f(0.6f, 0.2f, 0.2f); // Red lips
        glBegin(GL_LINES);
        glVertex2f(x - 6, y + 20);
        glVertex2f(x + 6, y + 20);
        glEnd();
    }
}


void drawCustomers() {
    for (int i = 0; i < MAX_CUSTOMERS; i++) {
        if (bakery.customers[i].active) {
            drawCustomer(bakery.customers[i].x, bakery.customers[i].y, bakery.customers[i].mood);
        }
    }
}


void drawResources() {
    const char* resource_names[] = {"Wheat", "Yeast", "Butter", "Milk", "Sugar", "Salt", "Sweet Items", "Cheese", "Salami"};
    
    // Get current values from shared memory
    float resources[9];
    resources[0] = modify_shared_int(sem_wheat_id, shm_wheat_ptr, 0) / (float)config.wheat_amount_max;
    resources[1] = modify_shared_int(sem_yeast_id, shm_yeast_ptr, 0) / (float)config.yeast_amount_max;
    resources[2] = modify_shared_int(sem_butter_id, shm_butter_ptr, 0) / (float)config.butter_amount_max;
    resources[3] = modify_shared_int(sem_milk_id, shm_milk_ptr, 0) / (float)config.milk_amount_max;
    resources[4] = modify_shared_int(sem_sugar_id, shm_sugar_ptr, 0) / (float)config.sugar_amount_max;
    resources[5] = modify_shared_int(sem_salt_id, shm_salt_ptr, 0) / (float)config.salt_amount_max;
    resources[6] = modify_shared_int(sem_sweet_items_id, shm_sweet_items_ptr, 0) / (float)config.sweet_items_amount_max;
    resources[7] = modify_shared_int(sem_cheese_id, shm_cheese_ptr, 0) / (float)config.cheese_amount_max;
    resources[8] = modify_shared_int(sem_salami_id, shm_salami_ptr, 0) / (float)config.salami_amount_max;

    // Display resources horizontally at the top
    float x_start = 20.0f; // Starting x position
    float y_position = 770.0f; // Shifted y position for resource names
    float spacing = 130.0f; // Spacing between resource displays

    for (int i = 0; i < 9; i++) {
        char buffer[100];
        float current = resources[i] * ((i == 0) ? config.wheat_amount_max : 
                                      (i == 1) ? config.yeast_amount_max :
                                      (i == 2) ? config.butter_amount_max :
                                      (i == 3) ? config.milk_amount_max :
                                      (i == 4) ? config.sugar_amount_max :
                                      (i == 5) ? config.salt_amount_max :
                                      (i == 6) ? config.sweet_items_amount_max :
                                      (i == 7) ? config.cheese_amount_max : config.salami_amount_max);
        
        float max = (i == 0) ? config.wheat_amount_max : 
                   (i == 1) ? config.yeast_amount_max :
                   (i == 2) ? config.butter_amount_max :
                   (i == 3) ? config.milk_amount_max :
                   (i == 4) ? config.sugar_amount_max :
                   (i == 5) ? config.salt_amount_max :
                   (i == 6) ? config.sweet_items_amount_max :
                   (i == 7) ? config.cheese_amount_max : config.salami_amount_max;
        
        sprintf(buffer, "%.1f/%.1f", current, max);

        // Draw resource name in bold
        glColor3f(0.0f, 0.0f, 0.0f);
        glRasterPos2f(x_start + i * spacing, y_position);
        int len = (int)strlen(resource_names[i]);
        for (int j = 0; j < len; j++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, resource_names[i][j]); // Larger font for bold effect
        }

        // Draw resource value under the name with larger text
        glRasterPos2f(x_start + i * spacing + 10, y_position - 15);
        len = (int)strlen(buffer);
        for (int j = 0; j < len; j++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, buffer[j]); // Larger font
        }
        
        // Draw resource bar
        glColor3f(0.8f, 0.8f, 0.8f);
        glBegin(GL_QUADS);
        glVertex2f(x_start + i * spacing, y_position - 30);
        glVertex2f(x_start + i * spacing + 100, y_position - 30);
        glVertex2f(x_start + i * spacing + 100, y_position - 40);
        glVertex2f(x_start + i * spacing, y_position - 40);
        glEnd();
        
        if (resources[i] > 0.3f) {
            glColor3f(0.2f, 0.8f, 0.2f);
        } else if (resources[i] > 0.1f) {
            glColor3f(0.8f, 0.8f, 0.2f);
        } else {
            glColor3f(0.8f, 0.2f, 0.2f);
        }
        
        glBegin(GL_QUADS);
        glVertex2f(x_start + i * spacing, y_position - 30);
        glVertex2f(x_start + i * spacing + resources[i] * 100, y_position - 30);
        glVertex2f(x_start + i * spacing + resources[i] * 100, y_position - 40);
        glVertex2f(x_start + i * spacing, y_position - 40);
        glEnd();
        
        glColor3f(0.0f, 0.0f, 0.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(x_start + i * spacing, y_position - 30);
        glVertex2f(x_start + i * spacing + 100, y_position - 30);
        glVertex2f(x_start + i * spacing + 100, y_position - 40);
        glVertex2f(x_start + i * spacing, y_position - 40);
        glEnd();
    }
}


void drawStatus() {
    char buffer[100];
    
    glColor3f(0.0f, 0.0f, 0.0f);
    sprintf(buffer, "Profit: $%.2f", bakery.profit);
    drawText(50, 460, buffer);
    
    sprintf(buffer, "Served: %d", bakery.customers_served);
    drawText(50, 435, buffer);
    
    sprintf(buffer, "Frustrated: %d", bakery.customers_frustrated);
    drawText(50, 410, buffer);
    
    sprintf(buffer, "Complaints: %d", bakery.customers_complained);
    drawText(50, 385, buffer);
    
    if (bakery.simulation_running) {
        glColor3f(0.0f, 0.8f, 0.0f);
        drawText(50, 360, "Status: Running");
    } else {
        glColor3f(0.8f, 0.0f, 0.0f);
        drawText(50, 360, "Status: Stopped");
    }
    }


void drawText(float x, float y, const char* string) {
    if (!string) return;
    
    glRasterPos2f(x, y);
    int len = (int)strlen(string);
    for (int i = 0; i < len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, string[i]);
    }
}

void drawPreparationProgress(){
    // Array of shared memory pointers and corresponding semaphores
    char *shm_ptrs[] = {shm_paste_ptr, shm_cake_paste_ptr, shm_sweets_paste_ptr, shm_sweet_patiss_paste_ptr, shm_savory_patiss_paste_ptr};
    int sem_ids[] = {sem_paste_id, sem_cake_paste_id, sem_sweets_paste_id, sem_sweet_patiss_paste_id, sem_savory_patiss_paste_id};

    // Iterate over each preparation table
    for (int i = 0; i < 5; i++) {
        // Get the current value from shared memory
        int value = modify_shared_int(sem_ids[i], shm_ptrs[i], 0);

        // Check if value is null, make it zero
        if (value < 0) {
            value = 0;
        }

        // Find the center of the corresponding preparation table
        float center_x = prep_tables[i].x + prep_tables[i].width / 2;
        float center_y = prep_tables[i].y + prep_tables[i].height / 2;

        // Convert the value to a string
        char buffer[20];
        sprintf(buffer, "%d", value);

        // Display the value at the center of the table
        glColor3f(0.0f, 0.0f, 0.0f); // Black text
        drawText(center_x - 10, center_y, buffer); // Adjust x slightly for centering
    }
}
void drawFinalProducts(){
    // Draw bread categories
    for (int i = 0; i < bakery.bread_categories; i++) {
        int value = modify_shared_int(bread_catagories_sem_id[i], bread_catagories_shm_ptr[i], 0);
        if (value < 0) value = 0; // Ensure value is non-negative

        float center_x = display_tables[0].x + 30 + (i % 3) * 40;
        float center_y = display_tables[0].y + 20 + (i / 3) * 30;

        char buffer[20];
        sprintf(buffer, "%d", value);

        glColor3f(0.0f, 0.0f, 0.0f); // Black text
        drawText(center_x, center_y, buffer);
    }

    // Draw sandwich types
    for (int i = 0; i < bakery.sandwich_types; i++) {
        int value = modify_shared_int(sandwiches_sem_id[i], sandwiches_shm_ptr[i], 0);
        if (value < 0) value = 0;

        float center_x = display_tables[2].x + 30 + (i % 3) * 40;
        float center_y = display_tables[2].y + 20 + (i / 3) * 30;

        char buffer[20];
        sprintf(buffer, "%d", value);

        glColor3f(0.0f, 0.0f, 0.0f);
        drawText(center_x, center_y, buffer);
    }

    // Draw cake flavors
    for (int i = 0; i < bakery.cake_flavors; i++) {
        int value = modify_shared_int(cake_flavors_sem_id[i], cake_flavors_shm_ptr[i], 0);
        if (value < 0) value = 0;

        float center_x = display_tables[1].x + 30 + (i % 3) * 40;
        float center_y = display_tables[1].y + 20 + (i / 3) * 30;

        char buffer[20];
        sprintf(buffer, "%d", value);

        glColor3f(0.0f, 0.0f, 0.0f);
        drawText(center_x, center_y, buffer);
    }

    // Draw sweets flavors
    for (int i = 0; i < bakery.sweets_flavors; i++) {
        int value = modify_shared_int(sweets_flavors_sem_id[i], sweets_flavors_shm_ptr[i], 0);
        if (value < 0) value = 0;

        float center_x = display_tables[3].x + 30 + (i % 3) * 40;
        float center_y = display_tables[3].y + 20 + (i / 3) * 30;

        char buffer[20];
        sprintf(buffer, "%d", value);

        glColor3f(0.0f, 0.0f, 0.0f);
        drawText(center_x, center_y, buffer);
    }

    // Draw sweet patisseries
    for (int i = 0; i < bakery.patisserie_types / 2; i++) {
        int value = modify_shared_int(sweet_patisseries_sem_id[i], sweet_patisseries_shm_ptr[i], 0);
        if (value < 0) value = 0;

        float center_x = display_tables[4].x + 30 + (i % 3) * 40;
        float center_y = display_tables[4].y + 20 + (i / 3) * 30;

        char buffer[20];
        sprintf(buffer, "%d", value);

        glColor3f(0.0f, 0.0f, 0.0f);
        drawText(center_x, center_y, buffer);
    }

    // Draw savory patisseries
    for (int i = 0; i < bakery.patisserie_types / 2; i++) {
        int value = modify_shared_int(savory_patisseries_sem_id[i], savory_patisseries_shm_ptr[i], 0);
        if (value < 0) value = 0;

        float center_x = display_tables[5].x + 30 + (i % 3) * 40;
        float center_y = display_tables[5].y + 20 + (i / 3) * 30;

        char buffer[20];
        sprintf(buffer, "%d", value);

        glColor3f(0.0f, 0.0f, 0.0f);
        drawText(center_x, center_y, buffer);
    }
    


    
}