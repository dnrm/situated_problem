// * Daniel Medina | A01286980
// * Monday September 22nd, 2025

#include <stdio.h>
#include <time.h>

#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "src/colors.h"
#include "src/sorts.h"
#include "src/utils.h"

// * Project Description

// This project reads a list of food orders from a text file and sorts them
// by restaurant name using iterative quicksort with a linked list and stack.
// The user can then search for orders from a specific restaurant, which are
// displayed sorted by date. The results show the count of orders and can be
// saved to an output file.

// Throughout the file there are constants in uppercase, which are color code
// constantes defined in the src/colors.h file. They are used to color the text
// in the terminal and give the project a better appearance.

using namespace std;

// * Main function

// * Functions from the "utils.cpp" file:
// * – parse_order_line
// * – save_to_file
// * – filter_orders_by_restaurant
// * – get_string

// * Functions from the "sorts.cpp" file:
// * – iterative_quick_sort_linked_list (with linked list and stack)
// * – vector_to_linked_list
// * – linked_list_to_vector
// * – merge_sort (for sorting filtered results by date)

int main() {
    fstream OrdersFile("orders.txt");

    vector<Order> orders = {};

    string text;

    // Loop over every line and create a vector of Orders.

    if (OrdersFile.is_open()) {
        while (getline(OrdersFile, text)) {
            Order order = parse_order_line(text);
            orders.push_back(order);
        }
    } else {
        cout << ERROR_HEADER << " ERROR " << RESET << " " << ERROR_STYLE
             << "Failed to open orders.txt" << RESET << endl;
        return 1;
    }

    OrdersFile.close();

    cout << INFO_STYLE << "Loaded " << orders.size() << " orders from orders.txt"
         << RESET << "\n\n";

    // ===== STEP 1: Sort by Restaurant Name using Iterative QuickSort =====
    // with Linked List and Stack
    
    cout << SECTION_HEADER << "===== Sorting by Restaurant Name =====    " 
         << RESET << "\n" << endl;

    cout << INFO_STYLE 
         << "Using Iterative QuickSort with Linked List and Stack..." 
         << RESET << endl;

    // Setup the chronometer to time the algorithm
    auto start = chrono::high_resolution_clock::now();
    
    // Convert vector to linked list
    OrderNode* head = vector_to_linked_list(orders);
    
    // Sort by restaurant name using iterative quicksort
    head = iterative_quick_sort_linked_list(head, true);
    
    // Convert back to vector
    vector<Order> sorted_by_restaurant = linked_list_to_vector(head);
    
    // Free the linked list memory
    free_linked_list(head);

    // End the timer
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, std::milli> duration = end - start;

    cout << INFO_STYLE << "Sorting took " << SUCCESS_STYLE
         << duration.count() << " ms" << RESET << "\n";
    cout << "\n";

    // Display first 10 sorted orders by restaurant
    cout << SECTION_HEADER 
         << "===== First 10 Orders (Sorted by Restaurant) =====    " 
         << RESET << "\n" << endl;

    int display_count = min(10, (int)sorted_by_restaurant.size());
    for (int i = 0; i < display_count; i++) {
        char time_str[20];
        struct tm* timeinfo = localtime(&sorted_by_restaurant[i].timestamp);
        strftime(time_str, sizeof(time_str), "%b %d %H:%M:%S", timeinfo);

        cout << BOLD << (i + 1) << ". " << RESET << RESTAURANT_STYLE
             << sorted_by_restaurant[i].restaurant << RESET << " | " 
             << TIMESTAMP_STYLE << time_str << RESET << " | "
             << ITEM_STYLE << sorted_by_restaurant[i].item << RESET << " | "
             << PRICE_STYLE << "$" << sorted_by_restaurant[i].price << RESET
             << endl;
    }
    cout << "\n";

    // ===== STEP 2: Request Restaurant Name from User =====
    
    cout << SECTION_HEADER << "===== Restaurant Search =====    " << RESET
         << "\n" << endl;

    string restaurant_name = get_string("Enter restaurant name: ");
    
    if (restaurant_name.empty()) {
        cout << ERROR_HEADER << " ERROR " << RESET << " " << ERROR_STYLE
             << "Restaurant name cannot be empty." << RESET << endl;
        return 1;
    }

    // ===== STEP 3: Filter by Restaurant =====
    
    vector<Order> restaurant_orders = 
        filter_orders_by_restaurant(sorted_by_restaurant, restaurant_name);

    if (restaurant_orders.empty()) {
        cout << WARNING_STYLE << "No orders found for restaurant: " 
             << restaurant_name << RESET << endl;
        return 0;
    }

    // ===== STEP 4: Sort Filtered Results by Date =====
    
    cout << "\n" << INFO_STYLE << "Sorting " << restaurant_orders.size() 
         << " order(s) by date..." << RESET << endl;
    
    vector<Order> date_sorted = merge_sort(restaurant_orders);

    // ===== STEP 5: Display Results =====
    
    cout << "\n" << SECTION_HEADER 
         << "===== Results for '" << restaurant_name << "' =====" 
         << RESET << endl;
    cout << SUCCESS_STYLE << "Found " << date_sorted.size() 
         << " order(s)" << RESET << endl;
    cout << DIM << "-------------------------" << RESET << endl;

    for (size_t i = 0; i < date_sorted.size(); i++) {
        char time_str[20];
        struct tm* timeinfo = localtime(&date_sorted[i].timestamp);
        strftime(time_str, sizeof(time_str), "%b %d %H:%M:%S", timeinfo);

        cout << BOLD << (i + 1) << ". " << RESET << TIMESTAMP_STYLE 
             << time_str << RESET << " | " << ITEM_STYLE 
             << date_sorted[i].item << RESET << " | " << PRICE_STYLE 
             << "$" << date_sorted[i].price << RESET << endl;
    }
    cout << DIM << "-------------------------" << RESET << endl;

    // ===== STEP 6: Save to File =====
    
    cout << "\n" << INFO_STYLE 
         << "Saving results to restaurant_results.txt..." 
         << RESET << endl;
    
    save_to_file("restaurant_results.txt", date_sorted);
    
    cout << SUCCESS_HEADER << " SUCCESS " << RESET << " " << SUCCESS_STYLE
         << "Results saved to restaurant_results.txt!" << RESET << endl;

    return 0;
}