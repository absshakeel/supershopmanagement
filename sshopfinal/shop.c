#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <conio.h>
#include <time.h>
#include <windows.h>

#define MAX_PRODUCTS 100
#define MAX_CART_ITEMS 100
#define SALES_REPORT_FILE "sales_report.txt"
#define USER_SALES_FILE "user_sales.txt"

struct Product {
    int id;
    char name[50];
    float price;
    int stock;
    char unit[10];
};

struct CartItem {
    int productId;
    int quantity;
    float totalCost;
    char unit[10];
};

struct Order {
    long id;
    char date[20]; 
    int payment_method;
    char customer_phone[15];
    float total_amount;
    float manual_discount_percentage;
    int item_count;
    struct {
        long product_id;
        float price;
        int quantity;
    } items[100]; 
};

void deleteItem();
int trackID();
void insert();
void getMaskedInput(char *password, int maxLength);
void saveProduct(const char *filename, struct Product products[], int productCount);
void loadProducts(const char *filename, struct Product products[], int *productCount);
void displayProductCatalog(struct Product products[], int productCount);
void addToCart(struct CartItem cart[], struct Product products[], int *cartCount, int productCount);
void viewCart(struct CartItem cart[], struct Product products[], int cartCount);
void removeFromCart(struct CartItem cart[], struct Product products[], int *cartCount);
void checkout(struct CartItem cart[], struct Product products[], int *cartCount, int productCount, const char *filename, const char *username);
void updateItem(); 
void logUserSale(const char *username, float totalAmount, const char *date); 
void clearScreen();
void showView(int selectedView, struct Product products[], int productCount, struct CartItem cart[], int cartCount);
void updateViewAndMenu(int *selectedView, struct Product products[], int productCount, struct CartItem cart[], int cartCount);
void processPayment(float total, struct Order *order, struct Product products[], int productCount);
void generateReceipt(struct Order order, const char *transactionId, struct Product products[], int productCount);
void generateSalesReport(int reportType);
void logSale(float totalAmount, const char *date);
void viewUserSalesHistory(const char *username);
void viewAllUserSales();
const char* getProductNameById(struct Product products[], int productCount, long productId);

