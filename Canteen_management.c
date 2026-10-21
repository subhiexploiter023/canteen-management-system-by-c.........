#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_QUEUE 100
#define MAX_STACK 100
#define MENU_FILE "menu.dat"
#define ORDER_FILE "orders.dat"

typedef struct {
    int id;
    char name[50];
    float price;
    int stock;
} Food;

typedef struct Node {
    Food food;
    struct Node *next;
} Node;

typedef struct {
    int orderId;
    char customer[50];
    int foodId;
    char foodName[50];
    int quantity;
    float total;
} Order;

typedef struct {
    Order items[MAX_QUEUE];
    int front, rear;
} Queue;

typedef struct {
    Order items[MAX_STACK];
    int top;
} Stack;

Node *menuHead = NULL;
Queue orderQueue;
Stack cancelledOrders;
int nextOrderId = 1001;

/* ---------- Utility ---------- */
void clearInput(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

void pauseScreen(void) {
    printf("\nPress Enter to continue...");
    getchar();
}

void line(void) {
    printf("============================================================\n");
}

/* ---------- Linked List: Menu ---------- */
Node* createNode(Food food) {
    Node *newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    newNode->food = food;
    newNode->next = NULL;
    return newNode;
}

void insertFood(Food food) {
    Node *newNode = createNode(food);

    if (menuHead == NULL) {
        menuHead = newNode;
        return;
    }

    Node *temp = menuHead;
    while (temp->next != NULL)
        temp = temp->next;

    temp->next = newNode;
}

Node* findFood(int id) {
    Node *temp = menuHead;
    while (temp != NULL) {
        if (temp->food.id == id)
            return temp;
        temp = temp->next;
    }
    return NULL;
}

int countMenu(void) {
    int count = 0;
    Node *temp = menuHead;
    while (temp) {
        count++;
        temp = temp->next;
    }
    return count;
}

void displayMenu(void) {
    Node *temp = menuHead;

    line();
    printf("                    CANTEEN MENU\n");
    line();
    printf("%-6s %-30s %-12s %-10s\n", "ID", "ITEM", "PRICE", "STOCK");
    line();

    if (!temp) {
        printf("No food items available.\n");
    }

    while (temp) {
        printf("%-6d %-30s Rs.%-9.2f %-10d\n",
               temp->food.id, temp->food.name,
               temp->food.price, temp->food.stock);
        temp = temp->next;
    }
    line();
}

/* ---------- Sorting ---------- */
int menuToArray(Food arr[]) {
    int i = 0;
    Node *temp = menuHead;
    while (temp) {
        arr[i++] = temp->food;
        temp = temp->next;
    }
    return i;
}

void sortByPrice(void) {
    int n = countMenu();
    if (n < 2) {
        printf("\nNot enough items to sort.\n");
        return;
    }

    Food *arr = (Food*)malloc(n * sizeof(Food));
    n = menuToArray(arr);

    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j].price > arr[j + 1].price) {
                Food temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }

    printf("\nMenu sorted by price (low to high):\n");
    line();
    printf("%-6s %-30s %-12s %-10s\n", "ID", "ITEM", "PRICE", "STOCK");
    line();
    for (int i = 0; i < n; i++)
        printf("%-6d %-30s Rs.%-9.2f %-10d\n",
               arr[i].id, arr[i].name, arr[i].price, arr[i].stock);
    line();

    free(arr);
}

void sortByName(void) {
    int n = countMenu();
    if (n < 2) {
        printf("\nNot enough items to sort.\n");
        return;
    }

    Food *arr = (Food*)malloc(n * sizeof(Food));
    n = menuToArray(arr);

    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (strcasecmp(arr[j].name, arr[j + 1].name) > 0) {
                Food temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }

    printf("\nMenu sorted alphabetically:\n");
    line();
    printf("%-6s %-30s %-12s %-10s\n", "ID", "ITEM", "PRICE", "STOCK");
    line();
    for (int i = 0; i < n; i++)
        printf("%-6d %-30s Rs.%-9.2f %-10d\n",
               arr[i].id, arr[i].name, arr[i].price, arr[i].stock);
    line();

    free(arr);
}

/* ---------- Searching ---------- */
void searchFood(void) {
    char keyword[50];
    int found = 0;

    printf("\nEnter food name to search: ");
    fgets(keyword, sizeof(keyword), stdin);
    keyword[strcspn(keyword, "\n")] = '\0';

    line();
    printf("Search Results:\n");
    line();

    Node *temp = menuHead;
    while (temp) {
        char nameCopy[50];
        char keyCopy[50];

        strcpy(nameCopy, temp->food.name);
        strcpy(keyCopy, keyword);

        for (int i = 0; nameCopy[i]; i++)
            nameCopy[i] = (char)tolower((unsigned char)nameCopy[i]);
        for (int i = 0; keyCopy[i]; i++)
            keyCopy[i] = (char)tolower((unsigned char)keyCopy[i]);

        if (strstr(nameCopy, keyCopy)) {
            printf("ID: %d | %s | Rs. %.2f | Stock: %d\n",
                   temp->food.id, temp->food.name,
                   temp->food.price, temp->food.stock);
            found = 1;
        }
        temp = temp->next;
    }

    if (!found)
        printf("No matching food item found.\n");

    line();
}

