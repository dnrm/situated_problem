// * Daniel Medina | A01286980
// * Carlos Daniel Cruz García | A01286479
// * Friday 5th of December 2025

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <set>
#include <queue>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <limits>

using namespace std;

// ANSI color codes for terminal output
const string GREEN = "\033[32m";
const string RED = "\033[31m";
const string YELLOW = "\033[33m";
const string BLUE = "\033[34m";
const string CYAN = "\033[36m";
const string RESET = "\033[0m";
const string BOLD = "\033[1m";

// ============================================================================
// COORDINATE STRUCTURE
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
    
    // Calculate Manhattan distance (straight-line, not path distance)
    int manhattanDistance(const Coordinate& other) const {
        return abs(x - other.x) + abs(y - other.y);
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

// ============================================================================
// DISH INFORMATION STRUCTURE
// ============================================================================

struct DishInfo {
    string name;
    int price;
    
    DishInfo(const string& n = "", int p = 0) : name(n), price(p) {}
};

// ============================================================================
// GRAPH CLASS - For city navigation with weighted edges
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
    
    int getNodeCount() const {
        return adjList.size();
    }
    
    int getEdgeCount() const {
        int count = 0;
        for (const auto& pair : adjList) {
            count += pair.second.size();
        }
        return isDirected ? count : count / 2;
    }
};

// ============================================================================
// DIJKSTRA'S ALGORITHM - Shortest path in weighted graph
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
    
    // Initialize
    distances[start] = 0;
    pq.push({0, start});
    
    while (!pq.empty()) {
        auto [currentDist, current] = pq.top();
        pq.pop();
        
        if (visited.count(current)) continue;
        visited.insert(current);
        
        if (current == end) break;
        
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
    
    // Check if path found
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
// HELPER FUNCTIONS
// ============================================================================

// Parse coordinate from string such as "(14, 27)"
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
        return DishInfo("", 0);
    }
    
    string name = str.substr(0, pos);
    string priceStr = str.substr(pos + 1, str.length() - pos - 2);
    int price = stoi(priceStr);
    
    return DishInfo(name, price);
}

// Trim whitespace from string
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

// ============================================================================
// HASH TABLE-BASED MENU SYSTEM
// ============================================================================

class MenuSystem {
private:
    // HASH TABLE 1: Restaurant -> List of dishes
    unordered_map<string, vector<DishInfo>> restaurantMenus;
    
    // HASH TABLE 2: Dish -> List of (restaurant, price) pairs
    unordered_map<string, vector<pair<string, int>>> dishToRestaurants;
    
    // HASH TABLE 3: Restaurant -> Location
    unordered_map<string, Coordinate> restaurantLocations;

public:
    // Add a menu item to the hash tables
    void addMenuItem(const string& restaurant, const string& dish, int price) {
        restaurantMenus[restaurant].push_back(DishInfo(dish, price));
        dishToRestaurants[dish].push_back({restaurant, price});
    }
    
    // Add restaurant location to hash table
    void addRestaurantLocation(const string& restaurant, const Coordinate& location) {
        restaurantLocations[restaurant] = location;
    }
    
    // Query 1: Get menu of a specific restaurant (O(1) average case with hash table)
    vector<DishInfo> getRestaurantMenu(const string& restaurant) const {
        auto it = restaurantMenus.find(restaurant);
        if (it != restaurantMenus.end()) {
            return it->second;
        }
        return vector<DishInfo>();
    }
    
    // Query 2: Get restaurants offering a dish, sorted by price (O(1) lookup + O(n log n) sort)
    vector<pair<string, int>> getRestaurantsWithDish(const string& dish) const {
        auto it = dishToRestaurants.find(dish);
        if (it != dishToRestaurants.end()) {
            vector<pair<string, int>> result = it->second;
            // Sort by price in ascending order
            sort(result.begin(), result.end(), 
                 [](const pair<string, int>& a, const pair<string, int>& b) {
                     return a.second < b.second;
                 });
            return result;
        }
        return vector<pair<string, int>>();
    }
    