int main() {
    struct Product products[MAX_PRODUCTS];
    int choice = 0, pageChoice, login = 0;
    char userPass[20], userPassword[20];
    char username[20], userID[20];

    while (1) {
        clearScreen();

        printf("------------------\n");
        printf("  | Super shop |\n");
        printf("------------------\n");
        printf("1. Login as Admin\n");
        printf("2. Login as User\n");
        printf("3. Create new account\n");
        printf("4. Exit program\n");

        printf("\nEnter your choice: ");
        scanf("%d", &pageChoice);

        if (pageChoice == 1) {
            FILE *fptr = fopen("namepass.txt", "r");
            if (!fptr) {
                printf("Error: Could not open file.\n");
                exit(EXIT_FAILURE);
            }

            printf("\nEnter admin username: ");
            scanf(" %s", userID);
            printf("\nEnter admin password: ");
            getMaskedInput(userPass, sizeof(userPass));

            fscanf(fptr, "%s %s", username, userPassword);
            if (strcmp(userPassword, userPass) == 0 && strcmp(username, userID) == 0) {
                printf("\n--------------------\n");
                printf("| Login successful |");
                printf("\n--------------------\n\n");
                login = 1;
                sleep(2);
            } else {
                printf("\n------------------------------\n");
                printf("| Wrong password or username |");
                printf("\n------------------------------\n\n");
                sleep(2);
            }
            fclose(fptr);
        } else if (pageChoice == 2) {
            printf("\nEnter your username: ");
            scanf(" %s", userID);
            printf("\nEnter your password: ");
            getMaskedInput(userPass, sizeof(userPass));

            FILE *fptr = fopen("namepass.txt", "r");
            if (!fptr) {
                perror("Error opening file");
                exit(EXIT_FAILURE);
            }

            login = 0;
            while (fscanf(fptr, "%s %s", username, userPassword) != EOF) {
                if (strcmp(userPassword, userPass) == 0 && strcmp(username, userID) == 0) {
                    printf("\n--------------------\n");
                    printf("| Login successful |");
                    printf("\n--------------------\n\n");
                    login = 1;
                    sleep(2);
                    break;
                }
            }
            fclose(fptr);

            if (!login) {
                printf("\n------------------------------\n");
                printf("| Wrong password or username |");
                printf("\n------------------------------\n\n");
                sleep(2);
            }
        } else if (pageChoice == 3) {
            printf("\nEnter your username: ");
            scanf(" %s", userID);

            FILE *fptr = fopen("namepass.txt", "a+");
            if (!fptr) {
                perror("Error opening file");
                exit(EXIT_FAILURE);
            }

            int userExists = 0;
            while (fscanf(fptr, "%s %s", username, userPassword) != EOF) {
                if (strcmp(username, userID) == 0) {
                    printf("\n-----------------------\n");
                    printf("User already exists\n");
                    printf("-----------------------\n");
                    userExists = 1;
                    sleep(2);
                    break;
                }
            }

            if (!userExists) {
                printf("\nEnter your password: ");
                getMaskedInput(userPass, sizeof(userPass));
                fprintf(fptr, "%s %s\n", userID, userPass);

                printf("\n-----------------------\n");
                printf("New account created\n");
                printf("-----------------------\n");
                sleep(2);
            }

            fclose(fptr);
        } else if (pageChoice == 4) {
            return 0;
        } else {
            printf("Wrong choice, try again...\n");
            sleep(2);
        }

        clearScreen();

        int productCount = sizeof(products) / sizeof(products[0]);
        loadProducts("Products.txt", products, &productCount);

        struct CartItem cart[MAX_CART_ITEMS];
        int cartCount = 0;

        choice = 1;
        int selectedView = 1;

        while (login == 1) {
            loadProducts("Products.txt", products, &productCount);
            updateViewAndMenu(&selectedView, products, productCount, cart, cartCount);
            if (pageChoice == 2) {
                printf("\nMenu:\n");
                if (selectedView == 1) printf("1. Switch to Cart\n");
                else printf("1. Switch Product Catalog\n");
                printf("2. Add to Cart\n");
                printf("3. Remove Item from Cart\n");
                printf("4. Checkout\n");
                printf("5. View My Sales History\n");
                printf("6. Exit\n");
                printf("--------------------------------------------\n");
                printf("Enter your choice: ");
                scanf("%d", &choice);

                if (choice == 1) {
                    if (selectedView == 1) selectedView = 2;
                    else selectedView = 1;
                } else if (choice == 2) {
                    addToCart(cart, products, &cartCount, productCount);
                } else if (choice == 3) {
                    removeFromCart(cart, products, &cartCount);
                } else if (choice == 4) {
                    checkout(cart, products, &cartCount, productCount, "Products.txt", userID);
                } else if (choice == 5) {
                    viewUserSalesHistory(userID);
                } else if (choice == 6) {
                    login = 0;
                } else {
                    printf("Wrong choice, try again...\n");
                    sleep(2);
                    continue;
                }
            } else if (pageChoice == 1) {
                selectedView = 1;
                printf("\nMenu:\n");
                printf("1. Add Product\n");
                printf("2. Remove Product\n");
                printf("3. Update Product\n");
                printf("4. View Sales Reports\n");
                printf("5. Exit\n");
                printf("--------------------------------------------\n");
                printf("Enter your choice: ");
                scanf("%d", &choice);

                if (choice == 1) {
                    insert();
                } else if (choice == 2) {
                    deleteItem();
                } else if (choice == 3) {
                    updateItem();
                } else if (choice == 4) {
                    int reportChoice;
                    printf("1. Daily Sales Report\n");
                    printf("2. Monthly Sales Report\n");
                    printf("3. Total Sales Report\n");
                    printf("4. All User Sales Report\n");
                    printf("Select report type: ");
                    scanf("%d", &reportChoice);
                    generateSalesReport(reportChoice);
                } else if (choice == 5) {
                    login = 0;
                } else {
                    printf("Wrong choice, try again...\n");
                    sleep(2);
                    continue;
                }
            }

            getchar();
            clearScreen();
        }
    }
    return 0;
}