/* ---------- Queue ---------- */
void initQueue(void) {
    orderQueue.front = 0;
    orderQueue.rear = -1;
}

int isQueueEmpty(void) {
    return orderQueue.rear < orderQueue.front;
}

int isQueueFull(void) {
    return orderQueue.rear == MAX_QUEUE - 1;
}

void enqueue(Order order) {
    if (isQueueFull()) {
        printf("Order queue is full!\n");
        return;
    }
    orderQueue.items[++orderQueue.rear] = order;
}

Order dequeue(void) {
    Order empty = {0};
    if (isQueueEmpty())
        return empty;

    Order order = orderQueue.items[orderQueue.front++];

    if (orderQueue.front > orderQueue.rear) {
        orderQueue.front = 0;
        orderQueue.rear = -1;
    }

    return order;
}

void displayPendingOrders(void) {
    if (isQueueEmpty()) {
        printf("\nNo pending orders.\n");
        return;
    }

    line();
    printf("                    PENDING ORDERS\n");
    line();

    for (int i = orderQueue.front; i <= orderQueue.rear; i++) {
        Order o = orderQueue.items[i];
        printf("Order #%d | Customer: %s | %s x%d | Total: Rs. %.2f\n",
               o.orderId, o.customer, o.foodName, o.quantity, o.total);
    }
    line();
}

/* ---------- Stack: Cancelled Orders ---------- */
void initStack(void) {
    cancelledOrders.top = -1;
}

int isStackEmpty(void) {
    return cancelledOrders.top == -1;
}

int isStackFull(void) {
    return cancelledOrders.top == MAX_STACK - 1;
}

void pushCancelled(Order order) {
    if (isStackFull()) {
        printf("Cancelled order stack is full!\n");
        return;
    }
    cancelledOrders.items[++cancelledOrders.top] = order;
}

void displayCancelledOrders(void) {
    if (isStackEmpty()) {
        printf("\nNo cancelled orders.\n");
        return;
    }

    line();
    printf("                 CANCELLED ORDERS (STACK)\n");
    line();

    for (int i = cancelledOrders.top; i >= 0; i--) {
        Order o = cancelledOrders.items[i];
        printf("Order #%d | Customer: %s | %s x%d | Rs. %.2f\n",
               o.orderId, o.customer, o.foodName, o.quantity, o.total);
    }
    line();
}

/* ---------- File Handling ---------- */
void saveMenu(void) {
    FILE *fp = fopen(MENU_FILE, "wb");
    if (!fp) {
        printf("Warning: Could not save menu.\n");
        return;
    }

    Node *temp = menuHead;
    while (temp) {
        fwrite(&temp->food, sizeof(Food), 1, fp);
        temp = temp->next;
    }
    fclose(fp);
}

void loadMenu(void) {
    FILE *fp = fopen(MENU_FILE, "rb");
    if (!fp)
        return;

    Food food;
    while (fread(&food, sizeof(Food), 1, fp) == 1)
        insertFood(food);

    fclose(fp);
}

void saveOrder(Order order) {
    FILE *fp = fopen(ORDER_FILE, "ab");
    if (!fp) {
        printf("Warning: Could not save order history.\n");
        return;
    }

    fwrite(&order, sizeof(Order), 1, fp);
    fclose(fp);
}

void showOrderHistory(void) {
    FILE *fp = fopen(ORDER_FILE, "rb");
    if (!fp) {
        printf("\nNo order history found.\n");
        return;
    }

    Order order;
    int found = 0;

    line();
    printf("                     ORDER HISTORY\n");
    line();

    while (fread(&order, sizeof(Order), 1, fp) == 1) {
        printf("Order #%d | %-20s | %-25s | Qty: %d | Rs. %.2f\n",
               order.orderId, order.customer, order.foodName,
               order.quantity, order.total);
        found = 1;
    }

    if (!found)
        printf("No completed orders yet.\n");

    line();
    fclose(fp);
}

