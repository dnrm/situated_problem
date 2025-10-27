// * Daniel Medina | A01286980
// * Monday September 22nd, 2025

// ❗️ All of the sorts sort the data based on the `timestamp` field.

// Why size_t is used:
// https://www.geeksforgeeks.org/cpp/difference-between-int-and-size_t-in-cpp/

#include "sorts.h"

// * ========= BEGIN SORT IMPLEMENTATIONS =========

// 1. Insertion Sort

vector<Order> insertion_sort(vector<Order> orders_copy) {
    size_t n = orders_copy.size();
    for (size_t i = 1; i < n; i++) {
        Order key = orders_copy[i];
        int j = i - 1;  // Keep as int since we need to check j >= 0
        while (j >= 0 && orders_copy[j].timestamp > key.timestamp) {
            orders_copy[j + 1] = orders_copy[j];
            j = j - 1;
        }
        orders_copy[j + 1] = key;
    }
    return orders_copy;
}

// 2. Selection Sort

vector<Order> selection_sort(vector<Order> orders_copy) {
    size_t n = orders_copy.size();
    for (size_t i = 0; i < n - 1; i++) {
        size_t min_idx = i;
        for (size_t j = i + 1; j < n; j++) {
            if (orders_copy[j].timestamp < orders_copy[min_idx].timestamp) {
                min_idx = j;
            }
        }
        swap(orders_copy[i], orders_copy[min_idx]);
    }
    return orders_copy;
}

// 3. Swap Sort

vector<Order> swap_sort(vector<Order> orders_copy) {
    size_t n = orders_copy.size();
    bool swapped;
    do {
        swapped = false;
        for (size_t i = 0; i < n - 1; i++) {
            if (orders_copy[i].timestamp > orders_copy[i + 1].timestamp) {
                swap(orders_copy[i], orders_copy[i + 1]);
                swapped = true;
            }
        }
    } while (swapped);
    return orders_copy;
}

// 4. Bubble Sort

vector<Order> bubble_sort(vector<Order> orders_copy) {
    size_t n = orders_copy.size();
    for (size_t i = 0; i < n - 1; i++) {
        for (size_t j = 0; j < n - i - 1; j++) {
            if (orders_copy[j].timestamp > orders_copy[j + 1].timestamp) {
                swap(orders_copy[j], orders_copy[j + 1]);
            }
        }
    }
    return orders_copy;
}

// 5. Merge Sort (Recursive)

// This is the helper function to make it recursive
vector<Order> merge(const vector<Order>& left, const vector<Order>& right) {
    vector<Order> result;
    size_t i = 0, j = 0;
    while (i < left.size() && j < right.size()) {
        if (left[i].timestamp <= right[j].timestamp) {
            result.push_back(left[i]);
            i++;
        } else {
            result.push_back(right[j]);
            j++;
        }
    }
    while (i < left.size()) {
        result.push_back(left[i]);
        i++;
    }
    while (j < right.size()) {
        result.push_back(right[j]);
        j++;
    }
    return result;
}

vector<Order> merge_sort(vector<Order> orders_copy) {
    if (orders_copy.size() <= 1) {
        return orders_copy;
    }
    size_t mid = orders_copy.size() / 2;
    vector<Order> left(orders_copy.begin(), orders_copy.begin() + mid);
    vector<Order> right(orders_copy.begin() + mid, orders_copy.end());
    return merge(merge_sort(left), merge_sort(right));
}

// 6. Quick Sort (with median-of-three pivot selection)

// Helper function to find median of three elements
size_t median_of_three(const vector<Order>& orders, size_t first, size_t mid,
                       size_t last) {
    time_t first_val = orders[first].timestamp;
    time_t mid_val = orders[mid].timestamp;
    time_t last_val = orders[last].timestamp;

    if ((first_val <= mid_val && mid_val <= last_val) ||
        (last_val <= mid_val && mid_val <= first_val)) {
        return mid;
    } else if ((mid_val <= first_val && first_val <= last_val) ||
               (last_val <= first_val && first_val <= mid_val)) {
        return first;
    } else {
        return last;
    }
}

vector<Order> quick_sort(vector<Order> orders_copy) {
    if (orders_copy.size() <= 1) {
        return orders_copy;
    }

    // Using median-of-three approach for pivot selection
    size_t first = 0;
    size_t last = orders_copy.size() - 1;
    size_t mid = orders_copy.size() / 2;
    size_t pivot_index = median_of_three(orders_copy, first, mid, last);
    Order pivot = orders_copy[pivot_index];

    vector<Order> less;
    vector<Order> equal;
    vector<Order> greater;
    for (const auto& order : orders_copy) {
        if (order.timestamp < pivot.timestamp) {
            less.push_back(order);
        } else if (order.timestamp == pivot.timestamp) {
            equal.push_back(order);
        } else {
            greater.push_back(order);
        }
    }
    vector<Order> sorted_less = quick_sort(less);
    vector<Order> sorted_greater = quick_sort(greater);
    sorted_less.insert(sorted_less.end(), equal.begin(), equal.end());
    sorted_less.insert(sorted_less.end(), sorted_greater.begin(),
                       sorted_greater.end());
    return sorted_less;
}