void getMaskedInput(char *password, int maxLength) {
    int i = 0;
    char ch;

    while (i < maxLength - 1) {
        ch = getch();
        if (ch == '\r') {
            break;
        } else if (ch == '\b' || ch == 127) {
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else {
            password[i++] = ch;
            printf("*");
        }
    }
    password[i] = '\0';
    printf("\n");
}

int trackID() {
    int n = 1, found;
    struct Product item;
    FILE *fp1 = fopen("Products.txt", "r");
    if (fp1 == NULL) {
        return n;
    }

    while (1) {
        found = 0;
        rewind(fp1);
        while (fscanf(fp1, "%d %s %f %d %s", &item.id, item.name, &item.price, &item.stock, item.unit) != EOF) {
            if (n == item.id) {
                found = 1;
                break;
            }
        }
        if (!found) {
            fclose(fp1);
            return n;
        }
        n++;
    }
}

void deleteItem() {
    int id, found = 0;
    char Line[200];
    struct Product item;

    FILE *fp1 = fopen("Products.txt", "r");
    FILE *fp2 = fopen("temp.txt", "w");

    if (fp1 == NULL || fp2 == NULL) {
        printf("Unable to open file.\n");
        sleep(2);
        return;
    }

    printf("Enter the item ID you want to delete: ");
    scanf("%d", &id);

    while (fgets(Line, sizeof(Line), fp1)) {
        sscanf(Line, "%d", &item.id);

        if (item.id != id) {
            fputs(Line, fp2);
        } else {
            found = 1;
        }
    }

    if (!found) {
        printf("Item with ID %d not found!\n", id);
        sleep(2);
    } else {
        printf("Item with ID %d deleted successfully.\n", id);
        sleep(2);
    }

    fclose(fp2);
    fclose(fp1);

    remove("Products.txt");
    rename("temp.txt", "Products.txt");
}

void saveProduct(const char *filename, struct Product products[], int productCount) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < productCount; i++) {
        fprintf(file, "%d %s %.2f %d %s\n",
                products[i].id,
                products[i].name,
                products[i].price,
                products[i].stock,
                products[i].unit);
    }

    fclose(file);
}

void updateItem() {
    int id, found = 0;
    char Line[200];
    struct Product item;
    float newPrice;
    int newStock;

    FILE *fp1 = fopen("Products.txt", "r");
    FILE *fp2 = fopen("temp.txt", "w");

    if (fp1 == NULL || fp2 == NULL) {
        printf("Unable to open file.\n");
        return;
    }

    printf("Enter the item ID you want to update: ");
    scanf("%d", &id);

    printf("Enter the new item price: ");
    scanf("%f", &newPrice);
    printf("Enter the new item stock: ");
    scanf("%d", &newStock);

    while (fgets(Line, sizeof(Line), fp1)) {
        if (sscanf(Line, "%d %s %f %d %s", &item.id, item.name, &item.price, &item.stock, item.unit) == 5) {
            if (item.id == id) {
                item.price = newPrice;
                item.stock = newStock;
                found = 1;
            }
            fprintf(fp2, "%d %s %.2f %d %s\n", item.id, item.name, item.price, item.stock, item.unit);
        } else {
            fputs(Line, fp2);
        }
    }

    if (!found) {
        printf("Item with ID %d not found!\n", id);
        sleep(2);
    }

    fclose(fp2);
    fclose(fp1);

    remove("Products.txt");
    rename("temp.txt", "Products.txt");
}

void insert() {
    struct Product item;
    int id = trackID();

    FILE *fp = fopen("Products.txt", "a");
    if (fp == NULL) {
        printf("Unable to open file.\n");
        return;
    }

    printf("Enter the item name: ");
    scanf("%s", item.name);

    printf("Enter the item price: ");
    scanf("%f", &item.price);

    printf("Enter the item amount: ");
    scanf("%d", &item.stock);

    printf("Enter the item unit: ");
    scanf("%s", item.unit);

    fprintf(fp, "%d %s %.2f %d %s\n", id, item.name, item.price, item.stock, item.unit);

    fclose(fp);
}

void loadProducts(const char *filename, struct Product products[], int *productCount) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening product file");
        exit(EXIT_FAILURE);
    }

    *productCount = 0;
    while (fscanf(file, "%d %s %f %d %s",
                  &products[*productCount].id,
                  products[*productCount].name,
                  &products[*productCount].price,
                  &products[*productCount].stock,
                  products[*productCount].unit) != EOF) {

        (*productCount)++;

        if (*productCount >= MAX_PRODUCTS) {
            printf("Warning: Maximum product limit reached. Remaining products ignored.\n");
            break;
        }
    }

    fclose(file);
}

