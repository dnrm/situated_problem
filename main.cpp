// * Daniel Medina | A01286980
// * Friday 28th of November 2025

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <queue>
#include <limits>
#include <algorithm>

using namespace std;

// ANSI color codes for terminal output
const string GREEN = "\033[32m";
const string RED = "\033[31m";
const string RESET = "\033[0m";
const string BOLD = "\033[1m";

// ============================================================================
// GRAPH CLASS - Generic weighted graph implementation using adjacency list
// ============================================================================

template<typename T>
class Graph {
private:
    unordered_map<T, vector<pair<T, int>>> adjList; // node -> [(neighbor, weight)]
    bool isDirected;

public:
    Graph(bool directed = false) : isDirected(directed) {}
    
    void addNode(const T& node) {
        if (adjList.find(node) == adjList.end()) {
            adjList[node] = vector<pair<T, int>>();
        }
    }
    
    void addEdge(const T& from, const T& to, int weight = 1) {
        addNode(from);
        addNode(to);
        adjList[from].push_back({to, weight});
        if (!isDirected) {
            adjList[to].push_back({from, weight});
        }
    }
    
    vector<pair<T, int>> getNeighbors(const T& node) const {
        auto it = adjList.find(node);
        if (it != adjList.end()) {
            return it->second;
        }
        return vector<pair<T, int>>();
    }
    
    bool hasNode(const T& node) const {
        return adjList.find(node) != adjList.end();
    }
    
    vector<T> getNodes() const {
        vector<T> nodes;
        for (const auto& pair : adjList) {
            nodes.push_back(pair.first);
        }
        return nodes;
    }
};

// ============================================================================
// HELPER STRUCTURES AND PARSING FUNCTIONS
// ============================================================================

struct Coordinate {
    int x, y;
    
    Coordinate(int x = 0, int y = 0) : x(x), y(y) {}
    
    bool operator==(const Coordinate& other) const {
        return x == other.x && y == other.y;
    }
    
    bool operator<(const Coordinate& other) const {
        if (x != other.x) return x < other.x;
        return y < other.y;
    }
    
    string toString() const {
        return "(" + to_string(x) + ", " + to_string(y) + ")";
    }
};

// Hash function for Coordinate to use in unordered_map
namespace std {
    template<>
    struct hash<Coordinate> {
        size_t operator()(const Coordinate& c) const {
            return hash<int>()(c.x) ^ (hash<int>()(c.y) << 1);
        }
    };
}

struct DishInfo {
    string name;
    int price;
};

// Parse coordinate from string like "(14, 27)"
Coordinate parseCoordinate(const string& str) {
    size_t start = str.find('(');
    size_t comma = str.find(',', start);
    size_t end = str.find(')', comma);
    
    if (start == string::npos || comma == string::npos || end == string::npos) {
        return Coordinate(-1, -1);
    }
    
    int x = stoi(str.substr(start + 1, comma - start - 1));
    int y = stoi(str.substr(comma + 1, end - comma - 1));
    
    return Coordinate(x, y);
}

// Parse dish name and price from string like "Falafel con Hummus(284)"
DishInfo parseDishPrice(const string& str) {
    size_t pos = str.rfind('(');
    if (pos == string::npos) {
        return {"", 0};
    }
    
    string name = str.substr(0, pos);
    string priceStr = str.substr(pos + 1, str.length() - pos - 2);
    int price = stoi(priceStr);
    
    return {name, price};
}

// Trim whitespace from string
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

// ============================================================================
// MENU GRAPH - Graph for restaurant-dish relationships
// ============================================================================

class MenuGraph {
private:
    map<string, vector<DishInfo>> restaurantMenus;  // restaurant -> dishes
    map<string, vector<pair<string, int>>> dishRestaurants;  // dish -> [(restaurant, price)]

public:
    void addMenuItem(const string& restaurant, const string& dish, int price) {
        restaurantMenus[restaurant].push_back({dish, price});
        dishRestaurants[dish].push_back({restaurant, price});
    }
    
    vector<DishInfo> getRestaurantMenu(const string& restaurant) const {
        auto it = restaurantMenus.find(restaurant);
        if (it != restaurantMenus.end()) {
            return it->second;
        }
        return vector<DishInfo>();
    }
    