// ========= ITERATIVE QUICKSORT WITH LINKED LIST AND STACK =========

// Helper to convert vector to linked list
OrderNode* vector_to_linked_list(const vector<Order>& orders) {
    if (orders.empty()) return nullptr;
    
    OrderNode* head = new OrderNode(orders[0]);
    OrderNode* current = head;
    
    for (size_t i = 1; i < orders.size(); i++) {
        current->next = new OrderNode(orders[i]);
        current = current->next;
    }
    
    return head;
}

// Helper to convert linked list to vector
vector<Order> linked_list_to_vector(OrderNode* head) {
    vector<Order> result;
    OrderNode* current = head;
    
    while (current != nullptr) {
        result.push_back(current->data);
        current = current->next;
    }
    
    return result;
}

// Helper to free linked list memory
void free_linked_list(OrderNode* head) {
    while (head != nullptr) {
        OrderNode* temp = head;
        head = head->next;
        delete temp;
    }
}

// Stack structure for iterative quicksort
struct StackNode {
    OrderNode* start;
    OrderNode* end;
    
    StackNode(OrderNode* s, OrderNode* e) : start(s), end(e) {}
};

// Get the last node of a linked list
OrderNode* get_last_node(OrderNode* head) {
    while (head != nullptr && head->next != nullptr) {
        head = head->next;
    }
    return head;
}

// Partition function for linked list
// Returns the pivot node after partitioning
OrderNode* partition_linked_list(OrderNode* start, OrderNode* end, bool sort_by_restaurant) {
    if (start == nullptr || start == end) {
        return start;
    }
    
    // Use the last element as pivot
    Order pivot = end->data;
    OrderNode* i = nullptr;  // Pointer to the last element less than pivot
    OrderNode* current = start;
    
    while (current != end) {
        bool should_swap = false;
        
        if (sort_by_restaurant) {
            // Compare by restaurant name (case-insensitive)
            string curr_restaurant = current->data.restaurant;
            string pivot_restaurant = pivot.restaurant;
            
            // Convert to lowercase for comparison
            for (auto& c : curr_restaurant) c = tolower(c);
            for (auto& c : pivot_restaurant) c = tolower(c);
            
            should_swap = (curr_restaurant < pivot_restaurant);
        } else {
            // Compare by timestamp
            should_swap = (current->data.timestamp < pivot.timestamp);
        }
        
        if (should_swap) {
            if (i == nullptr) {
                i = start;
            } else {
                i = i->next;
            }
            swap(i->data, current->data);
        }
        
        current = current->next;
    }
    
    // Place pivot in correct position
    if (i == nullptr) {
        swap(start->data, end->data);
        return start;
    } else {
        swap(i->next->data, end->data);
        return i->next;
    }
}

// Iterative QuickSort using Linked List and Stack
OrderNode* iterative_quick_sort_linked_list(OrderNode* head, bool sort_by_restaurant) {
    if (head == nullptr || head->next == nullptr) {
        return head;
    }
    
    // Manual stack implementation using vector
    vector<StackNode> stack;
    
    // Get the last node
    OrderNode* last = get_last_node(head);
    
    // Push initial range onto stack
    stack.push_back(StackNode(head, last));
    
    // Process stack iteratively
    while (!stack.empty()) {
        // Pop from stack
        StackNode current_range = stack.back();
        stack.pop_back();
        
        OrderNode* start = current_range.start;
        OrderNode* end = current_range.end;
        
        // Skip if range is invalid or single element
        if (start == nullptr || end == nullptr || start == end) {
            continue;
        }
        
        // Partition the list
        OrderNode* pivot = partition_linked_list(start, end, sort_by_restaurant);
        
        // Find the node before pivot
        OrderNode* before_pivot = nullptr;
        OrderNode* temp = start;
        while (temp != pivot) {
            before_pivot = temp;
            temp = temp->next;
        }
        
        // Push right partition onto stack (pivot+1 to end)
        if (pivot->next != nullptr && pivot != end) {
            stack.push_back(StackNode(pivot->next, end));
        }
        
        // Push left partition onto stack (start to pivot-1)
        if (before_pivot != nullptr && start != pivot) {
            stack.push_back(StackNode(start, before_pivot));
        }
    }
    
    return head;
}