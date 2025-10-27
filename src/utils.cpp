// * Daniel Medina | A01286980
// * Monday September 22nd, 2025

// This file contains the definitions for the utility functions used in the
// project. These include functions for converting time strings to Unix
// timestamps, parsing order details from strings, and printing orders.

#include "utils.h"

#include <stdio.h>
#include <time.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

// Function to map month abbreviation to number (1-12)
// Input: month - string with 3-letter month abbreviation
// Output: integer from 0-11 (0 for invalid month)
// Time complexity: O(1) - searches through constant array of 12 elements

int month_to_number(const std::string& month) {
    const std::string months[] = {"ene", "Feb", "Mar", "Abr", "May", "Jun",
                                  "Jul", "Ago", "Sep", "Oct", "Nov", "Dic"};

    // Tiny linear search 👀
    for (int i = 0; i < 12; i++) {
        if (month == months[i]) {
            return i;
        }
    }
    return 0;  // invalid month
}

// Simple helper function to convert a date string to a Unix timestamp
// Input: date_str - string in format "Feb 13 12:24:25"
// Output: time_t Unix timestamp, or -1 if parsing fails
// Time complexity: O(1) - fixed parsing operations

time_t convert_time_to_unix(const std::string& date_str) {
    // Example date "Feb 13 12:24:25"

    time_t now = time(nullptr);
    struct tm* local_tm_now = localtime(&now);
    int current_year = 1900 + local_tm_now->tm_year;

    char month_buffer[4];
    int day, hour, min, sec;

    if (sscanf(date_str.c_str(), "%3s %d %d:%d:%d", month_buffer, &day, &hour,
               &min, &sec) != 5) {
        return -1;
    }

    struct tm time_info = {};

    time_info.tm_year = current_year - 1900;
    time_info.tm_mon = month_to_number(month_buffer);
    time_info.tm_mday = day;
    time_info.tm_hour = hour;
    time_info.tm_min = min;
    time_info.tm_sec = sec;

    time_info.tm_isdst = -1;

    time_t timestamp = mktime(&time_info);

    return timestamp;
}

// This is called from within the parse_order_line function to get the
// restaurant, item, and price from the details section of the line.
// Input: details_str - string containing restaurant, order, and price info
// Output: order - Order struct modified by reference with parsed data
// Time complexity: O(n) where n is length of details_str for string operations

void parse_details(const string& details_str, Order& order) {
    size_t restaurant_start = details_str.find("R:") + 2;
    size_t item_start = details_str.find(" O:") + 3;
    size_t price_start = details_str.find("(") + 1;

    // Verify that the markers are there
    if (restaurant_start == string::npos || item_start == string::npos) {
        order.restaurant = "ERROR";
        order.item = "ERROR";
        order.price = -1;
        return;
    }

    order.restaurant =
        details_str.substr(restaurant_start, item_start - restaurant_start - 3);
    order.item = details_str.substr(item_start, price_start - item_start - 1);
    order.price = stoi(details_str.substr(price_start, details_str.rfind(")")));
}

// This is the function that parses the entire line into an Order struct
// It uses the above two helper functions to do so.
// Input: line - string containing complete order information
// Output: Order struct with parsed timestamp, restaurant, item, and price
// Time complexity: O(n) where n is length of line for string operations

Order parse_order_line(string line) {
    Order result;
    istringstream iss(line);

    std::string date_section, details_section;

    iss >> std::ws;  // This is to trim whitespace apparently lol

    getline(iss, date_section, ' ');  // Gets the month
    date_section += " ";

    string temp;
    iss >> temp;  // Day
    date_section += temp + " ";
    iss >> temp;  // Time
    date_section += temp;

    getline(iss, details_section);

    getline(iss, details_section);
    // Trim the leading space from the details
    if (!details_section.empty() && details_section[0] == ' ') {
        details_section = details_section.substr(1);
    }

    result.timestamp = convert_time_to_unix(date_section);
    parse_details(details_section, result);

    return result;
}

// Input: filename - path to output file, orders - vector of Order structs to
// save Output: void - writes orders to file in readable format Time complexity:
// O(n) where n is number of orders

void save_to_file(const std::string& filename,
                  const std::vector<Order>& orders) {
    std::ofstream outfile(filename);
    if (!outfile.is_open()) {
        std::cerr << ERROR_STYLE
                  << "Error opening file for writing: " << filename << RESET
                  << std::endl;
        return;
    }

    // Save each line to the output file

    for (const auto& order : orders) {
        // Convert timestamp back to readable format
        char time_str[20];
        struct tm* timeinfo = localtime(&order.timestamp);
        strftime(time_str, sizeof(time_str), "%b %d %H:%M:%S", timeinfo);

        outfile << time_str << " R:" << order.restaurant << " O:" << order.item
                << " ($" << order.price << ")" << std::endl;
    }

    outfile.close();
}