    vector<pair<string, int>> getRestaurantsWithDish(const string& dish) const {
        auto it = dishRestaurants.find(dish);
        if (it != dishRestaurants.end()) {
            return it->second;
        }
        return vector<pair<string, int>>();
    }
    
    bool hasRestaurant(const string& restaurant) const {
        return restaurantMenus.find(restaurant) != restaurantMenus.end();
    }
    
    bool hasDish(const string& dish) const {
        return dishRestaurants.find(dish) != dishRestaurants.end();
    }
    
    int getRestaurantCount() const {
        return restaurantMenus.size();
    }
    
    int getDishCount() const {
        return dishRestaurants.size();
    }
};

// Load menu data from file
MenuGraph loadMenuData(const string& filename) {
    MenuGraph menuGraph;
    ifstream file(filename);
    
    if (!file.is_open()) {
        cerr << "Error: Could not open " << filename << endl;
        return menuGraph;
    }
    
    string line;
    // First line contains metadata
    getline(file, line);
    
    // Parse remaining lines
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        size_t rPos = line.find("R:");
        size_t oPos = line.find("O:");
        
        if (rPos == string::npos || oPos == string::npos) continue;
        
        string restaurant = trim(line.substr(rPos + 2, oPos - rPos - 2));
        string dishStr = trim(line.substr(oPos + 2));
        
        DishInfo dish = parseDishPrice(dishStr);
        if (!dish.name.empty()) {
            menuGraph.addMenuItem(restaurant, dish.name, dish.price);
        }
    }
    
    file.close();
    return menuGraph;
}

// ============================================================================
// CITY GRAPH - Weighted graph for delivery routing
// ============================================================================

// Load restaurant locations from file
map<string, Coordinate> loadRestaurantLocations(const string& filename) {
    map<string, Coordinate> locations;
    ifstream file(filename);
    
    if (!file.is_open()) {
        cerr << "Error: Could not open " << filename << endl;
        return locations;
    }
    
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        size_t lastParen = line.rfind('(');
        if (lastParen == string::npos) continue;
        
        string restaurant = trim(line.substr(0, lastParen));
        Coordinate coord = parseCoordinate(line.substr(lastParen));
        
        locations[restaurant] = coord;
    }
    
    file.close();
    return locations;
}

// Load city graph from file
Graph<Coordinate> loadCityGraph(const string& filename) {
    Graph<Coordinate> cityGraph(false); // undirected
    ifstream file(filename);
    
    if (!file.is_open()) {
        cerr << "Error: Could not open " << filename << endl;
        return cityGraph;
    }
    
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        size_t firstClose = line.find(')');
        size_t secondOpen = line.find('(', firstClose);
        size_t secondClose = line.find(')', secondOpen);
        
        if (firstClose == string::npos || secondOpen == string::npos || 
            secondClose == string::npos) continue;
        
        Coordinate coord1 = parseCoordinate(line.substr(0, firstClose + 1));
        Coordinate coord2 = parseCoordinate(line.substr(secondOpen, secondClose - secondOpen + 1));
        
        string weightStr = trim(line.substr(secondClose + 1));
        int weight = stoi(weightStr);
        
        cityGraph.addEdge(coord1, coord2, weight);
    }
    
    file.close();
    return cityGraph;
}

// ============================================================================
// DIJKSTRA'S ALGORITHM - Shortest path calculation
// ============================================================================

struct PathResult {
    bool found;
    int distance;
    vector<Coordinate> path;
    
    PathResult() : found(false), distance(-1) {}
};