    // Query 3: Find N nearest restaurants using graph-based pathfinding
    vector<tuple<string, Coordinate, int>> getNearestRestaurants(
        const Coordinate& userLocation, 
        const Graph<Coordinate>& cityGraph,
        int n = 3
    ) const {
        vector<tuple<string, Coordinate, int>> distances; // (restaurant, location, distance)
        
        // Calculate shortest path distance to each restaurant using Dijkstra
        for (const auto& [restaurant, location] : restaurantLocations) {
            PathResult result = dijkstra(cityGraph, userLocation, location);
            if (result.found) {
                distances.push_back({restaurant, location, result.distance});
            }
        }
        
        // Sort by distance (shortest paths first)
        sort(distances.begin(), distances.end(),
             [](const auto& a, const auto& b) {
                 return get<2>(a) < get<2>(b);
             });
        
        // Return top N
        if (distances.size() > static_cast<size_t>(n)) {
            distances.resize(n);
        }
        
        return distances;
    }
    
    // Get restaurant location (O(1) with hash table)
    Coordinate getRestaurantLocation(const string& restaurant) const {
        auto it = restaurantLocations.find(restaurant);
        if (it != restaurantLocations.end()) {
            return it->second;
        }
        return Coordinate(-1, -1);
    }
    
    // Check if restaurant exists
    bool hasRestaurant(const string& restaurant) const {
        return restaurantMenus.find(restaurant) != restaurantMenus.end();
    }
    
    // Check if dish exists
    bool hasDish(const string& dish) const {
        return dishToRestaurants.find(dish) != dishToRestaurants.end();
    }
    
    // Get statistics
    int getRestaurantCount() const { return restaurantMenus.size(); }
    int getDishCount() const { return dishToRestaurants.size(); }
    int getLocationCount() const { return restaurantLocations.size(); }
};

// ============================================================================
// DATA LOADING FUNCTIONS
// ============================================================================

// Load menu data from file into hash tables
void loadMenuData(const string& filename, MenuSystem& menuSystem) {
    ifstream file(filename);
    
    if (!file.is_open()) {
        cerr << RED << "Error: Could not open " << filename << RESET << endl;
        return;
    }
    
    string line;
    // First line contains metadata
    getline(file, line);
    
    int itemCount = 0;
    
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
            menuSystem.addMenuItem(restaurant, dish.name, dish.price);
            itemCount++;
        }
    }
    
    file.close();
    cout << GREEN << "✓ Loaded " << itemCount << " menu items into hash tables" << RESET << endl;
}

// Load restaurant locations into hash table
void loadRestaurantLocations(const string& filename, MenuSystem& menuSystem) {
    ifstream file(filename);
    
    if (!file.is_open()) {
        cerr << RED << "Error: Could not open " << filename << RESET << endl;
        return;
    }
    
    string line;
    int locationCount = 0;
    
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        size_t lastParen = line.rfind('(');
        if (lastParen == string::npos) continue;
        
        string restaurant = trim(line.substr(0, lastParen));
        Coordinate coord = parseCoordinate(line.substr(lastParen));
        
        if (coord.x >= 0 && coord.y >= 0) {
            menuSystem.addRestaurantLocation(restaurant, coord);
            locationCount++;
        }
    }
    
    file.close();
    cout << GREEN << "✓ Loaded " << locationCount << " restaurant locations into hash table" << RESET << endl;
}

