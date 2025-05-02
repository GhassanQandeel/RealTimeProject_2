//gcc bakery_opengl.c -o bakery_opengl -lglut -lGL -lGLU -lm



// bakery_visualizer.c
// Compile with: gcc bakery_visualizer.c -o bakery_visualizer -lGL -lGLU -lglut
//gcc -o bakery_visualizer bakery_visualizer.c-lglut -lGL -lGLU -lm
/* 
Natural skin tone (peach) when entering
Green when happy (purchased)
Yellow when frustrated (left without buying)
Red should appear for angry customers (complained)
Body: Always blue (the rectangle you're seeing)*/
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define MAX_CUSTOMERS 20
#define MAX_PRODUCTS 50
#define MAX_STAFF 10

// Product types
#define BREAD 0
#define CAKE 1
#define SANDWICH 2
#define SWEET 3
#define PATISSERIE 4
#define PRODUCT_TYPES 5

// Staff types
#define CHEF 0
#define BAKER 1
#define SELLER 2

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
    // Configuration
    int bread_categories;
    int sandwich_types;
    int cake_flavors;
    int sweets_flavors;
    int patisserie_types;
    
    // Staff counts
    int chefs;
    int bakers;
    int sellers;
    int supply_employees;
    
    // Resources
    float resources[8]; // wheat, yeast, butter, milk, sugar_salt, sweet_items, cheese, salami
    
    // Stats
    int customers_served;
    int customers_frustrated;
    int customers_complained;
    int customers_missing_items;
    float profit;
    
    // Simulation state
    int simulation_running;
    Customer customers[MAX_CUSTOMERS];
    Product products[MAX_PRODUCTS];
    Staff staff[MAX_STAFF];
    float time_since_last_customer;
    float time_since_last_product;
} BakeryState;

BakeryState bakery;

// Table positions
typedef struct {
    float x, y;
    float width, height;
    const char* label;
} Table;

Table prep_tables[3] = {
    {150, 650, 200, 80, "Dough Prep"},   // Dough preparation
    {450, 650, 200, 80, "Baking Station"}, // Baking
    {750, 650, 200, 80, "Decoration"}     // Cake decoration
};

Table display_tables[PRODUCT_TYPES] = {
    {150, 250, 150, 60, "Bread"},      // Bread table
    {325, 250, 150, 60, "Cakes"},      // Cake table
    {500, 250, 150, 60, "Sandwiches"}, // Sandwich table
    {675, 250, 150, 60, "Sweets"},     // Sweet table
    {850, 250, 150, 60, "Patisserie"}  // Patisserie table
};

// Function prototypes
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

int main(int argc, char** argv) {
    srand(time(NULL));
    
    // Initialize bakery state
    memset(&bakery, 0, sizeof(BakeryState));
    bakery.simulation_running = 1;
    bakery.chefs = 3;
    bakery.bakers = 2;
    bakery.sellers = 2;
    bakery.supply_employees = 1;
    bakery.bread_categories = 3;
    bakery.sandwich_types = 2;
    bakery.cake_flavors = 4;
    bakery.sweets_flavors = 3;
    bakery.patisserie_types = 2;
    
    for (int i = 0; i < 8; i++) {
        bakery.resources[i] = 1.0f;
    }

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

    init();
    glutMainLoop();
    return 0;
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
    drawSalesArea();
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
    glVertex2f(50, 400);
    glVertex2f(1150, 400);
    glEnd();
    
    // Label for areas
    glColor3f(0.3f, 0.3f, 0.3f);
    drawText(100, 380, "PREPARATION AREA");
    drawText(100, 180, "SALES AREA");
}

void drawPreparationArea() {
    // Draw preparation tables
    for (int i = 0; i < 3; i++) {
        drawTable(prep_tables[i].x, prep_tables[i].y, 
                 prep_tables[i].width, prep_tables[i].height, 
                 prep_tables[i].label);
    }
    
    // Draw staff in preparation area
    for (int i = 0; i < bakery.chefs + bakery.bakers; i++) {
        drawStaffMember(bakery.staff[i].x, bakery.staff[i].y, 
                       bakery.staff[i].type, bakery.staff[i].busy);
    }
}

