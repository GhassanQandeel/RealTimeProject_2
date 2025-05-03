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
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#define MAX_CUSTOMERS 20
#define MAX_PRODUCTS 50

// Product types
#define BREAD 0
#define CAKE 1
#define SANDWICH 2
#define SWEET 3
#define PATISSERIE 4
#define PRODUCT_TYPES 5

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
    // Configuration
    int bread_categories;
    int sandwich_types;
    int cake_flavors;
    int sweets_flavors;
    int patisserie_types;
    
    // Staff
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
    float time_since_last_customer;
    float time_since_last_product;
} BakeryState;

BakeryState bakery;

// Table positions
typedef struct {
    float x, y;
    float width;
    const char* label;
} ProductTable;

ProductTable tables[PRODUCT_TYPES] = {
    {150, 250, 150, "Bread"},      // Bread table
    {325, 250, 150, "Cakes"},      // Cake table
    {500, 250, 150, "Sandwiches"}, // Sandwich table
    {675, 250, 150, "Sweets"},     // Sweet table
    {850, 250, 150, "Patisserie"}  // Patisserie table
};

// Function prototypes
void init();
void display();
void reshape(int w, int h);
void update(int value);
void drawFurnace(float xPos, float width, float counterTop, float height);
void drawSeller(int i);
void drawBakery();
void drawResources();
void drawStaff();
void drawCustomers();
void drawProductTables();
void drawProducts();
void drawStatus();
void drawText(float x, float y, const char* string);
void drawCustomer(float x, float y, int mood);
void drawBaker(float x, float y, int type);
void drawProduct(float x, float y, int type);
void addCustomer();
void addProduct();
void updateCustomer(Customer *c);
void updateProducts();