// Load city graph from file
Graph<Coordinate> loadCityGraph(const string& filename) {
    Graph<Coordinate> cityGraph(false); // undirected graph
    ifstream file(filename);
    
    if (!file.is_open()) {
        cerr << RED << "Error: Could not open " << filename << RESET << endl;
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
    cout << GREEN << "✓ Loaded city graph: " << cityGraph.getNodeCount() 
         << " nodes, " << cityGraph.getEdgeCount() << " edges" << RESET << endl;
    
    return cityGraph;
}

// ============================================================================
// USER INTERFACE FUNCTIONS
// ============================================================================

void displayRestaurantMenu(const string& restaurant, const MenuSystem& menuSystem) {
    cout << "\n" << BOLD << CYAN << "+===========================================================+" << RESET << endl;
    cout << BOLD << CYAN << "|  MENU: " << restaurant;
    // Pad to align
    int padding = 52 - restaurant.length();
    for (int i = 0; i < padding; i++) cout << " ";
    cout << "|" << RESET << endl;
    cout << BOLD << CYAN << "+===========================================================+" << RESET << endl;
    
    vector<DishInfo> menu = menuSystem.getRestaurantMenu(restaurant);
    if (menu.empty()) {
        cout << RED << "No menu items found for this restaurant." << RESET << endl;
        return;
    }
    
    cout << "\n" << BOLD << "Dishes Available:" << RESET << endl;
    cout << string(60, '-') << endl;
    
    for (size_t i = 0; i < menu.size(); i++) {
        cout << setw(3) << (i + 1) << ". " 
             << left << setw(45) << menu[i].name 
             << right << YELLOW << "$" << setw(4) << menu[i].price << RESET << endl;
    }
    
    cout << string(60, '-') << endl;
    cout << BOLD << "Total dishes: " << menu.size() << RESET << endl;
}

void searchDish(const MenuSystem& menuSystem) {
    cout << "\n" << BOLD << BLUE << "+===========================================================+" << RESET << endl;
    cout << BOLD << BLUE << "|              SEARCH FOR DISH                              |" << RESET << endl;
    cout << BOLD << BLUE << "+===========================================================+" << RESET << endl;
    
    cout << "\nEnter dish name: ";
    
    string dish;
    cin.ignore();
    getline(cin, dish);
    
    if (!menuSystem.hasDish(dish)) {
        cout << RED << "\n✗ Dish '" << dish << "' not found in any restaurant." << RESET << endl;
        return;
    }
    
    vector<pair<string, int>> restaurants = menuSystem.getRestaurantsWithDish(dish);
    
    cout << "\n" << GREEN << "✓ Found '" << dish << "' in " << restaurants.size() 
         << " restaurant(s) (sorted by price - ascending):" << RESET << endl;
    cout << string(60, '=') << endl;
    
    for (size_t i = 0; i < restaurants.size(); i++) {
        cout << setw(3) << (i + 1) << ". " 
             << left << setw(45) << restaurants[i].first 
             << right << YELLOW << "$" << setw(4) << restaurants[i].second << RESET << endl;
    }
    cout << string(60, '=') << endl;
}

void searchRestaurantMenu(const MenuSystem& menuSystem) {
    cout << "\n" << BOLD << BLUE << "+===========================================================+" << RESET << endl;
    cout << BOLD << BLUE << "|           SEARCH RESTAURANT MENU                          |" << RESET << endl;
    cout << BOLD << BLUE << "+===========================================================+" << RESET << endl;
    
    cout << "\nEnter restaurant name: ";
    
    string restaurant;
    cin.ignore();
    getline(cin, restaurant);
    
    if (!menuSystem.hasRestaurant(restaurant)) {
        cout << RED << "\n✗ Restaurant '" << restaurant << "' not found." << RESET << endl;
        return;
    }
    
    displayRestaurantMenu(restaurant, menuSystem);
}

void findClosestRestaurants(const MenuSystem& menuSystem, const Graph<Coordinate>& cityGraph) {
    cout << "\n" << BOLD << BLUE << "+===========================================================+" << RESET << endl;
    cout << BOLD << BLUE << "|     FIND 3 CLOSEST RESTAURANTS (USING GRAPH PATHFINDING)  |" << RESET << endl;
    cout << BOLD << BLUE << "+===========================================================+" << RESET << endl;
    
    cout << "\nEnter your location coordinates:" << endl;
    cout << "X coordinate (0-29): ";
    int x;
    cin >> x;
    
    cout << "Y coordinate (0-29): ";
    int y;
    cin >> y;
    
    if (cin.fail() || x < 0 || x > 29 || y < 0 || y > 29) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << RED << "\n✗ Invalid coordinates. Please enter values between 0 and 29." << RESET << endl;
        return;
    }
    
    Coordinate userLocation(x, y);
    
    // Verify user location exists in graph
    if (!cityGraph.hasNode(userLocation)) {
        cout << RED << "\n✗ Your location " << userLocation.toString() 
             << " is not in the city graph. Try another location." << RESET << endl;
        return;
    }
    
    cout << "\n" << YELLOW << "🔍 Calculating shortest paths using Dijkstra's algorithm..." << RESET << endl;
    cout << YELLOW << "   Starting from: " << userLocation.toString() << RESET << endl;
    
    vector<tuple<string, Coordinate, int>> nearest = menuSystem.getNearestRestaurants(userLocation, cityGraph, 3);
    
    if (nearest.empty()) {
        cout << RED << "✗ No reachable restaurants found from your location." << RESET << endl;
        return;
    }
    
    cout << "\n" << GREEN << BOLD << "✓ 3 CLOSEST RESTAURANTS (by shortest path):" << RESET << endl;
    cout << string(70, '=') << endl;
    
    for (size_t i = 0; i < nearest.size(); i++) {
        cout << "\n" << BOLD << CYAN << (i + 1) << ". " << get<0>(nearest[i]) << RESET << endl;
        cout << "   📍 Location: " << get<1>(nearest[i]).toString() << endl;
        cout << "   🛣️  Shortest Path Distance: " << YELLOW << get<2>(nearest[i]) 
             << " units" << RESET << " (via Dijkstra)" << endl;
    }
    cout << "\n" << string(70, '=') << endl;
    
    // Ask if user wants to see menus
    cout << "\nWould you like to see the menus of these restaurants? (y/n): ";
    char choice;
    cin >> choice;
    
    if (choice == 'y' || choice == 'Y') {
        for (size_t i = 0; i < nearest.size(); i++) {
            cout << "\n" << string(70, '=') << endl;
            displayRestaurantMenu(get<0>(nearest[i]), menuSystem);
        }
    }
}