/* ---------- Menu Management ---------- */
void addFood(void) {
    Food food;

    printf("\nEnter food ID: ");
    if (scanf("%d", &food.id) != 1) {
        clearInput();
        printf("Invalid ID.\n");
        return;
    }
    clearInput();

    if (findFood(food.id)) {
        printf("Food ID already exists.\n");
        return;
    }

    printf("Enter food name: ");
    fgets(food.name, sizeof(food.name), stdin);
    food.name[strcspn(food.name, "\n")] = '\0';

    printf("Enter price: Rs. ");
    if (scanf("%f", &food.price) != 1 || food.price < 0) {
        clearInput();
        printf("Invalid price.\n");
        return;
    }

    printf("Enter stock quantity: ");
    if (scanf("%d", &food.stock) != 1 || food.stock < 0) {
        clearInput();
        printf("Invalid stock.\n");
        return;
    }
    clearInput();

    insertFood(food);
    saveMenu();

    printf("\nFood item added successfully!\n");
}

void updateStock(void) {
    int id, stock;

    displayMenu();

    printf("\nEnter food ID: ");
    if (scanf("%d", &id) != 1) {
        clearInput();
        printf("Invalid ID.\n");
        return;
    }

    Node *node = findFood(id);
    if (!node) {
        clearInput();
        printf("Food item not found.\n");
        return;
    }

    printf("Enter new stock quantity: ");
    if (scanf("%d", &stock) != 1 || stock < 0) {
        clearInput();
        printf("Invalid stock.\n");
        return;
    }
    clearInput();

    node->food.stock = stock;
    saveMenu();

    printf("Stock updated successfully!\n");
}

/* ---------- Ordering ---------- */
void placeOrder(void) {
    if (isQueueFull()) {
        printf("\nOrder queue is full. Please try later.\n");
        return;
    }

    char customer[50];
    int id, quantity;

    displayMenu();

    printf("\nEnter customer name: ");
    fgets(customer, sizeof(customer), stdin);
    customer[strcspn(customer, "\n")] = '\0';

    printf("Enter food ID: ");
    if (scanf("%d", &id) != 1) {
        clearInput();
        printf("Invalid food ID.\n");
        return;
    }

    Node *foodNode = findFood(id);
    if (!foodNode) {
        clearInput();
        printf("Food item not found.\n");
        return;
    }

    printf("Enter quantity: ");
    if (scanf("%d", &quantity) != 1 || quantity <= 0) {
        clearInput();
        printf("Invalid quantity.\n");
        return;
    }
    clearInput();

    if (quantity > foodNode->food.stock) {
        printf("Sorry! Only %d item(s) available.\n", foodNode->food.stock);
        return;
    }

    Order order;
    order.orderId = nextOrderId++;
    strcpy(order.customer, customer);
    order.foodId = foodNode->food.id;
    strcpy(order.foodName, foodNode->food.name);
    order.quantity = quantity;
    order.total = foodNode->food.price * quantity;

    foodNode->food.stock -= quantity;
    enqueue(order);
    saveMenu();

    printf("\n---------------- ORDER RECEIPT ----------------\n");
    printf("Order ID : %d\n", order.orderId);
    printf("Customer : %s\n", order.customer);
    printf("Item     : %s\n", order.foodName);
    printf("Quantity : %d\n", order.quantity);
    printf("Price    : Rs. %.2f\n", foodNode->food.price);
    printf("Total    : Rs. %.2f\n", order.total);
    printf("Status   : Pending\n");
    printf("-----------------------------------------------\n");
    printf("Order placed successfully!\n");
}

void serveNextOrder(void) {
    if (isQueueEmpty()) {
        printf("\nNo pending orders to serve.\n");
        return;
    }

    Order order = dequeue();
    saveOrder(order);

    printf("\nOrder #%d served successfully!\n", order.orderId);
    printf("Customer: %s | %s x%d | Total: Rs. %.2f\n",
           order.customer, order.foodName, order.quantity, order.total);
}

void cancelLatestOrder(void) {
    if (isQueueEmpty()) {
        printf("\nNo pending order to cancel.\n");
        return;
    }

    /* Cancel the latest pending order (LIFO behavior for cancellation). */
    int index = orderQueue.rear;
    Order order = orderQueue.items[index];

    Node *foodNode = findFood(order.foodId);
    if (foodNode)
        foodNode->food.stock += order.quantity;

    pushCancelled(order);

    for (int i = index; i < orderQueue.rear; i++)
        orderQueue.items[i] = orderQueue.items[i + 1];

    orderQueue.rear--;

    if (orderQueue.rear < orderQueue.front) {
        orderQueue.front = 0;
        orderQueue.rear = -1;
    }

    saveMenu();

    printf("\nOrder #%d cancelled successfully.\n", order.orderId);
    printf("Stock has been restored.\n");
}