void drawSalesArea() {
    // Draw product display tables
    drawProductTables();
    drawProducts();
    
    // Draw customers
    drawCustomers();
    
    // Draw sellers
    for (int i = bakery.chefs + bakery.bakers; 
         i < bakery.chefs + bakery.bakers + bakery.sellers; i++) {
        drawStaffMember(bakery.staff[i].x, bakery.staff[i].y, 
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
    // Organize products by type first for better table arrangement
    int products_per_type[PRODUCT_TYPES] = {0};
    
    // Count products per type
    for (int i = 0; i < MAX_PRODUCTS; i++) {
        if (bakery.products[i].freshness > 0) {
            products_per_type[bakery.products[i].type]++;
        }
    }
    
    // Draw products with organized positioning
    for (int t = 0; t < PRODUCT_TYPES; t++) {
        Table table = display_tables[t];
        int products_on_table = 0;
        
        for (int i = 0; i < MAX_PRODUCTS; i++) {
            if (bakery.products[i].freshness > 0 && bakery.products[i].type == t) {
                // Calculate grid position if multiple products on same table
                int max_per_row = 3;
                float x_spacing = table.width / (max_per_row + 1);
                float y_spacing = table.height / 2;
                
                if (products_per_type[t] > max_per_row) {
                    // Grid layout for crowded tables
                    int row = products_on_table / max_per_row;
                    int col = products_on_table % max_per_row;
                    bakery.products[i].x = table.x + (col + 1) * x_spacing;
                    bakery.products[i].y = table.y + (row + 1) * y_spacing/2;
                } else {
                    // Center items when few products
                    bakery.products[i].x = table.x + table.width/2;
                    bakery.products[i].y = table.y + table.height/2;
                }
                
                // Draw the product
                drawProduct(bakery.products[i].x, bakery.products[i].y, t);
                
                // Draw freshness indicator
                glColor3f(0.3f, 0.3f, 0.3f);
                glBegin(GL_LINE_LOOP);
                glVertex2f(bakery.products[i].x - 8, bakery.products[i].y + 12);
                glVertex2f(bakery.products[i].x + 8, bakery.products[i].y + 12);
                glVertex2f(bakery.products[i].x + 8, bakery.products[i].y + 14);
                glVertex2f(bakery.products[i].x - 8, bakery.products[i].y + 14);
                glEnd();
                
                // Freshness level (green -> yellow -> red)
                float freshness = bakery.products[i].freshness/100.0f;
                if (freshness > 0.6f) {
                    glColor3f(0.0f, 1.0f, 0.0f);  // Green
                } else if (freshness > 0.3f) {
                    glColor3f(1.0f, 1.0f, 0.0f);  // Yellow
                } else {
                    glColor3f(1.0f, 0.0f, 0.0f);  // Red
                }
                
                glBegin(GL_QUADS);
                glVertex2f(bakery.products[i].x - 7, bakery.products[i].y + 13);
                glVertex2f(bakery.products[i].x - 7 + freshness*14, 
                          bakery.products[i].y + 13);
                glVertex2f(bakery.products[i].x - 7 + freshness*14, 
                          bakery.products[i].y + 14);
                glVertex2f(bakery.products[i].x - 7, bakery.products[i].y + 14);
                glEnd();
                
                products_on_table++;
            }
        }
    }
}

void drawProduct(float x, float y, int type) {
    switch(type) {
        case BREAD:
            glColor3f(0.85f, 0.75f, 0.55f); // Bread color
            glBegin(GL_POLYGON);
            for (int i = 0; i < 360; i += 20) {
                float angle = i * 3.14159f / 180.0f;
                glVertex2f(x + cos(angle) * 12, y + sin(angle) * 8);
            }
            glEnd();
            // Bread cuts
            glColor3f(0.7f, 0.6f, 0.4f);
            glBegin(GL_LINES);
            glVertex2f(x - 8, y - 3);
            glVertex2f(x - 8, y + 3);
            glVertex2f(x, y - 4);
            glVertex2f(x, y + 4);
            glVertex2f(x + 8, y - 3);
            glVertex2f(x + 8, y + 3);
            glEnd();
            break;
            
        case CAKE:
            glColor3f(0.9f, 0.8f, 0.7f); // Cake base
            glBegin(GL_POLYGON);
            for (int i = 0; i < 360; i += 20) {
                float angle = i * 3.14159f / 180.0f;
                glVertex2f(x + cos(angle) * 15, y + sin(angle) * 10);
            }
            glEnd();
            // Icing
            glColor3f(0.95f, 0.8f, 0.9f);
            glBegin(GL_POLYGON);
            for (int i = 0; i < 360; i += 20) {
                float angle = i * 3.14159f / 180.0f;
                glVertex2f(x + cos(angle) * 12, y + 5 + sin(angle) * 5);
            }
            glEnd();
            break;
            
        case SANDWICH:
            // Bread
            glColor3f(0.9f, 0.8f, 0.6f);
            glBegin(GL_QUADS);
            glVertex2f(x - 15, y - 5);
            glVertex2f(x + 15, y - 5);
            glVertex2f(x + 15, y + 5);
            glVertex2f(x - 15, y + 5);
            glEnd();
            // Filling
            glColor3f(0.8f, 0.6f, 0.4f);
            glBegin(GL_QUADS);
            glVertex2f(x - 12, y - 3);
            glVertex2f(x + 12, y - 3);
            glVertex2f(x + 12, y + 3);
            glVertex2f(x - 12, y + 3);
            glEnd();
            break;
            
        case SWEET:
            glColor3f(0.8f, 0.2f, 0.2f); // Red sweet
            glBegin(GL_POLYGON);
            for (int i = 0; i < 360; i += 20) {
                float angle = i * 3.14159f / 180.0f;
                glVertex2f(x + cos(angle) * 10, y + sin(angle) * 10);
            }
            glEnd();
            // Decoration
            glColor3f(1.0f, 1.0f, 1.0f);
            glBegin(GL_LINES);
            for (int i = 0; i < 360; i += 90) {
                float angle = i * 3.14159f / 180.0f;
                glVertex2f(x, y);
                glVertex2f(x + cos(angle) * 8, y + sin(angle) * 8);
            }
            glEnd();
            break;
            
        case PATISSERIE:
            // Base
            glColor3f(0.7f, 0.5f, 0.3f);
            glBegin(GL_POLYGON);
            for (int i = 0; i < 360; i += 20) {
                float angle = i * 3.14159f / 180.0f;
                glVertex2f(x + cos(angle) * 12, y + sin(angle) * 8);
            }
            glEnd();
            // Topping
            glColor3f(0.9f, 0.7f, 0.5f);
            glBegin(GL_POLYGON);
            for (int i = 0; i < 360; i += 20) {
                float angle = i * 3.14159f / 180.0f;
                glVertex2f(x + cos(angle) * 8, y + 5 + sin(angle) * 5);
            }
            glEnd();
            break;
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
    const char* resource_names[] = {"Wheat", "Yeast", "Butter", "Milk", "Sugar/Salt", "Sweet Items", "Cheese", "Salami"};
    
    glColor3f(0.0f, 0.0f, 0.0f);
    drawText(20, 750, "Resources:");
    
    for (int i = 0; i < 8; i++) {
        char buffer[100];
        sprintf(buffer, "%s: %.1f", resource_names[i], bakery.resources[i]);
        drawText(20, 720 - i*25, buffer);
        
        glColor3f(0.8f, 0.8f, 0.8f);
        glBegin(GL_QUADS);
        glVertex2f(150, 715 - i*25);
        glVertex2f(250, 715 - i*25);
        glVertex2f(250, 705 - i*25);
        glVertex2f(150, 705 - i*25);
        glEnd();
        
        if (bakery.resources[i] > 0.3f) {
            glColor3f(0.2f, 0.8f, 0.2f);
        } else if (bakery.resources[i] > 0.1f) {
            glColor3f(0.8f, 0.8f, 0.2f);
        } else {
            glColor3f(0.8f, 0.2f, 0.2f);
        }
        glBegin(GL_QUADS);
        glVertex2f(150, 715 - i*25);
        glVertex2f(150 + bakery.resources[i] * 100, 715 - i*25);
        glVertex2f(150 + bakery.resources[i] * 100, 705 - i*25);
        glVertex2f(150, 705 - i*25);
        glEnd();
        
        glColor3f(0.0f, 0.0f, 0.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(150, 715 - i*25);
        glVertex2f(250, 715 - i*25);
        glVertex2f(250, 705 - i*25);
        glVertex2f(150, 705 - i*25);
        glEnd();
    }
}

void drawStatus() {
    char buffer[100];
    
    glColor3f(0.0f, 0.0f, 0.0f);
    sprintf(buffer, "Profit: $%.2f", bakery.profit);
    drawText(1000, 750, buffer);
    
    sprintf(buffer, "Served: %d", bakery.customers_served);
    drawText(1000, 725, buffer);
    
    sprintf(buffer, "Frustrated: %d", bakery.customers_frustrated);
    drawText(1000, 700, buffer);
    
    sprintf(buffer, "Complaints: %d", bakery.customers_complained);
    drawText(1000, 675, buffer);
    
    if (bakery.simulation_running) {
        glColor3f(0.0f, 0.8f, 0.0f);
        drawText(1000, 650, "Status: Running");
    } else {
        glColor3f(0.8f, 0.0f, 0.0f);
        drawText(1000, 650, "Status: Stopped");
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
