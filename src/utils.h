// * Daniel Medina | A01286980
// * Monday September 22nd, 2025

// This file contains the declarations for the utility functions used in the
// project, including time conversion and order parsing.

#ifndef UTILS_H
#define UTILS_H

#include <ctime>
#include <string>
#include <vector>

#include "colors.h"

struct Order {
    time_t timestamp;
    std::string restaurant;
    std::string item;
    int price;
};

// Function declarations
int month_to_number(const std::string& month);
time_t convert_time_to_unix(const std::string& date_str);
void parse_details(const std::string& details_str, Order& order);
Order parse_order_line(std::string line);
void save_to_file(const std::string& filename,
                  const std::vector<Order>& orders);

// New functions for date range search
time_t parse_user_date(const std::string& date_str);
std::vector<Order> filter_orders_by_date_range(const std::vector<Order>& orders,
                                               time_t start_date,
                                               time_t end_date);
std::string get_string(const std::string& prompt);
void display_filtered_orders(const std::vector<Order>& orders);
bool ask_user_to_save();

// Function to filter orders by restaurant name
std::vector<Order> filter_orders_by_restaurant(const std::vector<Order>& orders,
                                               const std::string& restaurant_name);

#endif  // UTILS_H