/* ---------- Statistics ---------- */
void showStatistics(void) {
    int items = 0, stock = 0, lowStock = 0;
    float inventoryValue = 0.0f;

    Node *temp = menuHead;
    while (temp) {
        items++;
        stock += temp->food.stock;
        inventoryValue += temp->food.price * temp->food.stock;
        if (temp->food.stock <= 5)
            lowStock++;
        temp = temp->next;
    }

    printf("\n");
    line();
    printf("                    CANTEEN STATISTICS\n");
    line();
    printf("Food Items           : %d\n", items);
    printf("Total Stock Units    : %d\n", stock);
    printf("Low Stock Items      : %d\n", lowStock);
    printf("Inventory Value      : Rs. %.2f\n", inventoryValue);
    printf("Pending Orders       : %d\n",
           isQueueEmpty() ? 0 : orderQueue.rear - orderQueue.front + 1);
    printf("Cancelled Orders     : %d\n", cancelledOrders.top + 1);
    line();
}

/* ---------- Default Data ---------- */
void createDefaultMenu(void) {
    if (menuHead != NULL)
        return;

    Food defaults[] = {
        {1, "Veg Burger", 60.0f, 20},
        {2, "Cheese Pizza", 120.0f, 15},
        {3, "Masala Dosa", 80.0f, 18},
        {4, "Veg Chowmein", 70.0f, 20},
        {5, "Samosa", 15.0f, 40},
        {6, "Cold Coffee", 50.0f, 25},
        {7, "Masala Tea", 20.0f, 35},
        {8, "Fresh Lime", 35.0f, 25}
    };

    int n = sizeof(defaults) / sizeof(defaults[0]);
    for (int i = 0; i < n; i++)
        insertFood(defaults[i]);

    saveMenu();
}

/* ---------- Menus ---------- */
void customerMenu(void) {
    int choice;

    do {
        printf("\n");
        line();
        printf("                    CUSTOMER MENU\n");
        line();
        printf("1. View Food Menu\n");
        printf("2. Search Food\n");
        printf("3. Sort Menu by Price\n");
        printf("4. Sort Menu by Name\n");
        printf("5. Place Order\n");
        printf("6. View Pending Orders\n");
        printf("7. Cancel Latest Pending Order\n");
        printf("8. Back to Main Menu\n");
        line();
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            clearInput();
            printf("Invalid choice.\n");
            continue;
        }
        clearInput();

        switch (choice) {
            case 1: displayMenu(); pauseScreen(); break;
            case 2: searchFood(); pauseScreen(); break;
            case 3: sortByPrice(); pauseScreen(); break;
            case 4: sortByName(); pauseScreen(); break;
            case 5: placeOrder(); pauseScreen(); break;
            case 6: displayPendingOrders(); pauseScreen(); break;
            case 7: cancelLatestOrder(); pauseScreen(); break;
            case 8: break;
            default: printf("Please select a valid option.\n");
        }
    } while (choice != 8);
}

void adminMenu(void) {
    int choice;

    do {
        printf("\n");
        line();
        printf("                     ADMIN PANEL\n");
        line();
        printf("1. View Menu\n");
        printf("2. Add Food Item\n");
        printf("3. Update Stock\n");
        printf("4. View Pending Orders\n");
        printf("5. Serve Next Order\n");
        printf("6. View Cancelled Orders\n");
        printf("7. View Order History\n");
        printf("8. View Statistics\n");
        printf("9. Back to Main Menu\n");
        line();
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            clearInput();
            printf("Invalid choice.\n");
            continue;
        }
        clearInput();

        switch (choice) {
            case 1: displayMenu(); pauseScreen(); break;
            case 2: addFood(); pauseScreen(); break;
            case 3: updateStock(); pauseScreen(); break;
            case 4: displayPendingOrders(); pauseScreen(); break;
            case 5: serveNextOrder(); pauseScreen(); break;
            case 6: displayCancelledOrders(); pauseScreen(); break;
            case 7: showOrderHistory(); pauseScreen(); break;
            case 8: showStatistics(); pauseScreen(); break;
            case 9: break;
            default: printf("Please select a valid option.\n");
        }
    } while (choice != 9);
}

int main(void) {
    int choice;

    initQueue();
    initStack();
    loadMenu();
    createDefaultMenu();

    printf("\n");
    line();
    printf("              CANTEEN MANAGEMENT SYSTEM\n");
    printf("                 C / DSA PROJECT\n");
    line();
    printf("        Efficient food ordering and management\n");
    line();

    do {
        printf("\n");
        line();
        printf("                     MAIN MENU\n");
        line();
        printf("1. Customer / Order Management\n");
        printf("2. Admin Panel\n");
        printf("3. Exit\n");
        line();
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            clearInput();
            printf("Invalid choice. Please enter a number.\n");
            continue;
        }
        clearInput();

        switch (choice) {
            case 1:
                customerMenu();
                break;
            case 2:
                adminMenu();
                break;
            case 3:
                saveMenu();
                printf("\nThank you for using Canteen Management System!\n");
                break;
            default:
                printf("Please select a valid option.\n");
        }
    } while (choice != 3);

    return 0;
}