int main(int argc, char** argv) {
    srand(time(NULL));
    
    // Initialize bakery state
    memset(&bakery, 0, sizeof(BakeryState));
    bakery.simulation_running = 1;
    bakery.chefs = 5;
    bakery.bakers = 3;
    bakery.sellers = 2;
    bakery.supply_employees = 2;
    bakery.bread_categories = 3;
    bakery.sandwich_types = 2;
    bakery.cake_flavors = 4;
    bakery.sweets_flavors = 3;
    bakery.patisserie_types = 2;
    
    for (int i = 0; i < 8; i++) {
        bakery.resources[i] = 1.0f;
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
    glClearColor(0.95f, 0.95f, 0.9f, 1.0f); // Light gray background
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 1200, 0, 800);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawBakery();
    drawSeller(1);
    drawProductTables();
    drawProducts();
    drawStaff();
    drawCustomers();
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
        if (!bakery.products[i].freshness) {
            int type = rand() % PRODUCT_TYPES;
            bakery.products[i].type = type;
            bakery.products[i].freshness = 100;
            
            // Position product on its designated table
            ProductTable table = tables[type];
            bakery.products[i].x = table.x + 20 + rand() % (int)(table.width - 40);
            bakery.products[i].y = table.y + 30 + rand() % 40;
            
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
                c->x = tables[table].x + tables[table].width/2;
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
                                if (fabs(c->x - tables[t].x - tables[t].width/2) < tables[t].width/2) {
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

void drawProductTables() {
    for (int i = 0; i < PRODUCT_TYPES; i++) {
        ProductTable table = tables[i];
        
        // Table top
        glColor3f(0.6f, 0.4f, 0.2f); // Wood color
        glBegin(GL_QUADS);
        glVertex2f(table.x, table.y);
        glVertex2f(table.x + table.width, table.y);
        glVertex2f(table.x + table.width, table.y + 20);
        glVertex2f(table.x, table.y + 20);
        glEnd();
        
        // Table legs
        glColor3f(0.4f, 0.3f, 0.2f); // Darker wood
        glBegin(GL_QUADS);
        // Left leg
        glVertex2f(table.x + 10, table.y);
        glVertex2f(table.x + 20, table.y);
        glVertex2f(table.x + 20, table.y - 40);
        glVertex2f(table.x + 10, table.y - 40);
        // Right leg
        glVertex2f(table.x + table.width - 20, table.y);
        glVertex2f(table.x + table.width - 10, table.y);
        glVertex2f(table.x + table.width - 10, table.y - 40);
        glVertex2f(table.x + table.width - 20, table.y - 40);
        glEnd();
        
        // Table label
        glColor3f(0.0f, 0.0f, 0.0f);
        drawText(table.x + table.width/2 - 30, table.y + 40, table.label);
    }
}

void drawProducts() {
    for (int i = 0; i < MAX_PRODUCTS; i++) {
        if (bakery.products[i].freshness > 0) {
            drawProduct(bakery.products[i].x, bakery.products[i].y, bakery.products[i].type);
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

// [Rest of the drawing functions (drawCustomer, drawBaker, drawBakery, 
// drawResources, drawStaff, drawStatus, drawText) remain similar to previous version]



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

void drawBaker(float x, float y, int type) {
    // Body
    glColor3f(0.8f, 0.6f, 0.4f);
    glBegin(GL_QUADS);
    glVertex2f(x - 15, y);
    glVertex2f(x + 15, y);
    glVertex2f(x + 15, y - 40);
    glVertex2f(x - 15, y - 40);
    glEnd();
    
    // Head
    glColor3f(1.0f, 0.8f, 0.6f);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i += 20) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(x + cos(angle) * 12, y + 15 + sin(angle) * 12);
    }
    glEnd();
    
    // Hat based on type
    switch(type) {
        case 0: // Bread baker
            glColor3f(0.3f, 0.2f, 0.1f);
            break;
        case 1: // Cake baker
            glColor3f(0.9f, 0.9f, 0.9f);
            break;
        case 2: // Pastry chef
            glColor3f(0.8f, 0.5f, 0.2f);
            break;
    }
    glBegin(GL_QUADS);
    glVertex2f(x - 15, y + 20);
    glVertex2f(x + 15, y + 20);
    glVertex2f(x + 10, y + 30);
    glVertex2f(x - 10, y + 30);
    glEnd();
}



void drawCustomers() {
    for (int i = 0; i < MAX_CUSTOMERS; i++) {
        if (bakery.customers[i].active) {
            drawCustomer(bakery.customers[i].x, bakery.customers[i].y, bakery.customers[i].mood);
        }
    }
}

void drawStaff() {
    // Draw bakers
    for (int i = 0; i < bakery.bakers; i++) {
        drawBaker(150 + i * 100, 500, i % 3);
    }
    
    // Draw sellers
    glColor3f(0.2f, 0.8f, 0.2f);
    for (int i = 0; i < bakery.sellers; i++) {
        drawBaker(700 + i * 80, 500, 0);
    }
}

void drawFurnace(float xPos, float width, float counterTop, float height) {
    float furnaceWidth = width / 7.0f; // Adjusted width for three furnaces
    float furnaceHeight = (height - counterTop) / 2.0f;
    float furnaceLeft = xPos;
    float furnaceBottom = counterTop;

    // Main body
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(furnaceLeft, furnaceBottom);
    glVertex2f(furnaceLeft + furnaceWidth, furnaceBottom);
    glVertex2f(furnaceLeft + furnaceWidth, furnaceBottom + furnaceHeight);
    glVertex2f(furnaceLeft, furnaceBottom + furnaceHeight);
    glEnd();

    // Top part
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(furnaceLeft, furnaceBottom + furnaceHeight);
    glVertex2f(furnaceLeft + furnaceWidth, furnaceBottom + furnaceHeight);
    glVertex2f(furnaceLeft + furnaceWidth, furnaceBottom + furnaceHeight * 1.2f);
    glVertex2f(furnaceLeft, furnaceBottom + furnaceHeight * 1.2f);
    glEnd();

    // Chimney (darker grey)
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(furnaceLeft + furnaceWidth * 0.4f, furnaceBottom + furnaceHeight * 1.2f);
    glVertex2f(furnaceLeft + furnaceWidth * 0.6f, furnaceBottom + furnaceHeight * 1.2f);
    glVertex2f(furnaceLeft + furnaceWidth * 0.6f, furnaceBottom + furnaceHeight * 1.5f);
    glVertex2f(furnaceLeft + furnaceWidth * 0.4f, furnaceBottom + furnaceHeight * 1.5f);
    glEnd();

    // Fire (yellowish)
    glColor3f(0.9f, 0.7f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(furnaceLeft + furnaceWidth / 2.0f, furnaceBottom + furnaceHeight / 4.0f);
    glVertex2f(furnaceLeft + furnaceWidth / 3.0f, furnaceBottom);
    glVertex2f(furnaceLeft + furnaceWidth * 2.0f / 3.0f, furnaceBottom);
    glEnd();
}

void drawSeller(int i) {
    float baseX = 700.0f; // Starting x-position for sellers
    float spacingX = 80.0f; // Horizontal spacing between sellers
    float baseY = 500.0f; // Base y-position for sellers (feet)
    float scale = 0.8f;   // Overall scale of the seller

    float x = baseX + i * spacingX;
    float y = baseY;

    // Head (light skin tone)
    glColor3f(0.9f, 0.7f, 0.5f);
    float headRadius = 0.2f * scale;
    int numSegments = 36;
    glBegin(GL_POLYGON);
    for (int j = 0; j < numSegments; j++) {
        float angle = 2.0f * M_PI * j / numSegments;
        glVertex2f(x + headRadius * cos(angle), y + 1.8f * scale + headRadius * sin(angle));
    }
    glEnd();

    // Body (blue shirt)
    glColor3f(0.2f, 0.3f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(x - 0.2f * scale, y + 1.0f * scale);
    glVertex2f(x + 0.2f * scale, y + 1.0f * scale);
    glVertex2f(x + 0.2f * scale, y + 0.0f * scale);
    glVertex2f(x - 0.2f * scale, y + 0.0f * scale);
    glEnd();

    // Arms (long sleeves - same color as body)
    glColor3f(0.2f, 0.3f, 0.8f);
    glBegin(GL_QUADS);
    // Left arm
    glVertex2f(x - 0.3f * scale, y + 0.9f * scale);
    glVertex2f(x - 0.5f * scale, y + 0.5f * scale);
    glVertex2f(x - 0.55f * scale, y + 0.0f * scale);
    glVertex2f(x - 0.35f * scale, y + 0.4f * scale);
    // Right arm
    glVertex2f(x + 0.3f * scale, y + 0.9f * scale);
    glVertex2f(x + 0.5f * scale, y + 0.5f * scale);
    glVertex2f(x + 0.55f * scale, y + 0.0f * scale);
    glVertex2f(x + 0.35f * scale, y + 0.4f * scale);
    glEnd();

    // Legs (brown pants)
    glColor3f(0.4f, 0.2f, 0.0f);
    glBegin(GL_QUADS);
    // Left leg
    glVertex2f(x - 0.15f * scale, y + 0.0f * scale);
    glVertex2f(x - 0.05f * scale, y - 0.6f * scale);
    glVertex2f(x + 0.05f * scale, y - 0.6f * scale);
    glVertex2f(x + 0.15f * scale, y + 0.0f * scale);
    // Right leg
    glVertex2f(x + 0.05f * scale, y + 0.0f * scale);
    glVertex2f(x + 0.15f * scale, y - 0.6f * scale);
    glVertex2f(x + 0.25f * scale, y - 0.6f * scale);
    glVertex2f(x + 0.15f * scale, y + 0.0f * scale);
    glEnd();

    // Hair (dark brown)
    glColor3f(0.3f, 0.1f, 0.0f);
    glBegin(GL_POLYGON);
    for (int j = 0; j < numSegments; j++) {
        float angle = M_PI * j / numSegments; // Semicircle
        glVertex2f(x + 0.2f * scale * cos(angle), y + 1.8f * scale + 0.3f * scale + 0.2f * scale * sin(angle));
    }
    glEnd();
}

void drawBakery() {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    int width = viewport[2];
    int height = viewport[3];

    // Background (Walls - light beige, filling the viewport)
    glColor3f(0.9f, 0.8f, 0.7f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(width, 0);
    glVertex2f(width, height);
    glVertex2f(0, height);
    glEnd();

    // Floor (wooden brown, at the bottom of the viewport, 1/6th of the height)
    float floorHeight = height / 6.0f;
    glColor3f(0.6f, 0.3f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(width, 0);
    glVertex2f(width, floorHeight);
    glVertex2f(0, floorHeight);
    glEnd();

    // Floor Tiles with Borders
    int numTilesX = 20;
    int numTilesY = 5;
    float tileWidth = (float)width / numTilesX;
    float tileHeight = (float)(height / 6.0f) / numTilesY;
    float borderWidth = 0.001f; // Adjust for border thickness

    for (int i = 0; i < numTilesX; i++) {
        for (int j = 0; j < numTilesY; j++) {
            // Draw the inner tile (light grey)
            glColor3f(0.87f, 0.72f, 0.53f);
            glBegin(GL_QUADS);
            glVertex2f(i * tileWidth + borderWidth * tileWidth, j * tileHeight + borderWidth * tileHeight);
            glVertex2f((i + 1) * tileWidth - borderWidth * tileWidth, j * tileHeight + borderWidth * tileHeight);
            glVertex2f((i + 1) * tileWidth - borderWidth * tileWidth, (j + 1) * tileHeight - borderWidth * tileHeight);
            glVertex2f(i * tileWidth + borderWidth * tileWidth, (j + 1) * tileHeight - borderWidth * tileHeight);
            glEnd();

            // Draw the border (dark grey)
            glColor3f(0.3f, 0.3f, 0.3f);
            glBegin(GL_LINE_LOOP);
            glVertex2f(i * tileWidth, j * tileHeight);
            glVertex2f((i + 1) * tileWidth, j * tileHeight);
            glVertex2f((i + 1) * tileWidth, (j + 1) * tileHeight);
            glVertex2f(i * tileWidth, (j + 1) * tileHeight);
            glEnd();
        }
    }

    // Counter (light brown, positioned above the floor, full width)
    float counterHeight = height / 4.0f;
    float counterTop = height / 6.0f + counterHeight;
    glColor3f(0.7f, 0.5f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(0, height / 6.0f);         // Starts at the left edge, above the floor
    glVertex2f(width, height / 6.0f);     // Extends to the right edge
    glVertex2f(width, counterTop);      // Height of the counter
    glVertex2f(0, counterTop);          // Connects back to the left edge
    glEnd();

    // Counter Divisions (darker brown vertical lines)
    glColor3f(0.4f, 0.2f, 0.0f);
    for (int i = 1; i < 6; i++) { // Changed to 6 divisions (5 lines)
        float x = (float)width * i / 6.0f;
        glBegin(GL_LINES);
        glVertex2f(x, height / 6.0f);
        glVertex2f(x, counterTop);
        glEnd();
    }

    // Draw three furnaces
    float furnaceSpacing = width / 7.0f; // Space between furnaces
    drawFurnace((width - furnaceSpacing * 3) / 2.0f, width, counterTop, height); // Left furnace
    drawFurnace((width - furnaceSpacing) / 2.0f, width, counterTop, height);   // Center furnace
    drawFurnace((width + furnaceSpacing) / 2.0f, width, counterTop, height);   // Right furnace

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
    drawText(800, 750, buffer);
    
    sprintf(buffer, "Served: %d", bakery.customers_served);
    drawText(800, 725, buffer);
    
    sprintf(buffer, "Frustrated: %d", bakery.customers_frustrated);
    drawText(800, 700, buffer);
    
    sprintf(buffer, "Complaints: %d", bakery.customers_complained);
    drawText(800, 675, buffer);
    
    if (bakery.simulation_running) {
        glColor3f(0.0f, 0.8f, 0.0f);
        drawText(800, 650, "Status: Running");
    } else {
        glColor3f(0.8f, 0.0f, 0.0f);
        drawText(800, 650, "Status: Stopped");
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