void displayMainMenu() {
    cout << "\n" << BOLD << CYAN << "+============================================================+" << RESET << endl;
    cout << BOLD << CYAN << "|     RESTAURANT DELIVERY SYSTEM - MAIN MENU                 |" << RESET << endl;
    cout << BOLD << CYAN << "|     (Hash Tables + Graph Algorithms)                       |" << RESET << endl;
    cout << BOLD << CYAN << "+============================================================+" << RESET << endl;
    
    cout << "\n" << BOLD << "Available Options:" << RESET << endl;
    cout << "  " << GREEN << "1." << RESET << " Search restaurant menu " << CYAN << "(Hash Table)" << RESET << endl;
    cout << "  " << GREEN << "2." << RESET << " Search dish across restaurants " << CYAN << "(Hash Table)" << RESET << endl;
    cout << "  " << GREEN << "3." << RESET << " Find 3 closest restaurants " << CYAN << "(Graph + Dijkstra)" << RESET << endl;
    cout << "  " << RED << "4." << RESET << " Exit" << endl;
    cout << "\n" << BOLD << "Enter your choice (1-4): " << RESET;
}

void displayStatistics(const MenuSystem& menuSystem, const Graph<Coordinate>& cityGraph) {
    cout << "\n" << BOLD << GREEN << "+============================================================+" << RESET << endl;
    cout << BOLD << GREEN << "|              SYSTEM STATISTICS                             |" << RESET << endl;
    cout << BOLD << GREEN << "+============================================================+" << RESET << endl;
    
    cout << "\n📊 " << BOLD << "Hash Table Statistics:" << RESET << endl;
    cout << "   • Total Restaurants: " << CYAN << menuSystem.getRestaurantCount() << RESET << endl;
    cout << "   • Total Unique Dishes: " << CYAN << menuSystem.getDishCount() << RESET << endl;
    cout << "   • Restaurant Locations: " << CYAN << menuSystem.getLocationCount() << RESET << endl;
    
    cout << "\n🗺️  " << BOLD << "Graph Statistics:" << RESET << endl;
    cout << "   • Graph Nodes (intersections): " << CYAN << cityGraph.getNodeCount() << RESET << endl;
    cout << "   • Graph Edges (streets): " << CYAN << cityGraph.getEdgeCount() << RESET << endl;
    
    cout << "\n⚡ " << BOLD << "Algorithm Complexity:" << RESET << endl;
    cout << "   • Restaurant Menu Lookup: " << YELLOW << "O(1) average" << RESET << " [Hash Table]" << endl;
    cout << "   • Dish Search: " << YELLOW << "O(1) lookup + O(k log k) sort" << RESET << " [Hash Table]" << endl;
    cout << "   • Shortest Path (Dijkstra): " << YELLOW << "O((V + E) log V)" << RESET << " [Graph]" << endl;
    cout << "   • Find 3 Nearest: " << YELLOW << "O(R × (V + E) log V)" << RESET << " [Graph × R restaurants]" << endl;
    cout << endl;
}