void displayProductCatalog(struct Product products[], int productCount) {
    printf("\nProduct Catalog: (20%% discount for purchase over 4000 TK)\n");
    printf("-----------------------------------------------------------------\n");
    printf("%-5s | %-20s | %-10s  | %-20s\n", "ID", "Product Name", "Price", "Stock");
    printf("-----------------------------------------------------------------\n");

    for (int i = 0; i < productCount; i++) {
        printf("%-5d | %-20s | %-9.2fTK | %-10d%-10s\n", products[i].id, products[i].name, products[i].price, products[i].stock, products[i].unit);
        printf("-----------------------------------------------------------------\n");
    }
}

void addToCart(struct CartItem cart[], struct Product products[], int *cartCount, int productCount) {
    int productId, quantity;
    printf("\nEnter the product ID to add to cart: ");
    scanf("%d", &productId);

    for (int i = 0; i < productCount; i++) {
        if (products[i].id == productId) {
            printf("Enter quantity: ");
            scanf("%d", &quantity);

            if (quantity > products[i].stock || quantity <= 0) {
                printf("Invalid quantity! Available stock: %d\n", products[i].stock);
                sleep(2);
                return;
            }

            cart[*cartCount].productId = productId;
            cart[*cartCount].quantity = quantity;
            strcpy(cart[*cartCount].unit, products[i].unit);
            cart[*cartCount].totalCost = products[i].price * quantity;
            products[i].stock -= quantity;
            (*cartCount)++;
            printf("Added to cart: %s (x%d)\n", products[i].name, quantity);
            sleep(2);
            return;
        }
    }

    printf("Product ID not found in catalog!\n");
    sleep(2);
}

void viewCart(struct CartItem cart[], struct Product products[], int cartCount) {
    if (cartCount == 0) {
        printf("\nYour cart is empty!\n");
        return;
    }

    printf("\nYour Cart:\n");
    printf("-------------------------------------------------------------------\n");
    printf("%-5s | %-20s | %-20s  | %-10s\n", "ID", "Product Name", "Quantity", "Total Cost");
    printf("-------------------------------------------------------------------\n");

    float grandTotal = 0.0;
    for (int i = 0; i < cartCount; i++) {
        for (int j = 0; j < MAX_PRODUCTS; j++) {
            if (cart[i].productId == products[j].id) {
                printf("%-5d | %-20s | %-10d%-10s  | %-9.2fTK\n", products[j].id, products[j].name, cart[i].quantity, cart[i].unit, cart[i].totalCost);
                printf("-------------------------------------------------------------------\n");
                grandTotal += cart[i].totalCost;
                break;
            }
        }
    }

    if (grandTotal >= 4000) {
        float discount = grandTotal * 0.20;
        grandTotal -= discount;
        printf("You received a 20%% discount: -%.2fTK\n", discount);
    }

    printf("Grand Total: %.2fTK\n", grandTotal);
    printf("--------------------------------------------\n");
}

void removeFromCart(struct CartItem cart[], struct Product products[], int *cartCount) {
    if (*cartCount == 0) {
        printf("\nYour cart is empty! No items to remove.\n");
        sleep(2);
        return;
    }

    int productId;
    printf("\nEnter the product ID to remove from cart: ");
    scanf("%d", &productId);

    for (int i = 0; i < *cartCount; i++) {
        if (cart[i].productId == productId) {
            for (int j = 0; j < MAX_PRODUCTS; j++) {
                if (products[j].id == productId) {
                    products[j].stock += cart[i].quantity;
                    break;
                }
            }

            for (int k = i; k < *cartCount - 1; k++) {
                cart[k] = cart[k + 1];
            }
            (*cartCount)--;

            printf("Item removed from the cart.\n");
            sleep(2);
            return;
        }
    }
    printf("Product ID not found in the cart!\n");
    sleep(2);
}

