#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#ifdef _WIN32
    #include <io.h>
    #include <conio.h>
    #define access _access
    #define F_OK 0
#else
    #include <unistd.h>
    #include <termios.h>
#endif

#ifdef _WIN32
    #include <windows.h>
    #define sleep(x) Sleep(x * 1000)  
#else
    #include <unistd.h>                
#endif


// Function to set console color
void setColor(int textColor, int bgColor) {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (bgColor << 4) | textColor);
#else
    printf("\033[%d;%dm", textColor, bgColor);
#endif
}

#ifdef _WIN32
    #define RESET_COLOR setColor(7, 0)
    #define RED_COLOR setColor(4, 0)
    #define GREEN_COLOR setColor(2, 0) 
    #define YELLOW_COLOR setColor(6, 0) 
    #define BLUE_COLOR setColor(1, 0) 
#else
    #define RESET_COLOR printf("\033[0m")
    #define RED_COLOR printf("\033[1;31m")
    #define GREEN_COLOR printf("\033[1;32m")
    #define YELLOW_COLOR printf("\033[1;33m")
    #define BLUE_COLOR printf("\033[1;34m")
#endif

#ifdef _WIN32
    #define CLEAR_SCREEN system("cls")
#else
    #define CLEAR_SCREEN system("clear")
#endif

// Constants
#define MAX_STRING 100
#define MAX_PRODUCTS 1000
#define MAX_CUSTOMERS 1000
#define MAX_EMPLOYEES 100
#define MAX_CART_ITEMS 50
#define LOYALTY_THRESHOLD 100000
#define LOW_STOCK_THRESHOLD 10
#define PAYMENT_CASH 1
#define PAYMENT_1CARD 2
#define PAYMENT_BKASH 3
#define PAYMENT_NAGAD 4
#define PAYMENT_BANK 5
#define LOYALTY_MILESTONE_1 100000
#define LOYALTY_MILESTONE_2 200000
#define LOYALTY_MILESTONE_3 300000

typedef struct {
    int id;
    char name[MAX_STRING];
    char category[MAX_STRING];
    int quantity;
    float purchase_price;
    float sale_price;
    char date_added[MAX_STRING];
} Product;

typedef struct {
    char name[MAX_STRING];
    char phone[MAX_STRING];
    char address[MAX_STRING];
    float total_spending;
    int loyalty_points;
    int last_loyalty_milestone;
} Customer;

typedef struct {
    int id;
    char name[MAX_STRING];
    char username[MAX_STRING];
    char password[MAX_STRING];
    char role[MAX_STRING];
    float total_sales;
} Employee;

typedef struct {
    int product_id;
    int quantity;
    float price;
} CartItem;

typedef struct {
    long id;
    char customer_phone[MAX_STRING];
    int employee_id;
    char date[30];
    CartItem items[MAX_CART_ITEMS];
    int item_count;
    float total_amount;
    float discount;
    int payment_method;
    char transaction_id[50];
    float manual_discount_percentage;
    float card_discount_percentage;
} Order;

Product products[MAX_PRODUCTS];
Customer customers[MAX_CUSTOMERS];
Employee employees[MAX_EMPLOYEES];
CartItem current_cart[MAX_CART_ITEMS];
int product_count = 0;
int customer_count = 0;
int employee_count = 0;
int cart_count = 0;
Employee current_user;


// Authentication functions
void initializeSystem();
void loginScreen();
int authenticateUser(char* username, char* password);
void registerEmployee();
void changePassword();

// Menu functions
void mainMenu();
void productMenu();
void inventoryMenu();
void salesMenu();
void customerMenu();
void reportMenu();
void settingsMenu();

// Product management functions
void addProduct();
void viewProducts();
void searchProduct();
void editProduct();
void deleteProduct();
void processPayment(Order* order);
void updateInventory(Order order);
void saveProducts();
void loadProducts();

// Inventory management functions
void restockInventory();
void checkLowStock();
void generateStockReport();

// Sales management functions
void createNewSale();
void generateReceipt(Order order); 
void viewSalesHistory();
void addToCart();
void viewCart();
void clearCart();
void checkout();

// Customer management functions
void addCustomer();
void viewCustomerList();
void viewCustomerDetails(); 
void generateCustomerListPDF();
void generateCustomerListCSV();
void generateDetailedCustomerListPDF();
void generateDetailedCustomerListCSV();
void searchCustomer();
void generateCustomerSearchPDF(const char* phone);
void updateCustomer();
void saveCustomers();
void loadCustomers();
void updateEmployeeTotalSales(int employee_id, float sale_amount);
// Report functions
void dailySalesReport();
void monthlySalesReport();
void employeeSalesReport();
void profitReport();

// Utility functions
void clearScreen();
void printHeader(char* title);
void printLine();
char* getCurrentDate();
float calculateProfit(int product_id, int quantity);
float applyDiscount(float amount, const char* phone);
void saveTransactionToFile(Order order);
void generatePDF(Order order);


// Function prototypes
void exportOptions(const char* report_type);
void generateReportPDF(const char* report_type);
void generateReportCSV(const char* report_type);

// Report content generators
void generateProfitReportContent(FILE* file);
void generateDailySalesReportContent(FILE* file);
void generateMonthlySalesReportContent(FILE* file);
void generateEmployeeSalesReportContent(FILE* file);

// CSV generators
void generateProfitReportCSV(FILE* file);
void generateDailySalesReportCSV(FILE* file);
void generateMonthlySalesReportCSV(FILE* file);
void generateEmployeeSalesReportCSV(FILE* file);

// Utility functions
char* getCurrentDateTime(void);


// Utility Functions Implementation
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");  // For macOS and Linux
    #endif
}

void printHeader(char* title) {
    clearScreen();
    printLine();
    printf("\n\t\t%s\n", title);
    printLine();
}

void printLine() {
    printf("\n========================================================\n");
}

char* getCurrentDate() {
    static char date[MAX_STRING];
    time_t now = time(NULL);
    strftime(date, MAX_STRING, "%Y-%m-%d %H:%M:%S", localtime(&now));
    return date;
}


void initializeSystem() {
    loadProducts();
    loadCustomers();
    loginScreen();
}
void loginScreen() {
    char username[MAX_STRING];
    char password[MAX_STRING];
    int attempts = 3;

    while (attempts > 0) {
        printHeader("DIU SUPER SHOP - LOGIN");
        
        printf("\nUsername: ");
        fgets(username, sizeof(username), stdin);
        username[strcspn(username, "\n")] = 0; 

        printf("Password: ");
        
        #ifdef _WIN32
            char ch;
            int i = 0;
            while ((ch = _getch()) != '\r' && i < MAX_STRING - 1) { 
                password[i++] = ch;
                printf("*");
            }
            password[i] = '\0'; 
        #else
            struct termios oldt, newt;
            tcgetattr(STDIN_FILENO, &oldt);
            newt = oldt;
            newt.c_lflag &= ~(ICANON | ECHO);
            tcsetattr(STDIN_FILENO, TCSANOW, &newt);
            char ch;
            int i = 0;
            while ((ch = getchar()) != '\n' && i < MAX_STRING - 1) {
                password[i++] = ch;
                printf("*");
            }
            password[i] = '\0';
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        #endif
        if (authenticateUser(username, password)) {
            GREEN_COLOR;
            printf("\nLogin successful! Welcome, %s\n", current_user.name);
            RESET_COLOR;
            sleep(2);
            mainMenu();
            return;
        } else {
            attempts--;
            RED_COLOR;
            printf("\nInvalid username or password! Attempts remaining: %d\n", attempts);
            RESET_COLOR;
            sleep(2);
        }
    }

    RED_COLOR;
    printf("\nToo many failed attempts. System will now exit.\n");
    RESET_COLOR;
    exit(1);
}