PathResult dijkstra(const Graph<Coordinate>& graph, const Coordinate& start, const Coordinate& end) {
    PathResult result;
    
    if (!graph.hasNode(start) || !graph.hasNode(end)) {
        return result;
    }
    
    // Priority queue: (distance, coordinate)
    priority_queue<pair<int, Coordinate>, vector<pair<int, Coordinate>>, greater<pair<int, Coordinate>>> pq;
    
    unordered_map<Coordinate, int> distances;
    unordered_map<Coordinate, Coordinate> predecessors;
    set<Coordinate> visited;
    
    // Initialize distances
    distances[start] = 0;
    pq.push({0, start});
    
    while (!pq.empty()) {
        auto [currentDist, current] = pq.top();
        pq.pop();
        
        if (visited.count(current)) continue;
        visited.insert(current);
        
        if (current == end) {
            break;
        }
        
        for (const auto& [neighbor, weight] : graph.getNeighbors(current)) {
            if (visited.count(neighbor)) continue;
            
            int newDist = currentDist + weight;
            
            if (distances.find(neighbor) == distances.end() || newDist < distances[neighbor]) {
                distances[neighbor] = newDist;
                predecessors[neighbor] = current;
                pq.push({newDist, neighbor});
            }
        }
    }
    
    // Check if path was found
    if (distances.find(end) == distances.end()) {
        return result;
    }
    
    // Reconstruct path
    vector<Coordinate> path;
    Coordinate current = end;
    while (current == start || predecessors.find(current) != predecessors.end()) {
        path.push_back(current);
        if (current == start) break;
        current = predecessors[current];
    }
    
    reverse(path.begin(), path.end());
    
    result.found = true;
    result.distance = distances[end];
    result.path = path;
    
    return result;
}

// ============================================================================
// ORDER PROCESSING
// ============================================================================

struct Order {
    string date;
    string time;
    string restaurant;
    string dish;
    int price;
    Coordinate destination;
};

// Parse order from line
Order parseOrder(const string& line) {
    Order order;
    
    istringstream iss(line);
    string month, day, time;
    iss >> month >> day >> time;
    
    order.date = month + " " + day;
    order.time = time;
    
    size_t rPos = line.find("R:");
    size_t oPos = line.find("O:");
    size_t lastParen = line.rfind('(');
    
    if (rPos != string::npos && oPos != string::npos) {
        order.restaurant = trim(line.substr(rPos + 2, oPos - rPos - 3));
        
        // Find the coordinate part
        size_t coordStart = lastParen;
        size_t dishEnd = coordStart;
        
        // Find the dish and price part (before the coordinate)
        string dishPart = line.substr(oPos + 2, dishEnd - oPos - 2);
        dishPart = trim(dishPart);
        
        DishInfo dishInfo = parseDishPrice(dishPart);
        order.dish = dishInfo.name;
        order.price = dishInfo.price;
        
        order.destination = parseCoordinate(line.substr(coordStart));
    }
    
    return order;
}

vector<Order> loadOrders(const string& filename) {
    vector<Order> orders;
    ifstream file(filename);
    
    if (!file.is_open()) {
        cerr << "Error: Could not open " << filename << endl;
        return orders;
    }
    
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        orders.push_back(parseOrder(line));
    }
    
    file.close();
    return orders;
}

// ============================================================================
// MAIN FUNCTION - Testing and demonstrations
// ============================================================================