void checkout(struct CartItem cart[], struct Product products[], int *cartCount, int productCount, const char *filename, const char *username) {
    if (*cartCount == 0) {
        printf("\nYour cart is empty! Cannot proceed with checkout.\n");
        sleep(2);
        return;
    }

    printf("\nProceeding with checkout...\n");

    float totalAmount = 0.0;
    struct Order order;
    order.id = time(NULL); // Use current time as order ID
    strftime(order.date, sizeof(order.date), "%Y-%m-%d %H:%M:%S", localtime(&(time_t){time(NULL)}));
    order.item_count = *cartCount;

    for (int i = 0; i < *cartCount; i++) {
        int cartProductId = cart[i].productId;
        int cartQuantity = cart[i].quantity;

        for (int j = 0; j < productCount; j++) {
            if (products[j].id == cartProductId) {
                if (products[j].stock >= cartQuantity) {
                    products[j].stock -= cartQuantity;
                    totalAmount += products[j].price * cartQuantity;
                    order.items[i].product_id = cartProductId;
                    order.items[i].price = products[j].price;
                    order.items[i].quantity = cartQuantity;
                } else {
                    printf("Not enough stock for product: %s. Skipping item.\n", products[j].name);
                    sleep(1);
                }
                break;
            }
        }
    }

    processPayment(totalAmount, &order, products, productCount);
    logSale(totalAmount, order.date);
    logUserSale(username, totalAmount, order.date);

    *cartCount = 0;

    saveProduct(filename, products, productCount);

    printf("Catalog updated. Checkout complete. Your cart is now empty.\n");
    sleep(5);
}

void processPayment(float total, struct Order *order, struct Product products[], int productCount) {
    int choice;
    float discount = 0;

    printf("\n=== PAYMENT PROCESSING ===\n");
    printf("\nCurrent Total: %.2f", total);

    printf("\n\n=== SELECT PAYMENT METHOD ===\n");
    printf("\n1. Cash");
    printf("\n2. 1Card (5%% discount)");
    printf("\n3. bKash");
    printf("\n4. Nagad");
    printf("\n5. Bank Transfer");
    printf("\nSelect payment method (1-5): ");
    scanf("%d", &choice);

  
    if (choice == 2) {
        discount = total * 0.05; 
        order->manual_discount_percentage = 5.0; 
        printf("\n1Card discount applied: %.2f", discount);
    } else {
        order->manual_discount_percentage = 0.0; 
    }


    char transactionId[50] = ""; 
    if (choice == 3 || choice == 4 || choice == 5) {
        printf("Enter Transaction ID: ");
        scanf("%s", transactionId);
    }

    order->payment_method = choice; 
    float finalAmount = total - discount; 
    order->total_amount = finalAmount;

    printf("\n=== PAYMENT SUMMARY ===\n");
    printf("Original Amount: %.2f TK\n", total);
    if (discount > 0) {
        printf("Total Discount: %.2f TK\n", discount);
    }
    printf("Final Amount: %.2f\n", finalAmount);
    printf("Payment Method: %s\n", 
           choice == 1 ? "Cash" :
           choice == 2 ? "1Card" :
           choice == 3 ? "bKash" :
           choice == 4 ? "Nagad" : "Bank Transfer");


    generateReceipt(*order, transactionId, products, productCount); 

    printf("\nPress Enter to continue...");
    getchar();
    getchar();
}

void generateReceipt(struct Order order, const char *transactionId, struct Product products[], int productCount) {
    char filename[256];
    char html_file[256];
    char print_command[512];
    

    CreateDirectory("receipts", NULL);
    sprintf(html_file, "receipts\\receipt_%ld.html", order.id);
    sprintf(filename, "receipts\\receipt_%ld.pdf", order.id);

    FILE* file = fopen(html_file, "w");
    if (!file) {
        printf("\nError creating receipt file!\n");
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
    fprintf(file, "    <h1>DIU SUPER SHOP</h1><br>\n");
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
           order.payment_method == 1 ? "Cash" :
           order.payment_method == 2 ? "1Card" :
           order.payment_method == 3 ? "bKash" :
           order.payment_method == 4 ? "Nagad" : "Bank Transfer");
    if (strlen(transactionId) > 0) {
        fprintf(file, "\n    Transaction ID: %s\n", transactionId); 
    }
    fprintf(file, "  </div>\n");
    fprintf(file, "</div>\n");


    fprintf(file, "<table>\n");
    fprintf(file, "<tr><th>SL</th><th>Description</th><th>Price</th><th>Quantity</th><th>Total</th></tr>\n");
    
    float subtotal = 0;
    for (int i = 0; i < order.item_count; i++) {
        float amount = order.items[i].quantity * order.items[i].price;
        subtotal += amount;
        const char* productName = getProductNameById(products, productCount, order.items[i].product_id); // Get product name
        fprintf(file, "<tr><td>%d</td><td>%s</td><td>%.2f</td><td>%d</td><td>%.2f</td></tr>\n",
                i + 1,
                productName,
                order.items[i].price,
                order.items[i].quantity,
                amount);
    }
    fprintf(file, "</table>\n");
    
    fprintf(file, "<div class='footer'>\n");
    fprintf(file, "  <div class='terms'>\n");
    fprintf(file, "    <strong>Thank you for Purchage</strong><br><br>\n");
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
    
   
    
    fprintf(file, "</div>\n");

    fclose(file);

    // Convert HTML to PDF 
    sprintf(print_command, "wkhtmltopdf \"%s\" \"%s\"", html_file, filename);
    int result = system(print_command);
    
    if (result != 0) {
        printf("\nError generating PDF receipt!\n");
        return;
    }


    sprintf(print_command, "start %s", filename);
    system(print_command);


    sprintf(print_command, "del %s", html_file);
    system(print_command);

    printf("\nReceipt generated: %s\n", filename);
}