int authenticateUser(char* username, char* password) {
    FILE* file = fopen("employees.txt", "r");
    if (!file) return 0;

    Employee emp;
    while (fscanf(file, "%d,%[^,],%[^,],%[^,],%[^,],%f\n",
           &emp.id, emp.name, emp.username, emp.password,
           emp.role, &emp.total_sales) != EOF) {
        if (strcmp(emp.username, username) == 0 &&
            strcmp(emp.password, password) == 0) {
            current_user = emp;
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

void registerEmployee() {
    if (strcmp(current_user.role, "admin") != 0) {
        RED_COLOR;
        printf("\nOnly administrators can register new employees!\n");
        RESET_COLOR;
        return;
    }

    Employee new_emp;
    printHeader("REGISTER NEW EMPLOYEE");

    printf("\nEnter employee details:\n");
    printf("ID: ");
    scanf("%d", &new_emp.id);
    getchar(); 

    printf("Name: ");
    fgets(new_emp.name, MAX_STRING, stdin);
    new_emp.name[strcspn(new_emp.name, "\n")] = 0;

    printf("Username: ");
    scanf("%s", new_emp.username);

    printf("Password: ");
    scanf("%s", new_emp.password);

    printf("Role (admin/employee): ");
    scanf("%s", new_emp.role);

    new_emp.total_sales = 0.0;

    FILE* file = fopen("employees.txt", "a");
    if (file) {
        fprintf(file, "%d,%s,%s,%s,%s,%.2f\n",
                new_emp.id, new_emp.name, new_emp.username,
                new_emp.password, new_emp.role, new_emp.total_sales);
        fclose(file);
        GREEN_COLOR;
        printf("\nEmployee registered successfully!\n");
        RESET_COLOR;
    } else {
        RED_COLOR;
        printf("\nError registering employee!\n");
        RESET_COLOR;
    }
}

void changePassword() {
    char old_password[MAX_STRING];
    char new_password[MAX_STRING];

    printHeader("CHANGE PASSWORD");

    printf("\nEnter old password: ");
    scanf("%s", old_password);

    if (strcmp(old_password, current_user.password) != 0) {
        RED_COLOR;
        printf("\nIncorrect old password!\n");
        RESET_COLOR;
        return;
    }

    printf("Enter new password: ");
    scanf("%s", new_password);

    FILE* file = fopen("employees.txt", "r");
    FILE* temp = fopen("temp.txt", "w");

    if (!file || !temp) {
        RED_COLOR;
        printf("\nError changing password!\n");
        RESET_COLOR;
        return;
    }

    Employee emp;
    while (fscanf(file, "%d,%[^,],%[^,],%[^,],%[^,],%f\n",
           &emp.id, emp.name, emp.username, emp.password,
           emp.role, &emp.total_sales) != EOF) {
        if (emp.id == current_user.id) {
            strcpy(emp.password, new_password);
            strcpy(current_user.password, new_password);
        }
        fprintf(temp, "%d,%s,%s,%s,%s,%.2f\n",
                emp.id, emp.name, emp.username, emp.password,
                emp.role, emp.total_sales);
    }

    fclose(file);
    fclose(temp);
    remove("employees.txt");
    rename("temp.txt", "employees.txt");

    GREEN_COLOR;
    printf("\nPassword changed successfully!\n");
    RESET_COLOR;
}


void mainMenu() {
    int choice;

    while (1) {
        printHeader("MAIN MENU");
        printf("\n1. Product Management");
        printf("\n2. Inventory Management");
        printf("\n3. Sales Management");
        printf("\n4. Customer Management");
        printf("\n5. Reports");
        printf("\n6. Settings");
        printf("\n7. Logout");
        printf("\n\nEnter your choice: ");

        scanf("%d", &choice);

        switch (choice) {
            case 1: productMenu(); break;
            case 2: inventoryMenu(); break;
            case 3: salesMenu(); break;
            case 4: customerMenu(); break;
            case 5: reportMenu(); break;
            case 6: settingsMenu(); break;
            case 7:
                GREEN_COLOR;
                printf("\nLogging out...\n");
                RESET_COLOR;
                sleep(1);
                loginScreen();
                return;
            default:
                RED_COLOR;
                printf("\nInvalid choice! Please try again.\n");
                RESET_COLOR;
                sleep(1);
        }
    }
}

void productMenu() {
    int choice;

    while (1) {
        printHeader("PRODUCT MANAGEMENT");
        printf("\n1. Add New Product");
        printf("\n2. View All Products");
        printf("\n3. Search Product");
        printf("\n4. Edit Product");
        printf("\n5. Delete Product");
        printf("\n6. Back to Main Menu");
        printf("\n\nEnter your choice: ");

        scanf("%d", &choice);

        switch (choice) {
            case 1: addProduct(); break;
            case 2: viewProducts(); break;
            case 3: searchProduct(); break;
            case 4: editProduct(); break;
            case 5: deleteProduct(); break;
            case 6: return;
            default:
                RED_COLOR;
                printf("\nInvalid choice! Please try again.\n");
                RESET_COLOR;
                sleep(1);
        }
    }
}


void addProduct() {
    Product new_product;
    printHeader("ADD NEW PRODUCT");

    printf("\nEnter product details:\n");
    printf("ID: ");
    scanf("%d", &new_product.id);
    getchar();

    for (int i = 0; i < product_count; i++) {
        if (products[i].id == new_product.id) {
            RED_COLOR;
            printf("\nProduct ID already exists!\n");
            RESET_COLOR;
            sleep(2);
            return;
        }
    }

    printf("Name: ");
    fgets(new_product.name, MAX_STRING, stdin);
    new_product.name[strcspn(new_product.name, "\n")] = 0;

    printf("Category: ");
    fgets(new_product.category, MAX_STRING, stdin);
    new_product.category[strcspn(new_product.category, "\n")] = 0;

    printf("Quantity: ");
    scanf("%d", &new_product.quantity);

    printf("Purchase Price: ");
    scanf("%f", &new_product.purchase_price);

    printf("Sale Price: ");
    scanf("%f", &new_product.sale_price);

    strcpy(new_product.date_added, getCurrentDate());

    products[product_count++] = new_product;
    saveProducts();

    GREEN_COLOR;
    printf("\nProduct added successfully!\n");
    RESET_COLOR;
    sleep(2);
}

void viewProducts() {
    printHeader("PRODUCT LIST");

    if (product_count == 0) {
        YELLOW_COLOR;
        printf("\nNo products available!\n");
        RESET_COLOR;
        sleep(2);
        return;
    }

    printf("\nID\tName\t\tCategory\tQuantity\tPurchase Price\tSale Price\n");
    printLine();

    for (int i = 0; i < product_count; i++) {
        printf("%d\t%-16s%-16s%-16d%.2f\t\t%.2f\n",
               products[i].id,
               products[i].name,
               products[i].category,
               products[i].quantity,
               products[i].purchase_price,
               products[i].sale_price);
    }

    printf("\nPress Enter to continue...");
    getchar();
    getchar();
}

void searchProduct() {
    int choice;
    char search_term[MAX_STRING];
    int found = 0;

    printHeader("SEARCH PRODUCT");
    printf("\n1. Search by ID");
    printf("\n2. Search by Name");
    printf("\n3. Search by Category");
    printf("\nEnter your choice: ");
    scanf("%d", &choice);
    getchar();

    printf("Enter search term: ");
    fgets(search_term, MAX_STRING, stdin);
    search_term[strcspn(search_term, "\n")] = 0;

    printf("\nSearch Results:\n");
    printLine();

    for (int i = 0; i < product_count; i++) {
        int match = 0;
        switch (choice) {
            case 1:
                if (products[i].id == atoi(search_term)) match = 1;
                break;
            case 2:
                if (strstr(products[i].name, search_term)) match = 1;
                break;
            case 3:
                if (strstr(products[i].category, search_term)) match = 1;
                break;
        }

        if (match) {
            printf("\nID: %d", products[i].id);
            printf("\nName: %s", products[i].name);
            printf("\nCategory: %s", products[i].category);
            printf("\nQuantity: %d", products[i].quantity);
            printf("\nPurchase Price: %.2f", products[i].purchase_price);
            printf("\nSale Price: %.2f", products[i].sale_price);
            printf("\nDate Added: %s\n", products[i].date_added);
            printLine();
            found = 1;
        }
    }

    if (!found) {
        YELLOW_COLOR;
        printf("\nNo matching products found!\n");
        RESET_COLOR;
    }

    printf("\nPress Enter to continue...");
    getchar();
}

void editProduct() {
    int id;
    printHeader("EDIT PRODUCT");

    printf("\nEnter Product ID to edit: ");
    scanf("%d", &id);

    for (int i = 0; i < product_count; i++) {
        if (products[i].id == id) {
            printf("\nCurrent Details:");
            printf("\nName: %s", products[i].name);
            printf("\nCategory: %s", products[i].category);
            printf("\nQuantity: %d", products[i].quantity);
            printf("\nPurchase Price: %.2f", products[i].purchase_price);
            printf("\nSale Price: %.2f", products[i].sale_price);

            printf("\n\nEnter new details (press Enter to keep current value):\n");
            char input[MAX_STRING];
            getchar();

            printf("Name: ");
            fgets(input, MAX_STRING, stdin);
            if (input[0] != '\n') {
                input[strcspn(input, "\n")] = 0;
                strcpy(products[i].name, input);
            }

            printf("Category: ");
            fgets(input, MAX_STRING, stdin);
            if (input[0] != '\n') {
                input[strcspn(input, "\n")] = 0;
                strcpy(products[i].category, input);
            }

            printf("Quantity: ");
            fgets(input, MAX_STRING, stdin);
            if (input[0] != '\n') {
                products[i].quantity = atoi(input);
            }

            printf("Purchase Price: ");
            fgets(input, MAX_STRING, stdin);
            if (input[0] != '\n') {
                products[i].purchase_price = atof(input);
            }

            printf("Sale Price: ");
            fgets(input, MAX_STRING, stdin);
            if (input[0] != '\n') {
                products[i].sale_price = atof(input);
            }

            saveProducts();
            GREEN_COLOR;
            printf("\nProduct updated successfully!\n");
            RESET_COLOR;
            sleep(2);
            return;
        }
    }

    RED_COLOR;
    printf("\nProduct not found!\n");
    RESET_COLOR;
    sleep(2);
}

void deleteProduct() {
    int id;
    printHeader("DELETE PRODUCT");

    printf("\nEnter Product ID to delete: ");
    scanf("%d", &id);

    for (int i = 0; i < product_count; i++) {
        if (products[i].id == id) {
        
            char confirm;
            printf("\nAre you sure you want to delete %s? (y/n): ", products[i].name);
            getchar();
            scanf("%c", &confirm);

            if (tolower(confirm) == 'y') {
                // Shift remaining products
                for (int j = i; j < product_count - 1; j++) {
                    products[j] = products[j + 1];
                }
                product_count--;
                saveProducts();

                GREEN_COLOR;
                printf("\nProduct deleted successfully!\n");
                RESET_COLOR;
            } else {
                YELLOW_COLOR;
                printf("\nDeletion cancelled!\n");
                RESET_COLOR;
            }
            sleep(2);
            return;
        }
    }

    RED_COLOR;
    printf("\nProduct not found!\n");
    RESET_COLOR;
    sleep(2);
}

void saveProducts() {
    FILE* file = fopen("products.txt", "w");
    if (!file) {
        RED_COLOR;
        printf("\nError saving products!\n");
        RESET_COLOR;
        return;
    }

    for (int i = 0; i < product_count; i++) {
        fprintf(file, "%d,%s,%s,%d,%.2f,%.2f,%s\n",
                products[i].id,
                products[i].name,
                products[i].category,
                products[i].quantity,
                products[i].purchase_price,
                products[i].sale_price,
                products[i].date_added);
    }

    fclose(file);
}

void loadProducts() {
    FILE* file = fopen("products.txt", "r");
    if (!file) return;

    product_count = 0;
    while (fscanf(file, "%d,%[^,],%[^,],%d,%f,%f,%[^\n]\n",
                  &products[product_count].id,
                  products[product_count].name,
                  products[product_count].category,
                  &products[product_count].quantity,
                  &products[product_count].purchase_price,
                  &products[product_count].sale_price,
                  products[product_count].date_added) != EOF) {
        product_count++;
    }

    fclose(file);
}

// Inventory Management Functions
void inventoryMenu() {
    int choice;

    while (1) {
        printHeader("INVENTORY MANAGEMENT");
        printf("\n1. Restock Inventory");
        printf("\n2. Check Low Stock Items");
        printf("\n3. Generate Stock Report");
        printf("\n4. Back to Main Menu");
        printf("\n\nEnter your choice: ");

        scanf("%d", &choice);

        switch (choice) {
            case 1: restockInventory(); break;
            case 2: checkLowStock(); break;
            case 3: generateStockReport(); break;
            case 4: return;
            default:
                RED_COLOR;
                printf("\nInvalid choice! Please try again.\n");
                RESET_COLOR;
                sleep(1);
        }
    }
}

void restockInventory() {
    int id, quantity;
    printHeader("RESTOCK INVENTORY");

    printf("\nEnter Product ID: ");
    scanf("%d", &id);

    for (int i = 0; i < product_count; i++) {
        if (products[i].id == id) {
            printf("\nCurrent stock for %s: %d", products[i].name, products[i].quantity);
            printf("\nEnter quantity to add: ");
            scanf("%d", &quantity);

            if (quantity < 0) {
                RED_COLOR;
                printf("\nInvalid quantity!\n");
                RESET_COLOR;
                sleep(2);
                return;
            }

            products[i].quantity += quantity;
            saveProducts();

            GREEN_COLOR;
            printf("\nInventory updated successfully!");
            printf("\nNew stock level: %d\n", products[i].quantity);
            RESET_COLOR;
            sleep(2);
            return;
        }
    }

    RED_COLOR;
    printf("\nProduct not found!\n");
    RESET_COLOR;
    sleep(2);
}

void checkLowStock() {
    int found = 0;
    printHeader("LOW STOCK ITEMS");

    printf("\nProducts with stock below %d:\n", LOW_STOCK_THRESHOLD);
    printLine();

    for (int i = 0; i < product_count; i++) {
        if (products[i].quantity < LOW_STOCK_THRESHOLD) {
            printf("\nID: %d", products[i].id);
            printf("\nName: %s", products[i].name);
            printf("\nCurrent Stock: %d", products[i].quantity);
            printLine();
            found = 1;
        }
    }

    if (!found) {
        GREEN_COLOR;
        printf("\nNo low stock items found!\n");
        RESET_COLOR;
    }

    printf("\nPress Enter to continue...");
    getchar();
    getchar();
}

void generateStockReport() {
    float total_value = 0;
    int total_items = 0;

    printHeader("STOCK REPORT");
    printf("\nCurrent Inventory Status:\n");
    printLine();

    printf("\nCategory-wise Summary:\n");
    // Create a temporary array to store unique categories
    char categories[MAX_PRODUCTS][MAX_STRING];
    int category_count = 0;

    // Collect unique categories
    for (int i =0; i < product_count; i++) {
        int found = 0;
        for (int j = 0; j < category_count; j++) {
            if (strcmp(categories[j], products[i].category) == 0) {
                found = 1;
                break;
            }
        }
        if (!found) {
            strcpy(categories[category_count++], products[i].category);
        }
    }

    // Print category-wise summary
    for (int i =0; i < category_count; i++) {
        int cat_items = 0;
        float cat_value = 0;

        printf("\nCategory: %s", categories[i]);
        printf("\n-----------------");

        for (int j =0; j < product_count; j++) {
            if (strcmp(products[j].category, categories[i]) == 0) {
                cat_items += products[j].quantity;
                cat_value += products[j].quantity * products[j].purchase_price;

                printf("\n%s: %d units (Value: %.2f)",
                       products[j].name,
                       products[j].quantity,
                       products[j].quantity * products[j].purchase_price);

                if (products[j].quantity < LOW_STOCK_THRESHOLD) {
                    RED_COLOR;
                    printf(" [LOW STOCK]");
                    RESET_COLOR;
                }
            }
        }

        total_items += cat_items;
        total_value += cat_value;

        printf("\nTotal Items in Category: %d", cat_items);
        printf("\nTotal Value in Category: %.2f\n", cat_value);
    }

    printLine();
    printf("\nOverall Summary:");
    printf("\nTotal Number of Products: %d", product_count);
    printf("\nTotal Items in Stock: %d", total_items);
    printf("\nTotal Inventory Value: %.2f\n", total_value);

    printf("\nPress Enter to continue...");
    getchar();
    getchar();
}

// Sales Management Functions
void salesMenu() {
    int choice;

    while (1) {
        printHeader("SALES MANAGEMENT");
        printf("\n1. Create New Sale");
        printf("\n2. View Cart");
        printf("\n3. View Sales History");
        printf("\n4. Back to Main Menu");
        printf("\n\nEnter your choice: ");

        scanf("%d", &choice);

        switch (choice) {
            case 1: createNewSale(); break;
            case 2: viewCart(); break;
            case 3: viewSalesHistory(); break;
            case 4: return;
            default:
                RED_COLOR;
                printf("\nInvalid choice! Please try again.\n");
                RESET_COLOR;
                sleep(1);
        }
    }
}

void createNewSale() {
    printHeader("CREATE NEW SALE");

    if (cart_count > 0) {
        char choice;
        printf("\nThere are items in the cart. Clear cart? (y/n): ");
        getchar();
        scanf("%c", &choice);

        if (tolower(choice) == 'y') {
            clearCart();
        }
    }

    while (1) {
        printf("\n1. Add Item to Cart");
        printf("\n2. View Cart");
        printf("\n3. Proceed to Checkout");
        printf("\n4. Cancel Sale");
        printf("\nEnter choice: ");

        int choice;
        scanf("%d", &choice);

        switch (choice) {
            case 1: addToCart(); break;
            case 2: viewCart(); break;
            case 3:
                if (cart_count > 0) {
                    checkout();
                    return;
                } else {
                    YELLOW_COLOR;
                    printf("\nCart is empty!\n");
                    RESET_COLOR;
                    sleep(1);
                }
                break;
            case 4:
                clearCart();
                return;
            default:
                RED_COLOR;
                printf("\nInvalid choice!\n");
                RESET_COLOR;
                sleep(1);
        }
    }
}

void addToCart() {
    int id, quantity;

    printf("\nEnter Product ID: ");
    scanf("%d", &id);

    // Find product
    int product_index = -1;
    for (int i = 0; i < product_count; i++) {
        if (products[i].id == id) {
            product_index = i;
            break;
        }
    }

    if (product_index == -1) {
        RED_COLOR;
        printf("\nProduct not found!\n");
        RESET_COLOR;
        sleep(1);
        return;
    }
    printf("\nProduct Name: %s || Price : %.2f TK || ", products[product_index].name,products[product_index].sale_price);
    printf("Available Stock: %d\n", products[product_index].quantity);

    printf("Enter quantity: ");
    scanf("%d", &quantity);

    if (quantity <= 0) {
        RED_COLOR;
        printf("\nInvalid quantity!\n");
        RESET_COLOR;
        sleep(1);
        return;
    }

    if (quantity > products[product_index].quantity) {
        RED_COLOR;
        printf("\nInsufficient stock! Available: %d\n", products[product_index].quantity);
        RESET_COLOR;
        sleep(1);
        return;
    }

    // Check if product already in cart
    for (int i = 0; i < cart_count; i++) {
        if (current_cart[i].product_id == id) {
            current_cart[i].quantity += quantity;
            GREEN_COLOR;
            printf("\nCart updated successfully!\n");
            RESET_COLOR;
            sleep(1);
            return;
        }
    }

    // Add new item to cart
    if (cart_count >= MAX_CART_ITEMS) {
        RED_COLOR;
        printf("\nCart is full!\n");
        RESET_COLOR;
        sleep(1);
        return;
    }

    current_cart[cart_count].product_id = id;
    current_cart[cart_count].quantity = quantity;
    current_cart[cart_count].price = products[product_index].sale_price;
    cart_count++;

    GREEN_COLOR;
    printf("\nItem added to cart successfully!\n");
    RESET_COLOR;
    sleep(1);
}

void viewCart() {
    printHeader("SHOPPING CART");

    if (cart_count == 0) {
        YELLOW_COLOR;
        printf("\nCart is empty!\n");
        RESET_COLOR;
        printf("\nPress Enter to continue...");
        getchar();
        getchar();
        return;
    }

    float total = 0;
    printf("\nID\tProduct\t\tQuantity\tPrice\tSubtotal\n");
    printLine();

    for (int i = 0; i < cart_count; i++) {
        for (int j = 0; j < product_count; j++) {
            if (products[j].id == current_cart[i].product_id) {
                float subtotal = current_cart[i].quantity * current_cart[i].price;
                printf("%d\t%-16s%-16d%.2f\t%.2f\n",
                       products[j].id,
                       products[j].name,
                       current_cart[i].quantity,
                       current_cart[i].price,
                       subtotal);
                total += subtotal;
                break;
            }
        }
    }

    printLine();
    printf("Total Amount: %.2f TK \n", total);

    printf("\nPress Enter to continue...");
    getchar();
    getchar();
}

void clearCart() {
    cart_count = 0;
    YELLOW_COLOR;
    printf("\nCart cleared!\n");
    RESET_COLOR;
    sleep(1);
}

void updateInventory(Order order) {
    // Update product quantities
    for (int i = 0; i < order.item_count; i++) {
        CartItem item = order.items[i];
        for (int j = 0; j < product_count; j++) {
            if (products[j].id == item.product_id) {
                products[j].quantity -= item.quantity;
                printf("\nUpdated stock for %s: %d", products[j].name, products[j].quantity);
                break;
            }
        }
    }


    FILE* file = fopen("products.txt", "w");
    if (!file) {
        RED_COLOR;
        printf("\nError updating inventory!\n");
        RESET_COLOR;
        return;
    }
    
    for (int i = 0; i < product_count; i++) {
        fprintf(file, "%d,%s,%s,%d,%.2f,%.2f,%s\n",
                products[i].id,
                products[i].name,
                products[i].category,
                products[i].quantity,
                products[i].purchase_price,
                products[i].sale_price,
                products[i].date_added);
    }
    
    fclose(file);
    
    GREEN_COLOR;
    printf("\nInventory updated successfully!\n");
    RESET_COLOR;
}

 

void checkout() {
    char phone[MAX_STRING];
    Order order;
    char confirm;
    
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    order.id = (long)t; 
    strftime(order.date, sizeof(order.date), "%Y-%m-%d %I:%M:%S %p", tm);
    
    // Set employee ID from current user
    order.employee_id = current_user.id;
    
    // Initialize order with cart items
    order.item_count = cart_count;
    for(int i = 0; i < cart_count; i++) {
        order.items[i] = current_cart[i];
    }
    
    printf("\nEnter Customer Phone Number ");
    scanf("%s", phone);
    if (strlen(phone) == 0) {
        strcpy(order.customer_phone, "GUEST");
    } else {
        strcpy(order.customer_phone, phone);
    }
    
    int found = 0;
    for (int i = 0; i < customer_count; i++) {
        if (strcmp(customers[i].phone, phone) == 0) {
            found = 1;
            printf("\nCustomer Found!");
            printf("\nName: %s", customers[i].name);
            printf("\nTotal Previous Purchases: %.2f", customers[i].total_spending);
            if (customers[i].total_spending >= LOYALTY_THRESHOLD) {
                printf("\nLoyalty Customer - Eligible for 10%% discount!\n");
            }
            break;
        }
    }
    
    if (!found) {
        printf("\nCustomer not found! Create new account:");
        Customer newCustomer;
        strcpy(newCustomer.phone, phone);
        
        printf("\nEnter Customer Name: ");
        scanf(" %[^\n]s", newCustomer.name);
        
        printf("Enter Customer Address: ");
        scanf(" %[^\n]s", newCustomer.address);
        
        newCustomer.total_spending = 0;
        newCustomer.loyalty_points = 0;
        newCustomer.last_loyalty_milestone = 0;
        
        customers[customer_count] = newCustomer;
        customer_count++;
        
        // Save updated customer list
        saveCustomers();
        
        GREEN_COLOR;
        printf("\nNew customer account created successfully!\n");
        RESET_COLOR;
    }
    
    printf("\nProceed with checkout? (y/n): ");
    scanf(" %c", &confirm);
    if (tolower(confirm) == 'y') {
        processPayment(&order);
        updateInventory(order);  
        saveTransactionToFile(order);

        updateEmployeeTotalSales(order.employee_id, order.total_amount - order.discount);

        generateReceipt(order);
        
        // Update customer's total spending
        for (int i = 0; i < customer_count; i++) {
            if (strcmp(customers[i].phone, phone) == 0) {
                customers[i].total_spending += order.total_amount;
                break;
            }
        }
        saveCustomers();
        
        clearCart();
        
        GREEN_COLOR;
        printf("\nCheckout completed successfully!\n");
        printf("Order ID: %ld\n", order.id);
        RESET_COLOR;
        sleep(2);
    }
}

void processPayment(Order* order) {
    int choice;
    float total = 0;
    
    // Calculate total from cart
    for (int i = 0; i < cart_count; i++) {
        total += current_cart[i].quantity * current_cart[i].price;
    }
    order->total_amount = total;
    order->discount = 0;
    
    printf("\n=== PAYMENT PROCESSING ===\n");
    printf("\nCurrent Total: %.2f", total);
    
    // Check for Loyalty Milestone Discount
    for (int i = 0; i < customer_count; i++) {
        if (strcmp(customers[i].phone, order->customer_phone) == 0) {
            float current_total = customers[i].total_spending;
            int eligible_milestone = 0;
            
            if (current_total >= LOYALTY_MILESTONE_3 && 
                customers[i].last_loyalty_milestone < LOYALTY_MILESTONE_3) {
                eligible_milestone = LOYALTY_MILESTONE_3;
            }
            else if (current_total >= LOYALTY_MILESTONE_2 && 
                     customers[i].last_loyalty_milestone < LOYALTY_MILESTONE_2) {
                eligible_milestone = LOYALTY_MILESTONE_2;
            }
            else if (current_total >= LOYALTY_MILESTONE_1 && 
                     customers[i].last_loyalty_milestone < LOYALTY_MILESTONE_1) {
                eligible_milestone = LOYALTY_MILESTONE_1;
            }
            
            // Apply loyalty discount if eligible
            if (eligible_milestone > 0) {
                float loyalty_discount = total * 0.10;  // 10% loyalty discount
                order->discount += loyalty_discount;
                customers[i].last_loyalty_milestone = eligible_milestone;
                saveCustomers();  // Save the updated milestone
                
                GREEN_COLOR;
                printf("\nCongratulations! Loyalty Milestone of %.2f reached!", 
                       (float)eligible_milestone);
                printf("\nOne-time Loyalty Discount (10%%): %.2f", loyalty_discount);
                RESET_COLOR;
            }
            break;
        }
    }
    
    // Manual Discount
    char discount_choice;
    printf("\nApply manual discount? (y/n): ");
    scanf(" %c", &discount_choice);
    
    if(tolower(discount_choice) == 'y') {
        do {
            printf("\nEnter discount percentage (1-100): ");
            scanf("%f", &order->manual_discount_percentage);
            if(order->manual_discount_percentage < 0 || order->manual_discount_percentage > 100) {
                RED_COLOR;
                printf("\nInvalid discount percentage! Please enter between 1-100\n");
                RESET_COLOR;
            }
        } while(order->manual_discount_percentage < 0 || order->manual_discount_percentage > 100);
        
        float manual_discount = total * order->manual_discount_percentage / 100;
        order->discount += manual_discount;
        printf("\nManual discount applied: %.2f", manual_discount);
    }
    
    // Payment Method Selection
    printf("\n\n=== SELECT PAYMENT METHOD ===\n");
    printf("\n1. Cash");
    printf("\n2. 1Card (5%% discount)");
    printf("\n3. bKash");
    printf("\n4. Nagad");
    printf("\n5. Bank Transfer");
    printf("\nSelect payment method (1-5): ");
    scanf("%d", &choice);
    
    order->payment_method = choice;
    order->card_discount_percentage = 0;  // Reset card discount
    
    // Apply 1Card discount only if selected
    if(choice == PAYMENT_1CARD) {
        order->card_discount_percentage = 5.0;
        float remaining_amount = total - order->discount;
        float card_discount = remaining_amount * 0.05;
        order->discount += card_discount;
        printf("\n1Card discount applied: %.2f", card_discount);
    }
    
    // Get transaction ID for non-cash payments
    if(choice != PAYMENT_CASH) {
        printf("\nEnter Transaction ID: ");
        scanf("%s", order->transaction_id);
    }
    
    // Update final amount
    order->total_amount = total - order->discount;
    
    // Show payment summary
    printf("\n=== PAYMENT SUMMARY ===\n");
    printf("Original Amount: %.2f TK\n", total);
    
    // Show loyalty discount if applicable
    for (int i = 0; i < customer_count; i++) {
        if (strcmp(customers[i].phone, order->customer_phone) == 0) {
            if (customers[i].total_spending >= LOYALTY_THRESHOLD) {
                printf("Loyalty Discount (10%%): %.2f TK \n", total * 0.10);
            }
            break;
        }
    }
    
    if (order->manual_discount_percentage > 0) {
        printf("Manual Discount (%.0f%%): %.2f\n", 
               order->manual_discount_percentage, 
               total * order->manual_discount_percentage / 100);
    }
    
    if (order->payment_method == PAYMENT_1CARD) {
        float remaining_after_discounts = total - (total * 0.10) - (total * order->manual_discount_percentage / 100);
        printf("1Card Discount (5%%): %.2f TK \n", remaining_after_discounts * 0.05);
    }
    
    printf("Final Amount: %.2f\n", order->total_amount);
    printf("Payment Method: %s\n", 
           choice == PAYMENT_CASH ? "Cash" :
           choice == PAYMENT_1CARD ? "1Card" :
           choice == PAYMENT_BKASH ? "bKash" :
           choice == PAYMENT_NAGAD ? "Nagad" : "Bank Transfer");
    
    printf("\nPress Enter to continue...");
    getchar();
    getchar();
}

void generateReceipt(Order order) {
    char filename[256];
    char html_file[256];
    char print_command[512];
    
    // Create directories and filenames
    #ifdef _WIN32
        CreateDirectory("receipts", NULL);
        sprintf(html_file, "receipts\\receipt_%ld.html", order.id);
        sprintf(filename, "receipts\\receipt_%ld.pdf", order.id);
        sprintf(print_command, "wkhtmltopdf %s %s", html_file, filename);
    #else
        system("mkdir -p receipts");  // -p flag for macOS/Linux
        sprintf(html_file, "receipts/receipt_%ld.html", order.id);  // Forward slash for macOS
        sprintf(filename, "receipts/receipt_%ld.pdf", order.id);
        sprintf(print_command, "wkhtmltopdf %s %s", html_file, filename);
    #endif

    FILE* file = fopen(html_file, "w");
    if (!file) {
        RED_COLOR;
        printf("\nError creating receipt file!\n");
        RESET_COLOR;
        return;
    }
    
    fprintf(file, "<html><head>\n");
    fprintf(file, "<meta charset='UTF-8'>\n");
    fprintf(file, "<style>\n");
    fprintf(file, "body { font-family: Arial, sans-serif; margin: 0; padding: 20px; }\n");
    fprintf(file, ".container { width: 800px; margin: 0 auto; border: 1px solid #00a651; padding: 20px; }\n");
    
    fprintf(file, ".header { position: relative; margin-bottom: 40px; }\n");
    fprintf(file, ".invoice-title { color: #00a651; font-size: 24px; margin-bottom: 10px; }\n");
    fprintf(file, ".company-info { color: #666; font-size: 14px; line-height: 1.5; }\n");
    fprintf(file, ".invoice-number { position: absolute; top: 0; right: 0; background: #00a651; color: white; padding: 10px 20px; }\n");
    
    fprintf(file, ".info-section { display: flex; justify-content: space-between; margin: 30px 0; }\n");
    fprintf(file, ".payment-info { flex: 1; }\n");
    fprintf(file, ".invoice-to { flex: 1; text-align: right; }\n");
    
    fprintf(file, "table { width: 100%%; border-collapse: collapse; margin: 20px 0; }\n");
    fprintf(file, "th { background: #00a651; color: white; padding: 12px 8px; text-align: left; }\n");
    fprintf(file, "td { padding: 10px 8px; border-bottom: 1px solid #ddd; }\n");
    
    fprintf(file, ".footer { display: flex; margin-top: 30px; }\n");
    fprintf(file, ".terms { flex: 1; background: #f9f9f9; padding: 20px; }\n");
    fprintf(file, ".totals { width: 300px; text-align: right; margin-left: auto; }\n");
    fprintf(file, ".signature { margin-top: 100px; text-align: right; }\n");
    fprintf(file, ".signature-line { border-top: 1px solid #000; width: 200px; display: inline-block; text-align: center; padding-top: 5px; }\n");
    fprintf(file, "</style></head><body>\n");
    
    fprintf(file, "<div class='container'>\n");
    
    fprintf(file, "<div class='header'>\n");
    fprintf(file, "  <div class='invoice-title'>Invoice</div>\n");
    fprintf(file, "  <div class='company-info'>\n");
    fprintf(file, "    DIU SUPER SHOP<br>\n");
    fprintf(file, "    Daffodil Smart City, Ashulia, Dhaka<br>\n");
    fprintf(file, "    Phone: +880 1234-567890<br>\n");
    fprintf(file, "    Email: info@diusupershop.com\n");
    fprintf(file, "  </div>\n");
    fprintf(file, "  <div class='invoice-number'>\n");
    fprintf(file, "    Invoice No#<br>%ld\n", order.id);
    fprintf(file, "  </div>\n");
    fprintf(file, "</div>\n");
    
    fprintf(file, "<div class='info-section'>\n");
    fprintf(file, "  <div class='payment-info'>\n");
    fprintf(file, "    <strong>Payment Info</strong><br>\n");
    fprintf(file, "    Invoice ID: %ld<br>\n", order.id);
    fprintf(file, "    Date & Time: %s<br>\n", order.date);
    fprintf(file, "    Payment Method: %s\n",
            order.payment_method == PAYMENT_CASH ? "Cash" :
            order.payment_method == PAYMENT_1CARD ? "1Card" :
            order.payment_method == PAYMENT_BKASH ? "bKash" :
            order.payment_method == PAYMENT_NAGAD ? "Nagad" : "Bank Transfer");
    fprintf(file, "  </div>\n");
    
    fprintf(file, "  <div class='invoice-to'>\n");
    fprintf(file, "    <strong>Invoice To:</strong><br>\n");
    
    int found = 0;
    for (int i = 0; i < customer_count; i++) {
        if (strcmp(customers[i].phone, order.customer_phone) == 0) {
            fprintf(file, "    Name: %s<br>\n", customers[i].name);
            fprintf(file, "    Address: %s<br>\n", customers[i].address);
            fprintf(file, "    Phone: %s<br>\n", customers[i].phone);
            found = 1;
            break;
        }
    }
    
    if (!found) {
        fprintf(file, "    Guest Customer<br>\n");
        fprintf(file, "    Phone: %s<br>\n", order.customer_phone);
    }
    
    fprintf(file, "  </div>\n");
    fprintf(file, "</div>\n");

    // Table
    fprintf(file, "<table>\n");
    fprintf(file, "<tr><th>SL</th><th>Description</th><th>Price</th><th>Quantity</th><th>Total</th></tr>\n");
    
    float subtotal = 0;
    for (int i = 0; i < order.item_count; i++) {
        for (int j = 0; j < product_count; j++) {
            if (products[j].id == order.items[i].product_id) {
                float amount = order.items[i].quantity * order.items[i].price;
                subtotal += amount;
                fprintf(file, "<tr><td>%d</td><td>%s</td><td>%.2f</td><td>%d</td><td>%.2f</td></tr>\n",
                        i + 1,
                        products[j].name,
                        order.items[i].price,
                        order.items[i].quantity,
                        amount);
                break;
            }
        }
    }
    fprintf(file, "</table>\n");
    
    fprintf(file, "<div class='footer'>\n");
    fprintf(file, "  <div class='terms'>\n");
    fprintf(file, "    <strong>Thank you for your business</strong><br><br>\n");
    fprintf(file, "    <strong>Terms & Conditions</strong><br>\n");
    fprintf(file, "    1. All prices include VAT<br>\n");
    fprintf(file, "    2. No refund after purchase<br>\n");
    fprintf(file, "    3. Please keep the receipt for warranty\n");
    fprintf(file, "  </div>\n");
    fprintf(file, "  <div class='totals'>\n");
    fprintf(file, "    Sub Total: %.2f<br>\n", subtotal);
    if (order.manual_discount_percentage > 0) {
        fprintf(file, "    Discount %.0f%%: %.2f<br>\n", 
                order.manual_discount_percentage,
                subtotal * order.manual_discount_percentage / 100);
    }
    fprintf(file, "    <strong>Total Amount: %.2f</strong>\n", order.total_amount);
    fprintf(file, "  </div>\n");
    fprintf(file, "</div>\n");
    
    // Signature
    fprintf(file, "<div class='signature'>\n");
    fprintf(file, "  <div class='signature-line'>Authorized Sign</div>\n");
    fprintf(file, "</div>\n");
    
    fprintf(file, "</div>\n");

  fclose(file);

    // Convert HTML to PDF
    #ifdef _WIN32
        sprintf(print_command, "wkhtmltopdf %s %s", html_file, filename);
    #else
        sprintf(print_command, "wkhtmltopdf %s %s", html_file, filename);
    #endif

    int result = system(print_command);
    
    #ifdef _WIN32
        sprintf(print_command, "start %s", filename);
    #else
        sprintf(print_command, "xdg-open %s", filename);
    #endif
    system(print_command);

    #ifdef _WIN32
        sprintf(print_command, "del %s", html_file);
    #else
        sprintf(print_command, "rm %s", html_file);
    #endif
    system(print_command);

    printf("\nReceipt generated: %s\n", filename);
}

// Customer Management Functions
void customerMenu() {
    int choice;

    while (1) {
        printHeader("CUSTOMER MANAGEMENT");
        printf("\n1. Add New Customer");
        printf("\n2. View Customer List");
        printf("\n3. View Customer Details with History");
        printf("\n4. Search Customer");
        printf("\n5. Update Customer");
        printf("\n6. Back to Main Menu");
        printf("\n\nEnter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addCustomer(); break;
            case 2: viewCustomerList(); break;
            case 3: viewCustomerDetails(); break;
            case 4: searchCustomer(); break;
            case 5: updateCustomer(); break;
            case 6: return;
            default:
                RED_COLOR;
                printf("\nInvalid choice!\n");
                RESET_COLOR;
                sleep(1);
        }
    }
}

void addCustomer() {
    Customer new_customer;
    printHeader("ADD NEW CUSTOMER");

    printf("\nEnter customer details:\n");
    printf("Phone Number: ");
    scanf("%s", new_customer.phone);
    getchar(); 

    // Check if phone number already exists
    for (int i = 0; i < customer_count; i++) {
        if (strcmp(customers[i].phone, new_customer.phone) == 0) {
            RED_COLOR;
            printf("\nCustomer with this phone number already exists!\n");
            RESET_COLOR;
            sleep(2);
            return;
        }
    }

    printf("Name: ");
    fgets(new_customer.name, MAX_STRING, stdin);
    new_customer.name[strcspn(new_customer.name, "\n")] = 0;

    printf("Address: ");
    fgets(new_customer.address, MAX_STRING, stdin);
    new_customer.address[strcspn(new_customer.address, "\n")] = 0;

    new_customer.total_spending = 0;
    new_customer.loyalty_points = 0;

    customers[customer_count++] = new_customer;
    saveCustomers();

    GREEN_COLOR;
    printf("\nCustomer added successfully!\n");
    RESET_COLOR;
    sleep(2);
}


void viewCustomerList() {
    printHeader("CUSTOMER LIST");

    if (customer_count == 0) {
        YELLOW_COLOR;
        printf("\nNo customers registered!\n");
        RESET_COLOR;
        sleep(2);
        return;
    }

    printf("\n%-15s %-20s %-15s %-15s\n", 
           "Phone", "Name", "Total Spend", "Loyalty Points");
    printLine();

    for (int i = 0; i < customer_count; i++) {
        printf("%-15s %-20s %-15.2f %-15d\n",
               customers[i].phone,
               customers[i].name,
               customers[i].total_spending,
               customers[i].loyalty_points);
    }

    printf("\nExport Options:\n");
    printf("1. Generate PDF\n");
    printf("2. Generate CSV\n");
    printf("3. Back\n");
    printf("\nEnter your choice: ");

    int choice;
    scanf("%d", &choice);

    switch(choice) {
        case 1:
            generateCustomerListPDF();
            break;
        case 2:
            generateCustomerListCSV();
            break;
        case 3:
            return;
    }

    printf("\nPress Enter to continue...");
    getchar();
    getchar();
}

void generateCustomerListPDF() {
    char filename[256];
    char temp_html[256];
    char command[512];
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    char date_str[20];
    strftime(date_str, sizeof(date_str), "%Y%m%d_%H%M%S", tm);
    
    #ifdef _WIN32
        CreateDirectory("reports", NULL);
        sprintf(temp_html, "temp_%s.html", date_str);
        sprintf(filename, "reports\\customer_list_%s.pdf", date_str);
        sprintf(command, "wkhtmltopdf %s %s && del %s", temp_html, filename, temp_html);
    #else
        system("mkdir -p reports");
        sprintf(temp_html, "temp_%s.html", date_str);
        sprintf(filename, "reports/customer_list_%s.pdf", date_str);
        sprintf(command, "wkhtmltopdf %s %s && rm %s", temp_html, filename, temp_html);
    #endif

    FILE* file = fopen(temp_html, "w");
    if (!file) return;

    fprintf(file, "<html><head><style>\n");
    fprintf(file, "body { font-family: Arial; padding: 20px; }\n");
    fprintf(file, "table { width: 100%%; border-collapse: collapse; }\n");
    fprintf(file, "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n");
    fprintf(file, "th { background: #4CAF50; color: white; }\n");
    fprintf(file, "</style></head><body>\n");
    
    fprintf(file, "<h1>Customer List</h1>\n");
    fprintf(file, "<p>Generated on: %s</p>\n", getCurrentDateTime());
    
    fprintf(file, "<table>\n");
    fprintf(file, "<tr><th>Phone</th><th>Name</th><th>Total Spending</th><th>Loyalty Points</th></tr>\n");

    for (int i = 0; i < customer_count; i++) {
        fprintf(file, "<tr><td>%s</td><td>%s</td><td>%.2f</td><td>%d</td></tr>\n",
                customers[i].phone,
                customers[i].name,
                customers[i].total_spending,
                customers[i].loyalty_points);
    }

    fprintf(file, "</table></body></html>\n");
    fclose(file);

    system(command);

    GREEN_COLOR;
    printf("\nCustomer List PDF generated: %s\n", filename);
    RESET_COLOR;
}

void generateCustomerListCSV() {
    char filename[256];
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    char date_str[20];
    strftime(date_str, sizeof(date_str), "%Y%m%d_%H%M%S", tm);
    
    #ifdef _WIN32
        CreateDirectory("reports", NULL);
        sprintf(filename, "reports\\customer_list_%s.csv", date_str);
    #else
        system("mkdir -p reports");
        sprintf(filename, "reports/customer_list_%s.csv", date_str);
    #endif

    FILE* file = fopen(filename, "w");
    if (!file) return;

    fprintf(file, "Customer List\n");
    fprintf(file, "Generated on: %s\n\n", getCurrentDateTime());
    fprintf(file, "Phone,Name,Total Spending,Loyalty Points\n");

    for (int i = 0; i < customer_count; i++) {
        fprintf(file, "%s,%s,%.2f,%d\n",
                customers[i].phone,
                customers[i].name,
                customers[i].total_spending,
                customers[i].loyalty_points);
    }

    fclose(file);
    GREEN_COLOR;
    printf("\nCustomer List CSV generated: %s\n", filename);
    RESET_COLOR;
}
void viewCustomerDetails() {
    printHeader("CUSTOMER DETAILS WITH HISTORY");

    if (customer_count == 0) {
        YELLOW_COLOR;
        printf("\nNo customers registered!\n");
        RESET_COLOR;
        sleep(2);
        return;
    }

    printf("\n%-15s %-20s %-20s %-15s %-15s %s\n", 
           "Phone", "Name", "Address", "Total Spend", "Loyalty Points", "Purchase History");
    printLine();

    for (int i = 0; i < customer_count; i++) {
        printf("\n%-15s %-20s %-20s %-15.2f %-15d\n",
               customers[i].phone,
               customers[i].name,
               customers[i].address,
               customers[i].total_spending,
               customers[i].loyalty_points);

        printf("\nRecent Purchases:\n");
        printf("%-20s %-10s %-10s %-15s %-10s\n", 
               "Date", "Order ID", "Items", "Amount", "Discount");
        
        FILE* sales_file = fopen("sales.txt", "r");
        if (sales_file) {
            Order order;
            while (fscanf(sales_file, "%ld,%[^,],%d,%[^,],%f,%f\n",
                       &order.id,
                       order.customer_phone,
                       &order.employee_id,
                       order.date,
                       &order.total_amount,
                       &order.discount) != EOF) {
                
                if (strcmp(order.customer_phone, customers[i].phone) == 0) {
                    int items_count = 0;
                    FILE* items_file = fopen("sales_items.txt", "r");
                    if (items_file) {
                        int order_id, product_id, quantity;
                        float price;
                        while (fscanf(items_file, "%d,%d,%d,%f\n", 
                                    &order_id, &product_id, &quantity, &price) != EOF) {
                            if (order_id == order.id) {
                                items_count += quantity;
                            }
                        }
                        fclose(items_file);
                    }
                    
                    printf("%-20s %-10ld %-10d %-15.2f %-10.2f\n",
                           order.date,
                           order.id,
                           items_count,
                           order.total_amount,
                           order.discount);
                }
            }
            fclose(sales_file);
        }
        printLine();
    }

    printf("\nExport Options:\n");
    printf("1. Generate Detailed PDF Report\n");
    printf("2. Generate Detailed CSV Report\n");
    printf("3. Back\n");
    printf("\nEnter your choice: ");

    int choice;
    scanf("%d", &choice);

    switch(choice) {
        case 1:
            generateDetailedCustomerListPDF();
            break;
        case 2:
            generateDetailedCustomerListCSV();
            break;
        case 3:
            return;
        default:
            RED_COLOR;
            printf("\nInvalid choice!\n");
            RESET_COLOR;
            sleep(1);
    }

    printf("\nPress Enter to continue...");
    getchar();
    getchar();
}
 

void generateDetailedCustomerListPDF() {
    char filename[256];
    char command[512];
    char temp_html[256];
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    char date_str[20];
    strftime(date_str, sizeof(date_str), "%Y%m%d_%H%M%S", tm);
    
    #ifdef _WIN32
        CreateDirectory("reports", NULL);
        sprintf(temp_html, "temp_%s.html", date_str);
        sprintf(filename, "reports\\customer_details_%s.pdf", date_str);
        sprintf(command, "wkhtmltopdf %s %s && del %s", temp_html, filename, temp_html);
    #else
        system("mkdir -p reports");
        sprintf(temp_html, "temp_%s.html", date_str);
        sprintf(filename, "reports/customer_list_%s.pdf", date_str);
        sprintf(command, "wkhtmltopdf %s %s && rm %s", temp_html, filename, temp_html);
    #endif

    FILE* file = fopen(temp_html, "w");
    if (!file) return;

    fprintf(file, "<html><head><style>\n");
    fprintf(file, "body { font-family: Arial; padding: 20px; }\n");
    fprintf(file, "table { width: 100%%; border-collapse: collapse; margin: 10px 0; }\n");
    fprintf(file, "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n");
    fprintf(file, "th { background: #4CAF50; color: white; }\n");
    fprintf(file, ".customer-card { border: 1px solid #ddd; margin: 20px 0; padding: 15px; }\n");
    fprintf(file, ".purchase-history { margin-top: 10px; }\n");
    fprintf(file, "</style></head><body>\n");
    
    fprintf(file, "<h1>Detailed Customer Report</h1>\n");
    fprintf(file, "<p>Generated on: %s</p>\n", getCurrentDateTime());

    for (int i = 0; i < customer_count; i++) {
        fprintf(file, "<div class='customer-card'>\n");
        fprintf(file, "<h2>Customer Details</h2>\n");
        fprintf(file, "<table>\n");
        fprintf(file, "<tr><th>Phone</th><td>%s</td></tr>\n", customers[i].phone);
        fprintf(file, "<tr><th>Name</th><td>%s</td></tr>\n", customers[i].name);
        fprintf(file, "<tr><th>Address</th><td>%s</td></tr>\n", customers[i].address);
        fprintf(file, "<tr><th>Total Spending</th><td>%.2f</td></tr>\n", customers[i].total_spending);
        fprintf(file, "<tr><th>Loyalty Points</th><td>%d</td></tr>\n", customers[i].loyalty_points);
        fprintf(file, "</table>\n");

        fprintf(file, "<div class='purchase-history'>\n");
        fprintf(file, "<h3>Purchase History</h3>\n");
        fprintf(file, "<table>\n");
        fprintf(file, "<tr><th>Date</th><th>Order ID</th><th>Items</th><th>Amount</th><th>Discount</th><th>Net Amount</th></tr>\n");

        FILE* sales_file = fopen("sales.txt", "r");
        if (sales_file) {
            Order order;
            while (fscanf(sales_file, "%ld,%[^,],%d,%[^,],%f,%f\n",
                       &order.id, order.customer_phone, &order.employee_id,
                       order.date, &order.total_amount, &order.discount) != EOF) {
                
                if (strcmp(order.customer_phone, customers[i].phone) == 0) {
                    int items_count = 0;
                    FILE* items_file = fopen("sales_items.txt", "r");
                    if (items_file) {
                        int order_id, product_id, quantity;
                        float price;
                        while (fscanf(items_file, "%d,%d,%d,%f\n", 
                                    &order_id, &product_id, &quantity, &price) != EOF) {
                            if (order_id == order.id) {
                                items_count += quantity;
                            }
                        }
                        fclose(items_file);
                    }
                    
                    fprintf(file, "<tr><td>%s</td><td>%ld</td><td>%d</td><td>%.2f</td><td>%.2f</td><td>%.2f</td></tr>\n",
                            order.date, order.id, items_count, 
                            order.total_amount, order.discount,
                            order.total_amount - order.discount);
                }
            }
            fclose(sales_file);
        }
        fprintf(file, "</table></div></div>\n");
    }

    fprintf(file, "</body></html>\n");
    fclose(file);

    sprintf(command, "wkhtmltopdf %s %s && rm %s", temp_html, filename, temp_html);
    system(command);

    GREEN_COLOR;
    printf("\nDetailed PDF Report generated: %s\n", filename);
    RESET_COLOR;
}
void generateDetailedCustomerListCSV() {
    char filename[256];
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    char date_str[20];
    strftime(date_str, sizeof(date_str), "%Y%m%d_%H%M%S", tm);
    
    #ifdef _WIN32
        sprintf(filename, "reports\\customer_details_%s.csv", date_str);
    #else
        sprintf(filename, "reports/customer_details_%s.csv", date_str);
    #endif

    FILE* file = fopen(filename, "w");
    if (!file) {
        RED_COLOR;
        printf("\nError creating CSV file!\n");
        RESET_COLOR;
        return;
    }

    fprintf(file, "CUSTOMER DETAILS REPORT\n");
    fprintf(file, "Generated on: %s\n\n", getCurrentDateTime());

    for (int i = 0; i < customer_count; i++) {
        fprintf(file, "Customer Information\n");
        fprintf(file, "Phone,%s\n", customers[i].phone);
        fprintf(file, "Name,%s\n", customers[i].name);
        fprintf(file, "Address,%s\n", customers[i].address);
        fprintf(file, "Total Spending,%.2f\n", customers[i].total_spending);
        fprintf(file, "Loyalty Points,%d\n\n", customers[i].loyalty_points);

        fprintf(file, "Purchase History\n");
        fprintf(file, "Date,Order ID,Items,Amount,Discount,Net Amount\n");

        FILE* sales_file = fopen("sales.txt", "r");
        if (sales_file) {
            Order order;
            while (fscanf(sales_file, "%ld,%[^,],%d,%[^,],%f,%f\n",
                       &order.id, order.customer_phone, &order.employee_id,
                       order.date, &order.total_amount, &order.discount) != EOF) {
                
                if (strcmp(order.customer_phone, customers[i].phone) == 0) {
                    int items_count = 0;
                    FILE* items_file = fopen("sales_items.txt", "r");
                    if (items_file) {
                        int order_id, product_id, quantity;
                        float price;
                        while (fscanf(items_file, "%d,%d,%d,%f\n", 
                                    &order_id, &product_id, &quantity, &price) != EOF) {
                            if (order_id == order.id) {
                                items_count += quantity;
                            }
                        }
                        fclose(items_file);
                    }
                    
                    fprintf(file, "%s,%ld,%d,%.2f,%.2f,%.2f\n",
                            order.date, order.id, items_count, 
                            order.total_amount, order.discount,
                            order.total_amount - order.discount);
                }
            }
            fclose(sales_file);
        }
        fprintf(file, "\n\n"); 
    }

    fclose(file);
    GREEN_COLOR;
    printf("\nDetailed CSV Report generated: %s\n", filename);
    RESET_COLOR;
}
void searchCustomer() {
    char search_term[MAX_STRING];
    int found = 0;
    
    printHeader("SEARCH CUSTOMER");
    
    printf("\nEnter customer name or phone number: ");
    getchar();
    fgets(search_term, MAX_STRING, stdin);
    search_term[strcspn(search_term, "\n")] = 0;
    
    for (int i = 0; i < customer_count; i++) {
        if (strstr(customers[i].name, search_term) || 
            strstr(customers[i].phone, search_term)) {
             found = 1;
            printf("\nCustomer Details:");
            printf("\nPhone: %s", customers[i].phone);
            printf("\nName: %s", customers[i].name);
            printf("\nAddress: %s", customers[i].address);
            printf("\nTotal Spending: %.2f", customers[i].total_spending);
            printf("\nLoyalty Points: %d\n", customers[i].loyalty_points);
            printLine();

            printf("\nPurchase History:\n");
            printf("%-20s %-10s %-10s %-15s %-10s %-10s\n", 
                   "Date", "Order ID", "Items", "Amount", "Discount", "Net Amount");
            printLine();
            
            FILE* sales_file = fopen("sales.txt", "r");
            if (sales_file) {
                Order order;
                while (fscanf(sales_file, "%ld,%[^,],%d,%[^,],%f,%f\n",
                           &order.id,
                           order.customer_phone,
                           &order.employee_id,
                           order.date,
                           &order.total_amount,
                           &order.discount) != EOF) {
                    
                    if (strcmp(order.customer_phone, search_term) == 0) {
                        int items_count = 0;
                        FILE* items_file = fopen("sales_items.txt", "r");
                        if (items_file) {
                            int order_id, product_id, quantity;
                            float price;
                            while (fscanf(items_file, "%d,%d,%d,%f\n", 
                                        &order_id, &product_id, &quantity, &price) != EOF) {
                                if (order_id == order.id) {
                                    items_count += quantity;
                                }
                            }
                            fclose(items_file);
                        }
                        
                        printf("%-20s %-10ld %-10d %-15.2f %-10.2f %-10.2f\n",
                               order.date,
                               order.id,
                               items_count,
                               order.total_amount,
                               order.discount,
                               order.total_amount - order.discount);
                    }
                }
                fclose(sales_file);
            }
            
            printf("\nExport Options:\n");
            printf("1. Generate PDF Report\n");
            printf("2. Back\n");
            printf("\nEnter your choice: ");
            
            int choice;
            scanf("%d", &choice);
            
            switch(choice) {
                case 1:
                    generateCustomerSearchPDF(search_term);
                    break;
             
                case 2:
                    return;
                default:
                    RED_COLOR;
                    printf("\nInvalid choice!\n");
                    RESET_COLOR;
                    sleep(1);
            }
            break;  
        }
    }
    
    if (!found) {
        RED_COLOR;
        printf("\nNo customer found with this phone number!\n");
        RESET_COLOR;
    }
    
    printf("\nPress Enter to continue...");
    getchar();
    getchar();
}

void generateCustomerSearchPDF(const char* phone) {
    char filename[256];
    char temp_html[256];
    char command[512];
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    char date_str[20];
    strftime(date_str, sizeof(date_str), "%Y%m%d_%H%M%S", tm);
    
    #ifdef _WIN32
        CreateDirectory("reports", NULL);
        sprintf(temp_html, "temp_%s.html", date_str);
        sprintf(filename, "reports\\customer_search_%s.pdf", date_str);
        sprintf(command, "wkhtmltopdf %s %s && del %s", temp_html, filename, temp_html);
    #else
        system("mkdir -p reports");
        sprintf(temp_html, "temp_%s.html", date_str);
        sprintf(filename, "reports/customer_search_%s.pdf", date_str);
        sprintf(command, "wkhtmltopdf %s %s && rm %s", temp_html, filename, temp_html);
    #endif

    FILE* file = fopen(temp_html, "w");
    if (!file) return;

    // Write HTML content
    fprintf(file, "<html><head><style>\n");
    fprintf(file, "body { font-family: Arial; padding: 20px; }\n");
    fprintf(file, "table { width: 100%%; border-collapse: collapse; margin: 20px 0; }\n");
    fprintf(file, "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n");
    fprintf(file, "th { background: #4CAF50; color: white; }\n");
    fprintf(file, ".customer-info { margin-bottom: 30px; }\n");
    fprintf(file, "</style></head><body>\n");

    // Find customer
    for (int i = 0; i < customer_count; i++) {
        if (strcmp(customers[i].phone, phone) == 0) {
            // Customer details
            fprintf(file, "<div class='customer-info'>\n");
            fprintf(file, "<h2>Customer Details</h2>\n");
            fprintf(file, "<p><strong>Phone:</strong> %s</p>\n", customers[i].phone);
            fprintf(file, "<p><strong>Name:</strong> %s</p>\n", customers[i].name);
            fprintf(file, "<p><strong>Address:</strong> %s</p>\n", customers[i].address);
            fprintf(file, "<p><strong>Total Spending:</strong> %.2f</p>\n", customers[i].total_spending);
            fprintf(file, "<p><strong>Loyalty Points:</strong> %d</p>\n", customers[i].loyalty_points);
            fprintf(file, "</div>\n");

            // Purchase history
            fprintf(file, "<h2>Purchase History</h2>\n");
            fprintf(file, "<table>\n");
            fprintf(file, "<tr><th>Date</th><th>Order ID</th><th>Items</th><th>Amount</th><th>Discount</th><th>Net Amount</th></tr>\n");

            FILE* sales_file = fopen("sales.txt", "r");
            if (sales_file) {
                Order order;
                while (fscanf(sales_file, "%ld,%[^,],%d,%[^,],%f,%f\n",
                           &order.id, order.customer_phone, &order.employee_id,
                           order.date, &order.total_amount, &order.discount) != EOF) {
                    
                    if (strcmp(order.customer_phone, phone) == 0) {
                        int items_count = 0;
                        FILE* items_file = fopen("sales_items.txt", "r");
                        if (items_file) {
                            int order_id, product_id, quantity;
                            float price;
                            while (fscanf(items_file, "%d,%d,%d,%f\n", 
                                        &order_id, &product_id, &quantity, &price) != EOF) {
                                if (order_id == order.id) {
                                    items_count += quantity;
                                }
                            }
                            fclose(items_file);
                        }
                        
                        fprintf(file, "<tr><td>%s</td><td>%ld</td><td>%d</td><td>%.2f</td><td>%.2f</td><td>%.2f</td></tr>\n",
                                order.date, order.id, items_count, 
                                order.total_amount, order.discount,
                                order.total_amount - order.discount);
                    }
                }
                fclose(sales_file);
            }
            fprintf(file, "</table>\n");
            break;
        }
    }

    fprintf(file, "</body></html>\n");
    fclose(file);

    system(command);

    GREEN_COLOR;
    printf("\nCustomer Search PDF generated: %s\n", filename);
    RESET_COLOR;
}

// Report Functions
void reportMenu() {
    int choice;

    while (1) {
        printHeader("REPORTS");
        printf("\n1. Daily Sales Report");
        printf("\n2. Monthly Sales Report");
        printf("\n3. Employee Sales Report");
        printf("\n4. Profit Report");
        printf("\n5. Back to Main Menu");
        printf("\n\nEnter your choice: ");

        scanf("%d", &choice);

        switch (choice) {
            case 1: 
                dailySalesReport();
                exportOptions("daily_sales");
                break;
            case 2: 
                monthlySalesReport();
                exportOptions("monthly_sales");
                break;
            case 3: 
                employeeSalesReport();
                exportOptions("employee_sales");
                break;
            case 4: 
                profitReport();
                exportOptions("profit");
                break;
            case 5: return;
            default:
                RED_COLOR;
                printf("\nInvalid choice! Please try again.\n");
                RESET_COLOR;
                sleep(1);
        }
    }
}

void exportOptions(const char* report_type) {
    int choice;
    printf("\nExport Options:\n");
    printf("1. Generate PDF\n");
    printf("2. Generate CSV\n");
    printf("3. Skip\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch(choice) {
        case 1:
            generateReportPDF(report_type);
            break;
        case 2:
            generateReportCSV(report_type);
            break;
        case 3:
            return;
        default:
            printf("Invalid choice!\n");
    }
}

void generateReportPDF(const char* report_type) {
    char filename[256];
    char temp_html[256];
    char print_command[512];
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    char date_str[20];
    strftime(date_str, sizeof(date_str), "%Y%m%d_%H%M%S", tm);
    
    #ifdef _WIN32
        CreateDirectory("reports", NULL);
        sprintf(temp_html, "temp_%s.html", date_str);
        sprintf(filename, "reports\\%s_%s.pdf", report_type, date_str);
    #else
        system("mkdir -p reports"); 
        sprintf(temp_html, "temp_%s.html", date_str);
        sprintf(filename, "reports/%s_%s.pdf", report_type, date_str); 
    #endif

    FILE* file = fopen(temp_html, "w");
    if (!file) {
        RED_COLOR;
        printf("\nError creating temporary file!\n");
        RESET_COLOR;
        return;
    }

    fprintf(file, "<html><head>\n");
    fprintf(file, "<meta charset='UTF-8'>\n");
    fprintf(file, "<style>\n");
    fprintf(file, "body { font-family: Arial, sans-serif; margin: 20px; }\n");
    fprintf(file, "table { width: 100%%; border-collapse: collapse; margin: 20px 0; }\n");
    fprintf(file, "th, td { padding: 10px; border: 1px solid #ddd; text-align: left; }\n");
    fprintf(file, "th { background-color: #00a651; color: white; }\n");
    fprintf(file, ".summary { margin: 20px 0; padding: 20px; background: #f9f9f9; }\n");
    fprintf(file, ".header { text-align: center; margin-bottom: 30px; }\n");
    fprintf(file, ".header h1 { color: #00a651; }\n");
    fprintf(file, "</style></head><body>\n");

    fprintf(file, "<div class='header'>\n");
    fprintf(file, "<h1>DIU SUPER SHOP</h1>\n");
    fprintf(file, "<p>Generated on: %s</p>\n", getCurrentDateTime());
    fprintf(file, "</div>\n");

    if (strcmp(report_type, "profit") == 0) {
        generateProfitReportContent(file);
    } else if (strcmp(report_type, "daily_sales") == 0) {
        generateDailySalesReportContent(file);
    } else if (strcmp(report_type, "monthly_sales") == 0) {
        generateMonthlySalesReportContent(file);
    } else if (strcmp(report_type, "employee_sales") == 0) {
        generateEmployeeSalesReportContent(file);
    }

    fprintf(file, "</body></html>\n");
    fclose(file);

    #ifdef _WIN32
        sprintf(print_command, "wkhtmltopdf %s %s && del %s", temp_html, filename, temp_html);
    #else
        sprintf(print_command, "wkhtmltopdf %s %s && rm %s", temp_html, filename, temp_html);
    #endif
    
    if (system(print_command) == 0) {
        GREEN_COLOR;
        printf("\nPDF Report generated: %s\n", filename);
        RESET_COLOR;
    } else {
        RED_COLOR;
        printf("\nError generating PDF. Please make sure wkhtmltopdf is installed.\n");
        RESET_COLOR;
    }
}

void generateReportCSV(const char* report_type) {
    char filename[256];
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    char date_str[20];
    strftime(date_str, sizeof(date_str), "%Y%m%d_%H%M%S", tm);
    
    #ifdef _WIN32
        CreateDirectory("reports", NULL);
        sprintf(filename, "reports\\%s_%s.csv", report_type, date_str);
    #else
        system("mkdir -p reports");
        sprintf(filename, "reports/%s_%s.csv", report_type, date_str);
    #endif

    FILE* file = fopen(filename, "w");
    if (!file) {
        RED_COLOR;
        printf("\nError creating CSV file!\n");
        RESET_COLOR;
        return;
    }

    if (strcmp(report_type, "profit") == 0) {
        generateProfitReportCSV(file);
    } else if (strcmp(report_type, "daily_sales") == 0) {
        generateDailySalesReportCSV(file);
    } else if (strcmp(report_type, "monthly_sales") == 0) {
        generateMonthlySalesReportCSV(file);
    } else if (strcmp(report_type, "employee_sales") == 0) {
        generateEmployeeSalesReportCSV(file);
    }

    fclose(file);
    GREEN_COLOR;
    printf("\nCSV Report generated: %s\n", filename);
    RESET_COLOR;
}

void generateProfitReportContent(FILE* file) {
    fprintf(file, "<h2>Profit Report</h2>\n");
    fprintf(file, "<table>\n");
    fprintf(file, "<tr><th>Product Name</th><th>Qty Sold</th><th>Revenue</th><th>Cost</th><th>Profit</th></tr>\n");

    float total_revenue = 0, total_cost = 0, total_profit = 0;
    struct {
        int product_id;
        int qty_sold;
        float revenue;
        float cost;
    } product_summary[MAX_PRODUCTS] = {0};

    // Calculate product summaries
    FILE* sales_file = fopen("sales_items.txt", "r");
    if (sales_file) {
        int order_id, product_id, quantity;
        float price;
        
        while (fscanf(sales_file, "%d,%d,%d,%f\n", 
                     &order_id, &product_id, &quantity, &price) != EOF) {
            for (int i = 0; i < product_count; i++) {
                if (products[i].id == product_id) {
                    product_summary[i].product_id = product_id;
                    product_summary[i].qty_sold += quantity;
                    product_summary[i].revenue += quantity * price;
                    product_summary[i].cost += quantity * products[i].purchase_price;
                    break;
                }
            }
        }
        fclose(sales_file);
    }

    for (int i = 0; i < product_count; i++) {
        if (product_summary[i].qty_sold > 0) {
            float profit = product_summary[i].revenue - product_summary[i].cost;
            fprintf(file, "<tr><td>%s</td><td>%d</td><td>%.2f</td><td>%.2f</td><td>%.2f</td></tr>\n",
                   products[i].name,
                   product_summary[i].qty_sold,
                   product_summary[i].revenue,
                   product_summary[i].cost,
                   profit);
            
            total_revenue += product_summary[i].revenue;
            total_cost += product_summary[i].cost;
            total_profit += profit;
        }
    }

    fprintf(file, "</table>\n");
    fprintf(file, "<div class='summary'>\n");
    fprintf(file, "<h3>Overall Summary</h3>\n");
    fprintf(file, "<p><strong>Total Revenue:</strong> %.2f</p>\n", total_revenue);
    fprintf(file, "<p><strong>Total Cost:</strong> %.2f</p>\n", total_cost);
    fprintf(file, "<p><strong>Total Profit:</strong> %.2f</p>\n", total_profit);
    fprintf(file, "<p><strong>Profit Margin:</strong> %.2f%%</p>\n", 
            (total_revenue > 0) ? (total_profit/total_revenue)*100 : 0);
    fprintf(file, "</div>\n");
}

void generateProfitReportCSV(FILE* file) {
    fprintf(file, "Product Name,Qty Sold,Revenue,Cost,Profit\n");

    float total_revenue = 0, total_cost = 0, total_profit = 0;
    struct {
        int product_id;
        int qty_sold;
        float revenue;
        float cost;
    } product_summary[MAX_PRODUCTS] = {0};

    FILE* sales_file = fopen("sales_items.txt", "r");
    if (sales_file) {
        int order_id, product_id, quantity;
        float price;
        
        while (fscanf(sales_file, "%d,%d,%d,%f\n", 
                     &order_id, &product_id, &quantity, &price) != EOF) {
            for (int i = 0; i < product_count; i++) {
                if (products[i].id == product_id) {
                    product_summary[i].product_id = product_id;
                    product_summary[i].qty_sold += quantity;
                    product_summary[i].revenue += quantity * price;
                    product_summary[i].cost += quantity * products[i].purchase_price;
                    break;
                }
            }
        }
        fclose(sales_file);
    }

    for (int i =0; i < product_count; i++) {
        if (product_summary[i].qty_sold > 0) {
            float profit = product_summary[i].revenue - product_summary[i].cost;
            fprintf(file, "%s,%d,%.2f,%.2f,%.2f\n",
                   products[i].name,
                   product_summary[i].qty_sold,
                   product_summary[i].revenue,
                   product_summary[i].cost,
                   profit);
            
            total_revenue += product_summary[i].revenue;
            total_cost += product_summary[i].cost;
            total_profit += profit;
        }
    }

    fprintf(file, "\nOverall Summary\n");
    fprintf(file, "Total Revenue,%.2f\n", total_revenue);
    fprintf(file, "Total Cost,%.2f\n", total_cost);
    fprintf(file, "Total Profit,%.2f\n", total_profit);
    fprintf(file, "Profit Margin,%.2f%%\n", 
            (total_revenue > 0) ? (total_profit/total_revenue)*100 : 0);
}

void dailySalesReport() {
    char date[MAX_STRING];
    float total_sales = 0;
    int total_orders = 0;

    printHeader("DAILY SALES REPORT");

    printf("\nEnter date (YYYY-MM-DD): ");
    scanf("%s", date);

    FILE* file = fopen("sales.txt", "r");
    if (!file) {
        RED_COLOR;
        printf("\nNo sales records found!\n");
        RESET_COLOR;
        sleep(2);
        return;
    }

    Order order;
    printf("\nOrders for %s:\n", date);
    printLine();

    while (fscanf(file, "%ld,%[^,],%d,%[^,],%f,%f\n",
                  &order.id,
                  order.customer_phone,
                  &order.employee_id,
                  order.date,
                  &order.total_amount,
                  &order.discount) != EOF) {
        if (strstr(order.date, date)) {
            printf("\nOrder ID: %ld", order.id);
            printf("\nAmount: %.2f", order.total_amount);
            printf("\nDiscount: %.2f\n", order.discount);
            total_sales += order.total_amount;
            total_orders++;
        }
    }

    fclose(file);

    printLine();
    printf("\nSummary for %s:", date);
    printf("\nTotal Orders: %d", total_orders);
    printf("\nTotal Sales: %.2f\n", total_sales);

    printf("\nPress Enter to continue...");
    getchar();
    getchar();
}

void monthlySalesReport() {
    char month[8];
    char input[10];
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    
    printHeader("MONTHLY SALES REPORT");
    
    printf("\nEnter month (YYYY-MM): ");
    scanf("%s", input);
    strncpy(month, input, 7);
    month[7] = '\0';

    struct {
        char date[11];
        int orders;
        int items;
        float sales;
        float discount;
    } daily_summary[31] = {0};
    int days_count = 0;

    float monthly_total = 0, monthly_discount = 0;
    int monthly_orders = 0, monthly_items = 0;
    
    FILE* sales_file = fopen("sales.txt", "r");
    if (!sales_file) {
        RED_COLOR;
        printf("\nNo sales records found!\n");
        RESET_COLOR;
        return;
    }

    Order order;
    while (fscanf(sales_file, "%ld,%[^,],%d,%[^,],%f,%f\n",
                  &order.id,
                  order.customer_phone,
                  &order.employee_id,
                  order.date,
                  &order.total_amount,
                  &order.discount) != EOF) {
        
        if (strncmp(order.date, month, 7) == 0) {
            char date_str[11];
            strncpy(date_str, order.date, 10);
            date_str[10] = '\0';

            int day_index = -1;
            for (int i = 0; i < days_count; i++) {
                if (strcmp(daily_summary[i].date, date_str) == 0) {
                    day_index = i;
                    break;
                }
            }
            if (day_index == -1) {
                day_index = days_count++;
                strcpy(daily_summary[day_index].date, date_str);
            }

            int items_count = 0;
            FILE* items_file = fopen("sales_items.txt", "r");
            if (items_file) {
                int order_id, product_id, quantity;
                float price;
                while (fscanf(items_file, "%d,%d,%d,%f\n", 
                            &order_id, &product_id, &quantity, &price) != EOF) {
                    if (order_id == order.id) {
                        items_count += quantity;
                    }
                }
                fclose(items_file);
            }

            daily_summary[day_index].orders++;
            daily_summary[day_index].items += items_count;
            daily_summary[day_index].sales += order.total_amount;
            daily_summary[day_index].discount += order.discount;

            monthly_orders++;
            monthly_items += items_count;
            monthly_total += order.total_amount;
            monthly_discount += order.discount;
        }
    }
    fclose(sales_file);

    printf("\nSales Summary for %s:\n", month);
    printLine();
    printf("\nDaily Breakdown:\n");
    printf("%-12s%-10s%-12s%-15s%-15s%-15s\n", 
           "Date", "Orders", "Items", "Sales", "Discount", "Net Sales");
    printLine();

    for (int i = 0; i < days_count; i++) {
        printf("%-12s%-10d%-12d%-15.2f%-15.2f%-15.2f\n",
               daily_summary[i].date,
               daily_summary[i].orders,
               daily_summary[i].items,
               daily_summary[i].sales,
               daily_summary[i].discount,
               daily_summary[i].sales - daily_summary[i].discount);
    }
    
    printLine();
    printf("\nMonthly Summary:\n");
    GREEN_COLOR;
    printf("Total Days with Sales : %d\n", days_count);
    printf("Total Orders         : %d\n", monthly_orders);
    printf("Total Items Sold     : %d\n", monthly_items);
    printf("Total Sales          : %.2f\n", monthly_total);
    printf("Total Discounts      : %.2f\n", monthly_discount);
    printf("Net Sales            : %.2f\n", monthly_total - monthly_discount);
    printf("Average Daily Sales  : %.2f\n", 
           days_count > 0 ? (monthly_total - monthly_discount)/days_count : 0);
    printf("Average Order Value  : %.2f\n", 
           monthly_orders > 0 ? (monthly_total - monthly_discount)/monthly_orders : 0);
    RESET_COLOR;

    printf("\nPress Enter to continue...");
    getchar();
    getchar();
}

void employeeSalesReport() {
    printHeader("EMPLOYEE SALES REPORT");

    printf("\nLoading employee and admin sales data...\n");

    struct {
        int id;
        char name[MAX_STRING];
        char role[MAX_STRING];
        int orders;
        int items;
        float sales;
        float discount;
    } emp_summary[MAX_EMPLOYEES] = {0};
    int emp_count = 0;

    FILE* emp_file = fopen("employees.txt", "r");
    if (!emp_file) {
        RED_COLOR;
        printf("\nError: Cannot open employees.txt!\n");
        RESET_COLOR;
        sleep(2);
        return;
    }

    Employee emp;
    while (fscanf(emp_file, "%d,%[^,],%[^,],%[^,],%[^,],%f\n",
           &emp.id, emp.name, emp.username, emp.password,
           emp.role, &emp.total_sales) != EOF) {
        emp_summary[emp_count].id = emp.id;
        strcpy(emp_summary[emp_count].name, emp.name);
        strcpy(emp_summary[emp_count].role, emp.role);
        printf("Loaded: %s (%s)\n", emp.name, emp.role);
        emp_count++;
    }
    fclose(emp_file);

    FILE* sales_file = fopen("sales.txt", "r");
    if (!sales_file) {
        RED_COLOR;
        printf("\nNo sales records found!\n");
        RESET_COLOR;
        sleep(2);
        return;
    }

    Order order;
    float total_sales = 0, total_discount = 0;
    int total_orders = 0, total_items = 0;

    while (fscanf(sales_file, "%ld,%[^,],%d,%[^,],%f,%f\n",
                  &order.id,
                  order.customer_phone,
                  &order.employee_id,
                  order.date,
                  &order.total_amount,
                  &order.discount) != EOF) {
        
        for (int i = 0; i < emp_count; i++) {
            if (emp_summary[i].id == order.employee_id) {
                // Count items
                int items_count = 0;
                FILE* items_file = fopen("sales_items.txt", "r");
                if (items_file) {
                    int order_id, product_id, quantity;
                    float price;
                    while (fscanf(items_file, "%d,%d,%d,%f\n", 
                                &order_id, &product_id, &quantity, &price) != EOF) {
                        if (order_id == order.id) {
                            items_count += quantity;
                        }
                    }
                    fclose(items_file);
                }

                emp_summary[i].orders++;
                emp_summary[i].items += items_count;
                emp_summary[i].sales += order.total_amount;
                emp_summary[i].discount += order.discount;
                break;
            }
        }
    }
    fclose(sales_file);

    system("cls");
    printHeader("EMPLOYEE SALES REPORT");

    printf("\nAll Staff Sales Summary (Including Admin):\n");
    printLine();
    printf("%-20s %-10s %-10s %-12s %-15s %-15s %-15s\n", 
           "Name", "Role", "Orders", "Items", "Sales", "Discount", "Net Sales");
    printLine();

    int staff_with_sales = 0;
    for (int i = 0; i < emp_count; i++) {
        if (emp_summary[i].orders > 0) {  // Show anyone with sales
            printf("%-20s %-10s %-10d %-12d %-15.2f %-15.2f %-15.2f\n",
                   emp_summary[i].name,
                   emp_summary[i].role,
                   emp_summary[i].orders,
                   emp_summary[i].items,
                   emp_summary[i].sales,
                   emp_summary[i].discount,
                   emp_summary[i].sales - emp_summary[i].discount);

            total_orders += emp_summary[i].orders;
            total_items += emp_summary[i].items;
            total_sales += emp_summary[i].sales;
            total_discount += emp_summary[i].discount;
            staff_with_sales++;
        }
    }

    if (staff_with_sales == 0) {
        YELLOW_COLOR;
        printf("\nNo sales data found for any staff member.\n");
        RESET_COLOR;
    } else {
        printLine();
        GREEN_COLOR;
        printf("\nOverall Summary:\n");
        printf("Total Staff with Sales: %d\n", staff_with_sales);
        printf("Total Orders         : %d\n", total_orders);
        printf("Total Items Sold     : %d\n", total_items);
        printf("Total Sales          : %.2f\n", total_sales);
        printf("Total Discounts      : %.2f\n", total_discount);
        printf("Net Sales            : %.2f\n", total_sales - total_discount);
        printf("Average Sale         : %.2f\n", 
               total_orders > 0 ? (total_sales - total_discount)/total_orders : 0);
        RESET_COLOR;
    }

    printf("\nPress Enter to continue...");
    getchar();
    getchar();
}

void profitReport() {
    if (strcmp(current_user.role, "admin") != 0) {
        RED_COLOR;
        printf("\nAccess denied! Only administrators can view profit reports.\n");
        RESET_COLOR;
        sleep(2);
        return;
    }
    
    printHeader("PROFIT REPORT");
    
    float total_revenue = 0;
    float total_cost = 0;
    float total_profit = 0;
    int records_found = 0;
    
    FILE* file = fopen("sales.txt", "r");
    if (!file) {
        RED_COLOR;
        printf("\nNo sales records found!\n");
        RESET_COLOR;
        sleep(2);
        return;
    }
    
    printf("\nProduct-wise Profit Summary:\n");
    printLine();
    printf("%-20s%-12s%-15s%-15s%-15s\n", 
           "Product Name", "Qty Sold", "Revenue", "Cost", "Profit");
    printLine();
    
    struct {
        int product_id;
        int qty_sold;
        float revenue;
        float cost;
    } product_summary[MAX_PRODUCTS] = {0};
    
    Order order;
   while (fscanf(file, "%ld,%[^,],%d,%[^,],%f,%f\n",
               &order.id,
               order.customer_phone,
               &order.employee_id,
               order.date,
               &order.total_amount,
               &order.discount) != EOF) {
        
        FILE* items_file = fopen("sales_items.txt", "r");
        if (items_file) {
            int order_id, product_id, quantity;
            float price;
            
            while (fscanf(items_file, "%d,%d,%d,%f\n", 
                         &order_id, &product_id, &quantity, &price) != EOF) {
                if (order_id == order.id) {
                    for (int i = 0; i < product_count; i++) {
                        if (products[i].id == product_id) {
                            product_summary[i].product_id = product_id;
                            product_summary[i].qty_sold += quantity;
                            product_summary[i].revenue += quantity * price;
                            product_summary[i].cost += quantity * products[i].purchase_price;
                            records_found = 1;
                            break;
                        }
                    }
                }
            }
            fclose(items_file);
        }
    }
    fclose(file);
    
    if (!records_found) {
        YELLOW_COLOR;
        printf("\nNo sales data found for analysis.\n");
        RESET_COLOR;
        printf("\nPress Enter to continue...");
        getchar();
        getchar();
        return;
    }
    
    // Print product-wise summary
    for (int i = 0; i < product_count; i++) {
        if (product_summary[i].qty_sold > 0) {
            float profit = product_summary[i].revenue - product_summary[i].cost;
            printf("%-20s%-12d%-15.2f%-15.2f%-15.2f\n",
                   products[i].name,
                   product_summary[i].qty_sold,
                   product_summary[i].revenue,
                   product_summary[i].cost,
                   profit);
            
            total_revenue += product_summary[i].revenue;
            total_cost += product_summary[i].cost;
        }
    }
    
    total_profit = total_revenue - total_cost;
    float profit_margin = (total_revenue > 0) ? (total_profit/total_revenue)*100 : 0;
    
    printLine();
    printf("\nOverall Summary:\n");
    GREEN_COLOR;
    printf("Total Revenue  : %-15.2f\n", total_revenue);
    printf("Total Cost     : %-15.2f\n", total_cost);
    printf("Total Profit   : %-15.2f\n", total_profit);
    printf("Profit Margin  : %-14.2f%%\n", profit_margin);
    RESET_COLOR;
    
    printf("\nPress Enter to continue...");
    getchar();
    getchar();
}

void settingsMenu() {
    int choice;

    while (1) {
        printHeader("SETTINGS");
        printf("\n1. Change Password");
        printf("\n2. Register New Employee (Admin only)");
        printf("\n3. Back to Main Menu");
        printf("\n\nEnter your choice: ");

        scanf("%d", &choice);

        switch (choice) {
            case 1: changePassword(); break;
            case 2: registerEmployee(); break;
            case 3: return;
            default:
                RED_COLOR;
                printf("\nInvalid choice! Please try again.\n");
                RESET_COLOR;
                sleep(1);
        }
    }
}

// Main Function
int main() {
    initializeSystem();
    return 0;
}

// File Operations
void saveTransactionToFile(Order order) {
    // Save main order details to sales.txt
    FILE* file = fopen("sales.txt", "a");
    if (!file) {
        RED_COLOR;
        printf("\nError saving transaction!\n");
        RESET_COLOR;
        return;
    }

    fprintf(file, "%ld,%s,%d,%s,%.2f,%.2f\n",
            order.id,
            order.customer_phone,
            order.employee_id,
            order.date,
            order.total_amount,
            order.discount);

    fclose(file);

    // Save order items to sales_items.txt
    FILE* items_file = fopen("sales_items.txt", "a");
    if (!items_file) {
        RED_COLOR;
        printf("\nError saving transaction items!\n");
        RESET_COLOR;
        return;
    }

    // Save each item in the cart
    for (int i = 0; i < order.item_count; i++) {
        fprintf(items_file, "%ld,%d,%d,%.2f\n",
                order.id,                    
                order.items[i].product_id,   
                order.items[i].quantity,     
                order.items[i].price);      
    }

    fclose(items_file);

    // Update customer spending and loyalty points
    for (int i = 0; i < customer_count; i++) {
        if (strcmp(customers[i].phone, order.customer_phone) == 0) {
            customers[i].total_spending += order.total_amount;
            customers[i].loyalty_points = (int)(customers[i].total_spending / 100);
            saveCustomers();
            break;
        }
    }
}

void saveCustomers() {
    FILE* file = fopen("customers.txt", "w");
    if (!file) {
        RED_COLOR;
        printf("\nError saving customers!\n");
        RESET_COLOR;
        return;
    }
    
    for (int i = 0; i < customer_count; i++) {
        fprintf(file, "%s,%s,%s,%.2f,%d,%d\n",
                customers[i].phone,
                customers[i].name,
                customers[i].address,
                customers[i].total_spending,
                customers[i].loyalty_points,
                customers[i].last_loyalty_milestone);
    }
    
    fclose(file);
}

void loadCustomers() {
    FILE* file = fopen("customers.txt", "r");
    if (!file) return;
    
    customer_count = 0;
    while (!feof(file) && customer_count < MAX_CUSTOMERS) {
        fscanf(file, "%[^,],%[^,],%[^,],%f,%d,%d\n",
               customers[customer_count].phone,
               customers[customer_count].name,
               customers[customer_count].address,
               &customers[customer_count].total_spending,
               &customers[customer_count].loyalty_points,
               &customers[customer_count].last_loyalty_milestone);
        customer_count++;
    }
    
    fclose(file);
}

float applyDiscount(float amount, const char* phone) {
    float discount = 0;
    
    // Find customer
    for (int i = 0; i < customer_count; i++) {
        if (strcmp(customers[i].phone, phone) == 0) {
            // Loyalty discount
            if (customers[i].total_spending >= LOYALTY_THRESHOLD) {
                discount = amount * 0.1; // 10% discount for loyal customers
            }
            break;
        }
    }
    
    return discount;
}

void viewSalesHistory() {
    printHeader("SALES HISTORY");
    
    FILE* file = fopen("sales.txt", "r");
    if (!file) {
        YELLOW_COLOR;
        printf("\nNo sales history found!\n");
        RESET_COLOR;
        sleep(2);
        return;
    }
    
    Order order;
    float total_sales = 0;
    int total_orders = 0;
    
    printf("\nID\tDate\t\t\tCustomer\t\tPhone\t\tAmount\t\tDiscount\n");
    printLine();
    
    while (fscanf(file, "%ld,%[^,],%d,%[^,],%f,%f\n",
                  &order.id,
                  order.customer_phone,
                  &order.employee_id,
                  order.date,
                  &order.total_amount,
                  &order.discount) != EOF) {
        
        if (strcmp(current_user.role, "admin") != 0 && 
            order.employee_id != current_user.id) {
            continue;
        }
        
        char customer_name[MAX_STRING] = "Guest";
        for (int i = 0; i < customer_count; i++) {
            if (strcmp(customers[i].phone, order.customer_phone) == 0) {
                strcpy(customer_name, customers[i].name);
                break;
            }
        }
        
        printf("%ld\t%-20s%-16s%-16s%.2f\t\t%.2f\n",
               order.id,
               order.date,
               customer_name,
               order.customer_phone,
               order.total_amount,
               order.discount);
        
        total_sales += order.total_amount;
        total_orders++;
    }
    
    fclose(file);
    
    printLine();
    printf("\nTotal Orders: %d", total_orders);
    printf("\nTotal Sales: %.2f\n", total_sales);
    
    printf("\nPress Enter to continue...");
    getchar();
}

void updateCustomer() {
    char phone[MAX_STRING];
    printHeader("UPDATE CUSTOMER");
    
    printf("\nEnter Customer Phone Number: ");
    scanf("%s", phone);
    
    for (int i = 0; i < customer_count; i++) {
        if (strcmp(customers[i].phone, phone) == 0) {
            printf("\nCurrent Details:");
            printf("\nName: %s", customers[i].name);
            printf("\nPhone: %s", customers[i].phone);
            printf("\nAddress: %s", customers[i].address);
            printf("\nTotal Spending: %.2f", customers[i].total_spending);
            printf("\nLoyalty Points: %d", customers[i].loyalty_points);
            
            printf("\n\nEnter new details (press Enter to keep current value):\n");
            char input[MAX_STRING];
            getchar();
            
            printf("Name: ");
            fgets(input, MAX_STRING, stdin);
            if (input[0] != '\n') {
                input[strcspn(input, "\n")] = 0;
                strcpy(customers[i].name, input);
            }
            
            printf("Address: ");
            fgets(input, MAX_STRING, stdin);
            if (input[0] != '\n') {
                input[strcspn(input, "\n")] = 0;
                strcpy(customers[i].address, input);
            }
            
            saveCustomers();
            
            GREEN_COLOR;
            printf("\nCustomer updated successfully!\n");
            RESET_COLOR;
            sleep(2);
            return;
        }
    }
    
    RED_COLOR;
    printf("\nCustomer not found!\n");
    RESET_COLOR;
    sleep(2);
}
void updateEmployeeTotalSales(int employee_id, float sale_amount) {
    FILE *file = fopen("employees.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    
    if (!file || !temp) {
        RED_COLOR;
        printf("\nError updating employee sales!\n");
        RESET_COLOR;
        return;
    }

    Employee emp;
    int found = 0;

    while (fscanf(file, "%d,%[^,],%[^,],%[^,],%[^,],%f\n",
           &emp.id, emp.name, emp.username, emp.password,
           emp.role, &emp.total_sales) != EOF) {
        
        if (emp.id == employee_id) {
            emp.total_sales += sale_amount;
            found = 1;
        }
        
        fprintf(temp, "%d,%s,%s,%s,%s,%.2f\n",
                emp.id, emp.name, emp.username, emp.password,
                emp.role, emp.total_sales);
    }

    fclose(file);
    fclose(temp);

    if (found) {
        remove("employees.txt");
        rename("temp.txt", "employees.txt");
        GREEN_COLOR;
        printf("\nEmployee sales updated successfully!\n");
        RESET_COLOR;
    } else {
        remove("temp.txt");
        RED_COLOR;
        printf("\nEmployee ID %d not found!\n", employee_id);
        RESET_COLOR;
    }
}
void generateMonthlySalesReportContent(FILE* file) {
    char month[8];
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    strftime(month, sizeof(month), "%Y-%m", tm);

    fprintf(file, "<style>\n");
    fprintf(file, "table { width: 100%%; border-collapse: collapse; margin: 20px 0; }\n");
    fprintf(file, "th, td { padding: 10px; border: 1px solid #ddd; text-align: left; }\n");
    fprintf(file, "th { background-color: #4CAF50; color: white; }\n");
    fprintf(file, "tr:nth-child(even) { background-color: #f2f2f2; }\n");
    fprintf(file, ".summary { margin: 20px 0; padding: 20px; background: #f9f9f9; border: 1px solid #ddd; }\n");
    fprintf(file, "</style>\n");

    fprintf(file, "<div style='text-align: center; margin-bottom: 30px;'>\n");
    fprintf(file, "<h1 style='color: #4CAF50;'>Monthly Sales Report</h1>\n");
    fprintf(file, "<h2>%s</h2>\n", month);
    fprintf(file, "</div>\n");

    fprintf(file, "<div class='content'>\n");
    fprintf(file, "<h3>Daily Sales Breakdown</h3>\n");
    fprintf(file, "<table>\n");
    fprintf(file, "<thead>\n");
    fprintf(file, "<tr>\n");
    fprintf(file, "<th>Date</th>\n");
    fprintf(file, "<th>Orders</th>\n");
    fprintf(file, "<th>Items Sold</th>\n");
    fprintf(file, "<th>Total Sales</th>\n");
    fprintf(file, "<th>Total Discount</th>\n");
    fprintf(file, "<th>Net Sales</th>\n");
    fprintf(file, "</tr>\n");
    fprintf(file, "</thead>\n");
    fprintf(file, "<tbody>\n");

    struct DailySummary {
        char date[11];
        int orders;
        int items;
        float sales;
        float discount;
    };
    
    struct DailySummary daily_summary[31] = {0};
    int days_count = 0;

    float monthly_total = 0, monthly_discount = 0;
    int monthly_orders = 0, monthly_items = 0;
    Order order;

    FILE* sales_file = fopen("sales.txt", "r");
    if (sales_file) {
        while (fscanf(sales_file, "%ld,%[^,],%d,%[^,],%f,%f\n",
                    &order.id,
                    order.customer_phone,
                    &order.employee_id,
                    order.date,
                    &order.total_amount,
                    &order.discount) != EOF) {
            
            if (strncmp(order.date, month, 7) == 0) {
                char date_str[11];
                strncpy(date_str, order.date, 10);
                date_str[10] = '\0';

                // Find or create daily summary
                int day_index = -1;
                for (int i = 0; i < days_count; i++) {
                    if (strcmp(daily_summary[i].date, date_str) == 0) {
                        day_index = i;
                        break;
                    }
                }
                if (day_index == -1) {
                    day_index = days_count++;
                    strcpy(daily_summary[day_index].date, date_str);
                }

                // Count items
                int items_count = 0;
                FILE* items_file = fopen("sales_items.txt", "r");
                if (items_file) {
                    int order_id, product_id, quantity;
                    float price;
                    while (fscanf(items_file, "%d,%d,%d,%f\n", 
                                &order_id, &product_id, &quantity, &price) != EOF) {
                        if (order_id == order.id) {
                            items_count += quantity;
                        }
                    }
                    fclose(items_file);
                }

                daily_summary[day_index].orders++;
                daily_summary[day_index].items += items_count;
                daily_summary[day_index].sales += order.total_amount;
                daily_summary[day_index].discount += order.discount;

                monthly_orders++;
                monthly_items += items_count;
                monthly_total += order.total_amount;
                monthly_discount += order.discount;
            }
        }
        fclose(sales_file);
    }

    for (int i =0; i < days_count; i++) {
        fprintf(file, "<tr>\n");
        fprintf(file, "<td>%s</td>\n", daily_summary[i].date);
        fprintf(file, "<td>%d</td>\n", daily_summary[i].orders);
        fprintf(file, "<td>%d</td>\n", daily_summary[i].items);
        fprintf(file, "<td>%.2f</td>\n", daily_summary[i].sales);
        fprintf(file, "<td>%.2f</td>\n", daily_summary[i].discount);
        fprintf(file, "<td>%.2f</td>\n", daily_summary[i].sales - daily_summary[i].discount);
        fprintf(file, "</tr>\n");
    }

    fprintf(file, "</tbody>\n");
    fprintf(file, "</table>\n");

    fprintf(file, "<div class='summary'>\n");
    fprintf(file, "<h3 style='color: #4CAF50;'>Monthly Summary</h3>\n");
    fprintf(file, "<table style='width: 50%%; margin: 20px auto;'>\n");
    fprintf(file, "<tr><td><strong>Total Days with Sales:</strong></td><td>%d</td></tr>\n", days_count);
    fprintf(file, "<tr><td><strong>Total Orders:</strong></td><td>%d</td></tr>\n", monthly_orders);
    fprintf(file, "<tr><td><strong>Total Items Sold:</strong></td><td>%d</td></tr>\n", monthly_items);
    fprintf(file, "<tr><td><strong>Total Sales:</strong></td><td>%.2f</td></tr>\n", monthly_total);
    fprintf(file, "<tr><td><strong>Total Discounts:</strong></td><td>%.2f</td></tr>\n", monthly_discount);
    fprintf(file, "<tr><td><strong>Net Sales:</strong></td><td>%.2f</td></tr>\n", monthly_total - monthly_discount);
    fprintf(file, "<tr><td><strong>Average Daily Sales:</strong></td><td>%.2f</td></tr>\n", 
            days_count > 0 ? (monthly_total - monthly_discount)/days_count : 0);
    fprintf(file, "<tr><td><strong>Average Order Value:</strong></td><td>%.2f</td></tr>\n", 
            monthly_orders > 0 ? (monthly_total - monthly_discount)/monthly_orders : 0);
    fprintf(file, "</table>\n");
    fprintf(file, "</div>\n");
    fprintf(file, "</div>\n");
}

void generateMonthlySalesReportCSV(FILE* file) {
    char month[8];
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    strftime(month, sizeof(month), "%Y-%m", tm);

    fprintf(file, "Monthly Sales Report - %s\n\n", month);
    fprintf(file, "Date,Orders,Items Sold,Total Sales,Total Discount,Net Sales\n");

    struct {
        char date[11];
        int orders;
        int items;
        float sales;
        float discount;
    } daily_summary[31] = {0};
    int days_count = 0;

    FILE* sales_file = fopen("sales.txt", "r");
    if (!sales_file) return;

    float monthly_total = 0, monthly_discount = 0;
    int monthly_orders = 0, monthly_items = 0;
    Order order;

    while (fscanf(sales_file, "%ld,%[^,],%d,%[^,],%f,%f\n",
                  &order.id,
                  order.customer_phone,
                  &order.employee_id,
                  order.date,
                  &order.total_amount,
                  &order.discount) != EOF) {
    }
    fclose(sales_file);

    for (int i =0; i < days_count; i++) {
        fprintf(file, "%s,%d,%d,%.2f,%.2f,%.2f\n",
               daily_summary[i].date,
               daily_summary[i].orders,
               daily_summary[i].items,
               daily_summary[i].sales,
               daily_summary[i].discount,
               daily_summary[i].sales - daily_summary[i].discount);
    }

    fprintf(file, "\nMonthly Summary\n");
    fprintf(file, "Total Days with Sales,%d\n", days_count);
    fprintf(file, "Total Orders,%d\n", monthly_orders);
    fprintf(file, "Total Items Sold,%d\n", monthly_items);
    fprintf(file, "Total Sales,%.2f\n", monthly_total);
    fprintf(file, "Total Discounts,%.2f\n", monthly_discount);
    fprintf(file, "Net Sales,%.2f\n", monthly_total - monthly_discount);
    fprintf(file, "Average Daily Sales,%.2f\n", 
            days_count > 0 ? (monthly_total - monthly_discount)/days_count : 0);
    fprintf(file, "Average Order Value,%.2f\n", 
            monthly_orders > 0 ? (monthly_total - monthly_discount)/monthly_orders : 0);
}

// Employee Sales Report Generators
void generateEmployeeSalesReportContent(FILE* file) {
    fprintf(file, "<h2>Employee Sales Report</h2>\n");
    
    fprintf(file, "<style>\n");
    fprintf(file, "table { width: 100%%; border-collapse: collapse; margin: 20px 0; }\n");
    fprintf(file, "th, td { padding: 8px; border: 1px solid #ddd; text-align: left; }\n");
    fprintf(file, "th { background-color: #4CAF50; color: white; }\n");
    fprintf(file, ".summary { margin-top: 20px; }\n");
    fprintf(file, "</style>\n");

    // Start employee sales table
    fprintf(file, "<table>\n");
    fprintf(file, "<tr><th>Name</th><th>Orders</th><th>Items</th>");
    fprintf(file, "<th>Sales</th><th>Discount</th><th>Net Sales</th></tr>\n");
    
    struct {
        int id;
        char name[MAX_STRING];
        int orders;
        int items;
        float sales;
        float discount;
    } emp_summary[MAX_EMPLOYEES] = {0};

    FILE* emp_file = fopen("employees.txt", "r");
    if (emp_file) {
        Employee emp;
        while (fscanf(emp_file, "%d,%[^,],%[^,],%[^,],%[^,],%f\n",
               &emp.id, emp.name, emp.username, emp.password,
               emp.role, &emp.total_sales) != EOF) {
            for (int i = 0; i < MAX_EMPLOYEES; i++) {
                if (emp_summary[i].id == 0) {
                    emp_summary[i].id = emp.id;
                    strcpy(emp_summary[i].name, emp.name);
                    break;
                }
            }
        }
        fclose(emp_file);
    }

    FILE* sales_file = fopen("sales.txt", "r");
    if (!sales_file) return;

    Order order;
    while (fscanf(sales_file, "%ld,%[^,],%d,%[^,],%f,%f\n",
                  &order.id,
                  order.customer_phone,
                  &order.employee_id,
                  order.date,
                  &order.total_amount,
                  &order.discount) != EOF) {
        
        
        for (int i =0; i < MAX_EMPLOYEES; i++) {
            if (emp_summary[i].id == order.employee_id) {
                // Count items
                int items_count = 0;
                FILE* items_file = fopen("sales_items.txt", "r");
                if (items_file) {
                    int order_id, product_id, quantity;
                    float price;
                    while (fscanf(items_file, "%d,%d,%d,%f\n", 
                                &order_id, &product_id, &quantity, &price) != EOF) {
                        if (order_id == order.id) {
                            items_count += quantity;
                        }
                    }
                    fclose(items_file);
                }

                emp_summary[i].orders++;
                emp_summary[i].items += items_count;
                emp_summary[i].sales += order.total_amount;
                emp_summary[i].discount += order.discount;
                break;
            }
        }
    }
    fclose(sales_file);

    float total_sales = 0, total_discount = 0;
    int total_orders = 0, total_items = 0;

    for (int i = 0; i < MAX_EMPLOYEES; i++) {
        if (emp_summary[i].id != 0 && emp_summary[i].orders > 0) {
            fprintf(file, "<tr><td>%s</td><td>%d</td><td>%d</td><td>%.2f</td><td>%.2f</td><td>%.2f</td></tr>\n",
                   emp_summary[i].name,
                   emp_summary[i].orders,
                   emp_summary[i].items,
                   emp_summary[i].sales,
                   emp_summary[i].discount,
                   emp_summary[i].sales - emp_summary[i].discount);

            total_orders += emp_summary[i].orders;
            total_items += emp_summary[i].items;
            total_sales += emp_summary[i].sales;
            total_discount += emp_summary[i].discount;
        }
    }
    fprintf(file, "</table>\n");

    fprintf(file, "<div class='summary'>\n");
    fprintf(file, "<h3>Overall Summary</h3>\n");
    fprintf(file, "<p><strong>Total Orders:</strong> %d</p>\n", total_orders);
    fprintf(file, "<p><strong>Total Items Sold:</strong> %d</p>\n", total_items);
    fprintf(file, "<p><strong>Total Sales:</strong> %.2f</p>\n", total_sales);
    fprintf(file, "<p><strong>Total Discounts:</strong> %.2f</p>\n", total_discount);
    fprintf(file, "<p><strong>Net Sales:</strong> %.2f</p>\n", total_sales - total_discount);
    fprintf(file, "</div>\n");
}

void generateEmployeeSalesReportCSV(FILE* file) {
    fprintf(file, "Employee Sales Report\n\n");
    fprintf(file, "Employee,Orders,Items Sold,Total Sales,Total Discount,Net Sales\n");

    struct {
        int id;
        char name[MAX_STRING];
        int orders;
        int items;
        float sales;
        float discount;
    } emp_summary[MAX_EMPLOYEES] = {0};

    FILE* emp_file = fopen("employees.txt", "r");
    if (emp_file) {
        Employee emp;
        while (fscanf(emp_file, "%d,%[^,],%[^,],%[^,],%[^,],%f\n",
               &emp.id, emp.name, emp.username, emp.password,
               emp.role, &emp.total_sales) != EOF) {
            for (int i =0; i < MAX_EMPLOYEES; i++) {
                if (emp_summary[i].id == 0) {
                    emp_summary[i].id = emp.id;
                    strcpy(emp_summary[i].name, emp.name);
                    break;
                }
            }
        }
        fclose(emp_file);
    }

    FILE* sales_file = fopen("sales.txt", "r");
    if (!sales_file) return;

    Order order;
    while (fscanf(sales_file, "%ld,%[^,],%d,%[^,],%f,%f\n",
                  &order.id,
                  order.customer_phone,
                  &order.employee_id,
                  order.date,
                  &order.total_amount,
                  &order.discount) != EOF) {
        
        for (int i =0; i < MAX_EMPLOYEES; i++) {
            if (emp_summary[i].id == order.employee_id) {
                int items_count = 0;
                FILE* items_file = fopen("sales_items.txt", "r");
                if (items_file) {
                    int order_id, product_id, quantity;
                    float price;
                    while (fscanf(items_file, "%d,%d,%d,%f\n", 
                                &order_id, &product_id, &quantity, &price) != EOF) {
                        if (order_id == order.id) {
                            items_count += quantity;
                        }
                    }
                    fclose(items_file);
                }

                emp_summary[i].orders++;
                emp_summary[i].items += items_count;
                emp_summary[i].sales += order.total_amount;
                emp_summary[i].discount += order.discount;
                break;
            }
        }
    }
    fclose(sales_file);

    float total_sales = 0, total_discount = 0;
    int total_orders = 0, total_items = 0;

    for (int i =0; i < MAX_EMPLOYEES; i++) {
        if (emp_summary[i].id != 0 && emp_summary[i].orders > 0) {
            fprintf(file, "%s,%d,%d,%.2f,%.2f,%.2f\n",
                   emp_summary[i].name,
                   emp_summary[i].orders,
                   emp_summary[i].items,
                   emp_summary[i].sales,
                   emp_summary[i].discount,
                   emp_summary[i].sales - emp_summary[i].discount);

            total_orders += emp_summary[i].orders;
            total_items += emp_summary[i].items;
            total_sales += emp_summary[i].sales;
            total_discount += emp_summary[i].discount;
        }
    }

    fprintf(file, "\nOverall Summary\n");
    fprintf(file, "Total Orders,%d\n", total_orders);
    fprintf(file, "Total Items Sold,%d\n", total_items);
    fprintf(file, "Total Sales,%.2f\n", total_sales);
    fprintf(file, "Total Discounts,%.2f\n", total_discount);
    fprintf(file, "Net Sales,%.2f\n", total_sales - total_discount);
}

void generateDailySalesReportCSV(FILE* file) {
    char date[11];
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    strftime(date, sizeof(date), "%Y-%m-%d", tm);

    fprintf(file, "Daily Sales Report - %s\n\n", date);
    fprintf(file, "Time,Order ID,Customer,Items,Amount,Discount,Net Amount\n");

    float total_sales = 0, total_discount = 0;
    int total_orders = 0;
    Order order;

    FILE* sales_file = fopen("sales.txt", "r");
    if (sales_file) {
        while (fscanf(sales_file, "%ld,%[^,],%d,%[^,],%f,%f\n",
                    &order.id,
                    order.customer_phone,
                    &order.employee_id,
                    order.date,
                    &order.total_amount,
                    &order.discount) != EOF) {
            
            if (strstr(order.date, date)) {
                // Same processing as HTML version but with CSV formatting
                char customer_name[MAX_STRING] = "Guest";
                for (int i = 0; i < customer_count; i++) {
                    if (strcmp(customers[i].phone, order.customer_phone) == 0) {
                        strcpy(customer_name, customers[i].name);
                        break;
                    }
                }

                char time_str[9];
                strncpy(time_str, order.date + 11, 8);
                time_str[8] = '\0';

                int items_count = 0;
                FILE* items_file = fopen("sales_items.txt", "r");
                if (items_file) {
                    int order_id, product_id, quantity;
                    float price;
                    while (fscanf(items_file, "%d,%d,%d,%f\n", 
                                &order_id, &product_id, &quantity, &price) != EOF) {
                        if (order_id == order.id) {
                            items_count += quantity;
                        }
                    }
                    fclose(items_file);
                }

                fprintf(file, "%s,%ld,%s,%d,%.2f,%.2f,%.2f\n",
                       time_str, order.id, customer_name, items_count, 
                       order.total_amount, order.discount,
                       order.total_amount - order.discount);

                total_sales += order.total_amount;
                total_discount += order.discount;
                total_orders++;
            }
        }
        fclose(sales_file);
    }

    fprintf(file, "\nDaily Summary\n");
    fprintf(file, "Total Orders,%d\n", total_orders);
    fprintf(file, "Total Sales,%.2f\n", total_sales);
    fprintf(file, "Total Discounts,%.2f\n", total_discount);
    fprintf(file, "Net Sales,%.2f\n", total_sales - total_discount);
}

char* getCurrentDateTime(void) {
    static char datetime[20];
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    strftime(datetime, sizeof(datetime), "%Y-%m-%d %H:%M:%S", tm);
    return datetime;
}

void generateDailySalesReportContent(FILE* file) {
    char date[11];
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    strftime(date, sizeof(date), "%Y-%m-%d", tm);

    fprintf(file, "<h2>Daily Sales Report</h2>\n");
    fprintf(file, "<p><strong>Date:</strong> %s</p>\n", date);
    fprintf(file, "<table>\n");
    fprintf(file, "<tr><th>Time</th><th>Order ID</th><th>Customer</th><th>Items</th><th>Amount</th><th>Discount</th><th>Net Amount</th></tr>\n");

    float total_sales = 0, total_discount = 0;
    int total_orders = 0, total_items = 0;
    Order order;

    FILE* sales_file = fopen("sales.txt", "r");
    if (sales_file) {
        while (fscanf(sales_file, "%ld,%[^,],%d,%[^,],%f,%f\n",
                    &order.id,
                    order.customer_phone,
                    &order.employee_id,
                    order.date,
                    &order.total_amount,
                    &order.discount) != EOF) {
            
            if (strstr(order.date, date)) {
                // Get customer name
                char customer_name[MAX_STRING] = "Guest";
                for (int i = 0; i < customer_count; i++) {
                    if (strcmp(customers[i].phone, order.customer_phone) == 0) {
                        strcpy(customer_name, customers[i].name);
                        break;
                    }
                }

                // Extract time from date
                char time_str[9];
                strncpy(time_str, order.date + 11, 8);
                time_str[8] = '\0';

                // Count items in order
                int items_count = 0;
                FILE* items_file = fopen("sales_items.txt", "r");
                if (items_file) {
                    int order_id, product_id, quantity;
                    float price;
                    while (fscanf(items_file, "%d,%d,%d,%f\n", 
                                &order_id, &product_id, &quantity, &price) != EOF) {
                        if (order_id == order.id) {
                            items_count += quantity;
                        }
                    }
                    fclose(items_file);
                }

                // Write order row
                fprintf(file, "<tr><td>%s</td><td>%ld</td><td>%s</td><td>%d</td><td>%.2f</td><td>%.2f</td><td>%.2f</td></tr>\n",
                       time_str, 
                       order.id, 
                       customer_name, 
                       items_count, 
                       order.total_amount, 
                       order.discount,
                       order.total_amount - order.discount);

                total_sales += order.total_amount;
                total_discount += order.discount;
                total_orders++;
                total_items += items_count;
            }
        }
        fclose(sales_file);
    }

    fprintf(file, "</table>\n");
    
    // Write summary
    fprintf(file, "<div class='summary'>\n");
    fprintf(file, "<h3>Daily Summary</h3>\n");
    fprintf(file, "<p><strong>Total Orders:</strong> %d</p>\n", total_orders);
    fprintf(file, "<p><strong>Total Items Sold:</strong> %d</p>\n", total_items);
    fprintf(file, "<p><strong>Total Sales:</strong> %.2f</p>\n", total_sales);
    fprintf(file, "<p><strong>Total Discounts:</strong> %.2f</p>\n", total_discount);
    fprintf(file, "<p><strong>Net Sales:</strong> %.2f</p>\n", total_sales - total_discount);
    fprintf(file, "<p><strong>Average Order Value:</strong> %.2f</p>\n", 
            total_orders > 0 ? (total_sales - total_discount)/total_orders : 0);
    fprintf(file, "</div>\n");
}

void openPDF(const char* filename) {
    #ifdef _WIN32
        char command[512];
        sprintf(command, "start %s", filename);
    #else
        char command[512];
        sprintf(command, "open %s", filename);  // 'open' command for macOS
    #endif
    system(command);
}
