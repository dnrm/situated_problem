// * Daniel Medina | A01286980
// * Monday September 22nd, 2025

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

// ========== ANSI Color Codes ==========
const string RESET = "\033[0m";
const string BOLD = "\033[1m";
const string DIM = "\033[2m";
const string SECTION_HEADER = "\033[1;44m";
const string SUCCESS_HEADER = "\033[1;42m";
const string INFO_STYLE = "\033[36m";
const string SUCCESS_STYLE = "\033[32m";
const string RESTAURANT_STYLE = "\033[33m";
const string PRICE_STYLE = "\033[35m";

// ========== Restaurant Data Structure ==========
struct RestaurantData {
    string name;
    int orderCount;
    int totalSales;
    
    RestaurantData() : name(""), orderCount(0), totalSales(0) {}
    RestaurantData(string n) : name(n), orderCount(0), totalSales(0) {}
};

// ========== AVL Tree Node ==========
struct AVLNode {
    RestaurantData data;
    AVLNode* left;
    AVLNode* right;
    int height;
    
    AVLNode(string name) : data(name), left(nullptr), right(nullptr), height(1) {}
};

// ========== AVL Tree Class ==========
class AVLTree {
private:
    AVLNode* root;
    
    // Get height of node
    int getHeight(AVLNode* node) {
        return (node == nullptr) ? 0 : node->height;
    }
    
    // Get balance factor
    int getBalance(AVLNode* node) {
        return (node == nullptr) ? 0 : getHeight(node->left) - getHeight(node->right);
    }
    
    // Update height of node
    void updateHeight(AVLNode* node) {
        if (node != nullptr) {
            node->height = 1 + max(getHeight(node->left), getHeight(node->right));
        }
    }
    
    // Right rotation
    AVLNode* rotateRight(AVLNode* y) {
        AVLNode* x = y->left;
        AVLNode* T2 = x->right;
        
        x->right = y;
        y->left = T2;
        
        updateHeight(y);
        updateHeight(x);
        
        return x;
    }
    
    // Left rotation
    AVLNode* rotateLeft(AVLNode* x) {
        AVLNode* y = x->right;
        AVLNode* T2 = y->left;
        
        y->left = x;
        x->right = T2;
        
        updateHeight(x);
        updateHeight(y);
        
        return y;
    }
    
    // Insert helper with balancing
    AVLNode* insertHelper(AVLNode* node, string name) {
        // Standard BST insertion
        if (node == nullptr) {
            return new AVLNode(name);
        }
        
        if (name < node->data.name) {
            node->left = insertHelper(node->left, name);
        } else if (name > node->data.name) {
            node->right = insertHelper(node->right, name);
        } else {
            return node; // Duplicate names not allowed
        }
        
        // Update height
        updateHeight(node);
        
        // Get balance factor
        int balance = getBalance(node);
        
        // Left-Left Case
        if (balance > 1 && name < node->left->data.name) {
            return rotateRight(node);
        }
        
        // Right-Right Case
        if (balance < -1 && name > node->right->data.name) {
            return rotateLeft(node);
        }
        
        // Left-Right Case
        if (balance > 1 && name > node->left->data.name) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        
        // Right-Left Case
        if (balance < -1 && name < node->right->data.name) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }
        
        return node;
    }
    
    // Search helper
    AVLNode* searchHelper(AVLNode* node, string name) {
        if (node == nullptr || node->data.name == name) {
            return node;
        }
        
        if (name < node->data.name) {
            return searchHelper(node->left, name);
        }
        
        return searchHelper(node->right, name);
    }
    
    // Inorder traversal helper
    void inorderHelper(AVLNode* node, vector<RestaurantData>& result) {
        if (node != nullptr) {
            inorderHelper(node->left, result);
            result.push_back(node->data);
            inorderHelper(node->right, result);
        }
    }
    
    // Delete tree helper
    void deleteTree(AVLNode* node) {
        if (node != nullptr) {
            deleteTree(node->left);
            deleteTree(node->right);
            delete node;
        }
    }
    
public:
    AVLTree() : root(nullptr) {}
    
    ~AVLTree() {
        deleteTree(root);
    }
    
    void insert(string name) {
        root = insertHelper(root, name);
    }
    
    AVLNode* search(string name) {
        return searchHelper(root, name);
    }
    
    void addOrder(string name, int price) {
        AVLNode* node = search(name);
        if (node == nullptr) {
            insert(name);
            node = search(name);
        }
        node->data.orderCount++;
        node->data.totalSales += price;
    }
    
    vector<RestaurantData> getAllRestaurants() {
        vector<RestaurantData> result;
        inorderHelper(root, result);
        return result;
    }
    
    int getTreeHeight() {
        return getHeight(root);
    }
};

// ========== Parsing Function ==========
void parseOrderLine(const string& line, string& restaurant, int& price) {
    size_t rPos = line.find("R:");
    size_t oPos = line.find("O:");
    
    if (rPos == string::npos || oPos == string::npos) {
        restaurant = "";
        price = 0;
        return;
    }
    
    restaurant = line.substr(rPos + 2, oPos - rPos - 3);
    
    size_t priceStart = line.find("(", oPos);
    size_t priceEnd = line.find(")", priceStart);
    
    if (priceStart != string::npos && priceEnd != string::npos) {
        string priceStr = line.substr(priceStart + 1, priceEnd - priceStart - 1);
        price = stoi(priceStr);
    } else {
        price = 0;
    }
}