// ============================================================================
// MAIN FUNCTION - Interactive Menu System
// ============================================================================

int main() {
    cout << BOLD << CYAN << "+============================================================+" << RESET << endl;
    cout << BOLD << CYAN << "|   RESTAURANT DELIVERY SYSTEM                               |" << RESET << endl;
    cout << BOLD << CYAN << "|   Hash Tables + Graph Algorithms                           |" << RESET << endl;
    cout << BOLD << CYAN << "|   By: Daniel Medina | A01286980                            |" << RESET << endl;
    cout << BOLD << CYAN << "|   By: Carlos Daniel Cruz García | A01286479                            |" << RESET << endl;
    cout << BOLD << CYAN << "+============================================================+" << RESET << endl << endl;
    
    // Initialize menu system with hash tables
    MenuSystem menuSystem;
    
    // Load all data
    cout << BOLD << "📂 Loading data structures..." << RESET << endl;
    cout << string(60, '-') << endl;
    
    loadMenuData("menus.txt", menuSystem);
    loadRestaurantLocations("restaPlaces.txt", menuSystem);
    Graph<Coordinate> cityGraph = loadCityGraph("city30x30.txt");
    
    cout << string(60, '-') << endl;
    
    // Display statistics
    displayStatistics(menuSystem, cityGraph);
    
    // Main interactive loop
    int choice = 0;
    
    while (choice != 4) {
        displayMainMenu();
        cin >> choice;
        
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << RED << "\n✗ Invalid input. Please enter a number between 1 and 4." << RESET << endl;
            continue;
        }
        
        switch (choice) {
            case 1:
                searchRestaurantMenu(menuSystem);
                break;
                
            case 2:
                searchDish(menuSystem);
                break;
                
            case 3:
                findClosestRestaurants(menuSystem, cityGraph);
                break;
                
            case 4:
                cout << "\n" << GREEN << BOLD << "+============================================================+" << RESET << endl;
                cout << GREEN << BOLD << "|   Thank you for using the Restaurant Delivery System!     |" << RESET << endl;
                cout << GREEN << BOLD << "+============================================================+" << RESET << endl;
                cout << "\n" << CYAN << "Goodbye!" << RESET << "\n" << endl;
                break;
                
            default:
                cout << RED << "\n✗ Invalid choice. Please enter a number between 1 and 4." << RESET << endl;
                break;
        }
    }
    
    return 0;
}