void logSale(float totalAmount, const char *date) {
    FILE *file = fopen(SALES_REPORT_FILE, "a+");
    if (!file) {
        printf("Error opening sales report file!\n");
        return;
    }
    fprintf(file, "Sale Date: %s, Amount: %.2f\n", date, totalAmount);
    fclose(file);
}

void logUserSale(const char *username, float totalAmount, const char *date) {
    FILE *file = fopen(USER_SALES_FILE, "a+");
    if (!file) {
        printf("Error opening user sales report file!\n");
        return;
    }
    fprintf(file, "User: %s, Sale Date: %s, Amount: %.2f\n", username, date, totalAmount);
    fclose(file);
}
void generateSalesReport(int reportType) {
    FILE *file = fopen(SALES_REPORT_FILE, "r");
    if (!file) {
        printf("Error opening sales report file!\n");
        return;
    }

    float totalSales = 0.0;
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        float amount;
        sscanf(line, "Sale Date: %*[^,], Amount: %f", &amount);
        totalSales += amount;
    }
    fclose(file);

    if (reportType == 1) {
        printf("Daily Sales Report: Total Sales = %.2f\n", totalSales);
    } else if (reportType == 2) {
        printf("Monthly Sales Report: Total Sales = %.2f\n", totalSales);
    } else if (reportType == 3) {
        printf("Total Sales Report: Total Sales = %.2f\n", totalSales);
    } else if (reportType == 4) {
        viewAllUserSales();
    }
}

void viewUserSalesHistory(const char *username) {
    FILE *file = fopen(USER_SALES_FILE, "r");
    if (!file) {
        printf("Error opening user sales report file!\n");
        return;
    }

    char line[256];
    printf("\nSales History for %s:\n", username);
    printf("--------------------------------------------------\n");
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, username) != NULL) {
            printf("%s", line);
        }
    }
    fclose(file);
    printf("--------------------------------------------------\n");
    sleep(2);
}

void viewAllUserSales() {
    FILE *file = fopen(USER_SALES_FILE, "r");
    if (!file) {
        printf("Error opening user sales report file!\n");
        return;
    }

    char line[256];
    printf("\nAll User Sales History:\n");
    printf("--------------------------------------------------\n");
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }
    fclose(file);
    printf("--------------------------------------------------\n");
    sleep(2);
}

void clearScreen() {
    system("cls");
}

void showView(int selectedView, struct Product products[], int productCount, struct CartItem cart[], int cartCount) {
    if (selectedView == 1) {
        displayProductCatalog(products, productCount);
    } else if (selectedView == 2) {
        viewCart(cart, products, cartCount);
    }
}

void updateViewAndMenu(int *selectedView, struct Product products[], int productCount, struct CartItem cart[], int cartCount) {
    showView(*selectedView, products, productCount, cart, cartCount);
}

const char* getProductNameById(struct Product products[], int productCount, long productId) {
    for (int i = 0; i < productCount; i++) {
        if (products[i].id == productId) {
            return products[i].name;
        }
    }
    return "Unknown Product";
}