int main() {
    cout << BOLD << "========================================" << RESET << endl;
    cout << BOLD << "Restaurant Delivery System with Graphs" << RESET << endl;
    cout << BOLD << "========================================" << RESET << endl << endl;
    
    // Load all data
    cout << "Loading data files..." << endl;
    MenuGraph menuGraph = loadMenuData("menus.txt");
    map<string, Coordinate> restaurantLocations = loadRestaurantLocations("restaPlaces.txt");
    Graph<Coordinate> cityGraph = loadCityGraph("city30x30.txt");
    vector<Order> orders = loadOrders("orders-city30x30.txt");
    
    cout << "✓ Loaded " << menuGraph.getRestaurantCount() << " restaurants" << endl;
    cout << "✓ Loaded " << menuGraph.getDishCount() << " unique dishes" << endl;
    cout << "✓ Loaded " << restaurantLocations.size() << " restaurant locations" << endl;
    cout << "✓ Loaded city graph" << endl;
    cout << "✓ Loaded " << orders.size() << " orders" << endl << endl;
    
    // ========================================================================
    // PART 1: MENU SEARCH DEMONSTRATIONS
    // ========================================================================
    
    cout << BOLD << "PART 1: MENU SEARCH DEMONSTRATIONS" << RESET << endl;
    cout << "====================================" << endl << endl;
    
    // Demo 1: Query restaurant menu
    string testRestaurant = "La Terraza del Mar";
    cout << "Demo 1: Querying menu for '" << testRestaurant << "'" << endl;
    if (menuGraph.hasRestaurant(testRestaurant)) {
        vector<DishInfo> menu = menuGraph.getRestaurantMenu(testRestaurant);
        cout << "Found " << menu.size() << " dishes:" << endl;
        int count = 0;
        for (const auto& dish : menu) {
            cout << "  - " << dish.name << " ($" << dish.price << ")" << endl;
            if (++count >= 5) {
                cout << "  ... and " << (menu.size() - 5) << " more dishes" << endl;
                break;
            }
        }
    }
    cout << endl;
    
    // Demo 2: Query dish availability
    string testDish = "Falafel con Hummus";
    cout << "Demo 2: Querying restaurants offering '" << testDish << "'" << endl;
    if (menuGraph.hasDish(testDish)) {
        vector<pair<string, int>> restaurants = menuGraph.getRestaurantsWithDish(testDish);
        cout << "Found in " << restaurants.size() << " restaurant(s):" << endl;
        for (const auto& [restaurant, price] : restaurants) {
            cout << "  - " << restaurant << " ($" << price << ")" << endl;
        }
    }
    cout << endl;
    
    // ========================================================================
    // PART 2: DELIVERY ROUTE CALCULATIONS AND TESTING
    // ========================================================================
    
    cout << BOLD << "PART 2: DELIVERY ROUTE CALCULATIONS" << RESET << endl;
    cout << "=====================================" << endl << endl;
    
    int passedTests = 0;
    int totalTests = orders.size();
    
    for (size_t i = 0; i < orders.size(); i++) {
        const Order& order = orders[i];
        
        cout << BOLD << "Test " << (i + 1) << ": " << RESET;
        cout << "Order from '" << order.restaurant << "' to " << order.destination.toString() << endl;
        cout << "  Date/Time: " << order.date << " " << order.time << endl;
        cout << "  Dish: " << order.dish << " ($" << order.price << ")" << endl;
        
        // Find restaurant location
        auto it = restaurantLocations.find(order.restaurant);
        if (it == restaurantLocations.end()) {
            cout << "  " << RED << "✗ FAIL" << RESET << " - Restaurant location not found" << endl << endl;
            continue;
        }
        
        Coordinate restaurantCoord = it->second;
        cout << "  Restaurant location: " << restaurantCoord.toString() << endl;
        
        // Calculate shortest path
        PathResult result = dijkstra(cityGraph, restaurantCoord, order.destination);
        
        if (!result.found) {
            cout << "  " << RED << "✗ FAIL" << RESET << " - No path found" << endl << endl;
            continue;
        }
        
        cout << "  Number of edges: " << (result.path.size() - 1) << endl;
        cout << "  Total distance: " << result.distance << endl;
        cout << "  Path: ";
        
        for (size_t j = 0; j < result.path.size(); j++) {
            cout << result.path[j].toString();
            if (j < result.path.size() - 1) cout << " -> ";
            
            // Limit path display
            if (j >= 3 && result.path.size() > 6) {
                cout << " ... -> " << result.path[result.path.size() - 1].toString();
                break;
            }
        }
        cout << endl;
        
        // Validation
        bool passed = result.found && 
                     result.distance > 0 && 
                     !result.path.empty() &&
                     result.path[0] == restaurantCoord &&
                     result.path[result.path.size() - 1] == order.destination;
        
        if (passed) {
            cout << "  " << GREEN << "✓ PASS" << RESET << " - Path calculated successfully" << endl;
            passedTests++;
        } else {
            cout << "  " << RED << "✗ FAIL" << RESET << " - Path validation failed" << endl;
        }
        
        cout << endl;
    }
    
    // ========================================================================
    // TEST SUMMARY
    // ========================================================================
    
    cout << BOLD << "========================================" << RESET << endl;
    cout << BOLD << "TEST SUMMARY" << RESET << endl;
    cout << BOLD << "========================================" << RESET << endl;
    
    cout << "Tests passed: " << passedTests << "/" << totalTests << endl;
    
    if (passedTests == totalTests) {
        cout << GREEN << "✓ ALL TESTS PASSED" << RESET << endl;
    } else {
        cout << RED << "✗ SOME TESTS FAILED" << RESET << endl;
    }
    
    cout << endl;
    
    return 0;
}