// Input: date_str - string in format "Mon DD" (e.g., "Feb 13")
// Output: time_t timestamp for start of specified date, or -1 if parsing fails
// Time complexity: O(1) - fixed parsing operations

time_t parse_user_date(const std::string& date_str) {
    // Parse user input in format "Mon DD" to time_t
    char month_buffer[4];
    int day;

    // Validate format of user input
    if (sscanf(date_str.c_str(), "%3s %d", month_buffer, &day) != 2) {
        return -1;
    }

    // Get current year
    time_t now = time(nullptr);
    struct tm* local_tm_now = localtime(&now);
    int current_year = 1900 + local_tm_now->tm_year;

    struct tm time_info = {};
    time_info.tm_year = current_year - 1900;
    time_info.tm_mon = month_to_number(month_buffer);
    time_info.tm_mday = day;
    time_info.tm_hour = 0;
    time_info.tm_min = 0;
    time_info.tm_sec = 0;
    time_info.tm_isdst = -1;

    return mktime(&time_info);
}

// Function to filter orders within a date range
// Input: orders - vector of Order structs, start_date & end_date - time_t
// timestamps Output: vector of Order structs within the specified date range
// Time complexity: O(n) where n is number of orders in input vector

std::vector<Order> filter_orders_by_date_range(const std::vector<Order>& orders,
                                               time_t start_date,
                                               time_t end_date) {
    std::vector<Order> filtered_orders;

    // Adjust end_date to end of day (23:59:59) to include all orders on that
    // date and prevent timezone issues
    struct tm* end_tm = localtime(&end_date);
    end_tm->tm_hour = 23;
    end_tm->tm_min = 59;
    end_tm->tm_sec = 59;
    time_t adjusted_end_date = mktime(end_tm);

    for (const auto& order : orders) {
        if (order.timestamp >= start_date &&
            order.timestamp <= adjusted_end_date) {
            filtered_orders.push_back(order);
        }
    }

    return filtered_orders;
}

// Helper function to capture user input string

std::string get_string(const std::string& prompt) {
    std::string input;
    std::cout << INFO_STYLE << prompt << RESET;
    std::getline(std::cin, input);
    return input;
}

void display_filtered_orders(const std::vector<Order>& orders) {
    if (orders.empty()) {
        std::cout << WARNING_STYLE
                  << "No orders found in the specified date range." << RESET
                  << std::endl;
        return;
    }

    std::cout << "\n"
              << SECTION_HEADER << "===== Search Results =====" << RESET
              << std::endl;
    std::cout << INFO_STYLE << "Found " << orders.size()
              << " order(s):" << RESET << std::endl;
    std::cout << DIM << "-------------------------" << RESET << std::endl;

    // Loop over and print each order

    for (size_t i = 0; i < orders.size(); i++) {
        char time_str[20];
        struct tm* timeinfo = localtime(&orders[i].timestamp);
        strftime(time_str, sizeof(time_str), "%b %d %H:%M:%S", timeinfo);

        std::cout << BOLD << (i + 1) << ". " << RESET << TIMESTAMP_STYLE
                  << time_str << RESET << " | " << RESTAURANT_STYLE
                  << "Restaurant: " << orders[i].restaurant << RESET << " | "
                  << ITEM_STYLE << "Item: " << orders[i].item << RESET << " | "
                  << PRICE_STYLE << "Price: $" << orders[i].price << RESET
                  << std::endl;
    }
    std::cout << DIM << "-------------------------" << RESET << std::endl;
}

bool ask_user_to_save() {
    // Just a boolean pretty much

    std::string response;
    std::cout << "\n"
              << INFO_STYLE
              << "Would you like to save these results to a file? (y/n): "
              << RESET;
    std::getline(std::cin, response);

    return (response == "y" || response == "Y" || response == "yes" ||
            response == "Yes");
}

// Function to filter orders by restaurant name (case-insensitive)
std::vector<Order> filter_orders_by_restaurant(const std::vector<Order>& orders,
                                               const std::string& restaurant_name) {
    std::vector<Order> filtered_orders;
    
    // Convert search term to lowercase for case-insensitive comparison
    std::string search_lower = restaurant_name;
    for (auto& c : search_lower) c = tolower(c);
    
    for (const auto& order : orders) {
        std::string order_restaurant = order.restaurant;
        // Convert to lowercase for comparison
        for (auto& c : order_restaurant) c = tolower(c);
        
        if (order_restaurant == search_lower) {
            filtered_orders.push_back(order);
        }
    }
    
    return filtered_orders;
}