// ========== Sorting Comparators ==========
bool compareByOrderCount(const RestaurantData& a, const RestaurantData& b) {
    if (a.orderCount == b.orderCount) {
        return a.name < b.name;
    }
    return a.orderCount > b.orderCount;
}

bool compareBySales(const RestaurantData& a, const RestaurantData& b) {
    if (a.totalSales == b.totalSales) {
        return a.name < b.name;
    }
    return a.totalSales > b.totalSales;
}

// ========== Save to File Function ==========
void saveToFile(const string& filename, const vector<RestaurantData>& data, bool isByOrders) {
    ofstream outFile(filename);
    
    if (!outFile.is_open()) {
        cout << "Error: Could not create file " << filename << endl;
        return;
    }
    
    for (size_t i = 0; i < data.size(); i++) {
        outFile << (i + 1) << ". " << data[i].name 
                << " | Orders: " << data[i].orderCount
                << " | Total Sales: $" << data[i].totalSales << "\n";
    }
    
    outFile.close();
}

// ========== Main Function ==========
int main() {
    cout << SECTION_HEADER << "===== Restaurant Order Analysis System =====" 
         << RESET << "\n\n";
    
    // Open the orders file
    ifstream ordersFile("orders.txt");
    
    if (!ordersFile.is_open()) {
        cout << "Error: Could not open orders.txt" << endl;
        return 1;
    }
    
    // Create AVL Tree to store restaurant data
    AVLTree restaurantTree;
    string line;
    int totalOrders = 0;
    
    // Read and process each order
    cout << INFO_STYLE << "Reading orders from orders.txt..." << RESET << endl;
    
    while (getline(ordersFile, line)) {
        string restaurant;
        int price;
        parseOrderLine(line, restaurant, price);
        
        if (!restaurant.empty()) {
            restaurantTree.addOrder(restaurant, price);
            totalOrders++;
        }
    }
    
    ordersFile.close();
    
    cout << SUCCESS_STYLE << "Successfully loaded " << totalOrders 
         << " orders!" << RESET << "\n\n";
    
    // Get all restaurants from the tree
    vector<RestaurantData> restaurants = restaurantTree.getAllRestaurants();
    
    cout << INFO_STYLE << "Found " << restaurants.size() 
         << " unique restaurants." << RESET << endl;
    cout << INFO_STYLE << "AVL Tree height: " << restaurantTree.getTreeHeight() 
         << " (balanced)" << RESET << "\n\n";
    
    // ===== Sort by Order Count =====
    cout << SECTION_HEADER << "===== Sorting by Order Count =====" 
         << RESET << "\n\n";
    
    vector<RestaurantData> sortedByOrders = restaurants;
    sort(sortedByOrders.begin(), sortedByOrders.end(), compareByOrderCount);
    
    // Display top 10 by orders
    cout << BOLD << "Top 10 Restaurants by Number of Orders:\n" << RESET;
    cout << DIM << "---------------------------------------" << RESET << "\n";
    
    int displayCount = min(10, (int)sortedByOrders.size());
    for (int i = 0; i < displayCount; i++) {
        cout << BOLD << (i + 1) << ". " << RESET << RESTAURANT_STYLE
             << sortedByOrders[i].name << RESET 
             << " | Orders: " << sortedByOrders[i].orderCount
             << " | Sales: " << PRICE_STYLE << "$" << sortedByOrders[i].totalSales 
             << RESET << "\n";
    }
    cout << "\n";
    
    // Save all restaurants sorted by orders
    saveToFile("restaurants_by_orders.txt", sortedByOrders, true);
    cout << SUCCESS_STYLE << "✓ Saved all restaurants to restaurants_by_orders.txt" 
         << RESET << "\n\n";
    
    // ===== Sort by Total Sales =====
    cout << SECTION_HEADER << "===== Sorting by Total Sales =====" 
         << RESET << "\n\n";
    
    vector<RestaurantData> sortedBySales = restaurants;
    sort(sortedBySales.begin(), sortedBySales.end(), compareBySales);
    
    // Display top 10 by sales
    cout << BOLD << "Top 10 Restaurants by Total Sales:\n" << RESET;
    cout << DIM << "---------------------------------------" << RESET << "\n";
    
    displayCount = min(10, (int)sortedBySales.size());
    for (int i = 0; i < displayCount; i++) {
        cout << BOLD << (i + 1) << ". " << RESET << RESTAURANT_STYLE
             << sortedBySales[i].name << RESET 
             << " | Sales: " << PRICE_STYLE << "$" << sortedBySales[i].totalSales 
             << RESET << " | Orders: " << sortedBySales[i].orderCount << "\n";
    }
    cout << "\n";
    
    // Save all restaurants sorted by sales
    saveToFile("restaurants_by_sales.txt", sortedBySales, false);
    cout << SUCCESS_STYLE << "✓ Saved all restaurants to restaurants_by_sales.txt" 
         << RESET << "\n\n";
    
    cout << SECTION_HEADER << "===== Analysis Complete! =====" 
         << RESET << "\n";
    
    return 0;
}