# Restaurant Delivery System with Graphs

**Author:** Daniel Medina | A01286980  
**Date:** Friday 28th of November 2025

## Project Overview

This project implements a restaurant delivery system using graph data structures and algorithms in C++. The system provides two main functionalities:

1. **Menu Search System**: Query restaurant menus and find dishes across multiple restaurants using a bipartite graph structure.
2. **Delivery Route Optimization**: Calculate the shortest delivery paths through a city grid using Dijkstra's algorithm.

## Data Structures

### 1. Generic Graph Class

A template-based graph implementation using an **adjacency list** representation:

- **Node storage**: `unordered_map<T, vector<pair<T, int>>>`
- **Edge weights**: Integer weights stored with each neighbor
- **Graph types**: Supports both directed and undirected graphs
- **Operations**: 
  - `addNode(node)` - O(1) average
  - `addEdge(from, to, weight)` - O(1) average
  - `getNeighbors(node)` - O(1) average

### 2. Menu Bipartite Graph

A specialized graph structure for restaurant-dish relationships:

- **Restaurant nodes**: Map restaurant names to their menu items
- **Dish nodes**: Map dish names to restaurants that offer them
- **Edges**: Represent the availability of a dish at a restaurant, weighted by price

**Data structures used**:
- `map<string, vector<DishInfo>>` - Restaurant to dishes mapping
- `map<string, vector<pair<string, int>>>` - Dish to restaurants mapping

**Query Operations**:
- `getRestaurantMenu(restaurant)` - O(1) lookup + O(k) where k = number of dishes
- `getRestaurantsWithDish(dish)` - O(1) lookup + O(m) where m = number of restaurants

### 3. City Delivery Graph

An undirected weighted graph representing a 30×30 city grid:

- **Nodes**: 900 coordinate pairs (x, y) representing locations
- **Edges**: 1730 edges connecting adjacent nodes
- **Weights**: Distance between adjacent locations
- **Structure**: Graph<Coordinate> using the generic Graph template

## Algorithms

### Dijkstra's Shortest Path Algorithm

**Purpose**: Find the shortest delivery route from restaurant to customer

**Implementation Details**:
- **Priority Queue**: Min-heap for efficient node selection
- **Distance Tracking**: Unordered map for O(1) distance lookups
- **Path Reconstruction**: Predecessor map to rebuild the path

**Complexity Analysis**:
- **Time Complexity**: O((V + E) log V) where V = 900 nodes, E = 1730 edges
- **Space Complexity**: O(V) for distances, predecessors, and priority queue

**Algorithm Steps**:
1. Initialize all distances to infinity except source (0)
2. Use priority queue to always process nearest unvisited node
3. Update distances to neighbors if shorter path found
4. Track predecessors for path reconstruction
5. Reconstruct path by backtracking from destination to source

## File Descriptions

### Input Files

| File | Description | Format |
|------|-------------|--------|
| `menus.txt` | Restaurant menu data | `R:Restaurant O:Dish(Price)` |
| `city30x30.txt` | City grid graph | `(x1, y1) (x2, y2) weight` |
| `restaPlaces.txt` | Restaurant locations | `Restaurant (x, y)` |
| `orders-city30x30.txt` | Test delivery orders | `Date Time R:Restaurant O:Dish(Price) (x, y)` |

### Source Files

| File | Description | Lines |
|------|-------------|-------|
| `main.cpp` | Complete implementation | ~550 |
| `Makefile` | Build configuration | ~25 |
| `README.md` | Documentation (this file) | - |

## Compilation and Execution

### Prerequisites

- C++ compiler with C++17 support (g++ recommended)
- Make utility (optional, but recommended)

### Building the Project

Using Make:
```bash
make
```

Manual compilation:
```bash
g++ -std=c++17 -Wall -Wextra -O2 main.cpp -o main
```

### Running the Program

```bash
make run
```

Or directly:
```bash
./main
```

### Cleaning Build Artifacts

```bash
make clean
```

## Program Output

The program automatically runs demonstrations and tests:

### Part 1: Menu Search Demonstrations

- **Demo 1**: Query a specific restaurant's menu
  - Shows dishes available and their prices
  - Example: "La Terraza del Mar" menu

- **Demo 2**: Query restaurants offering a specific dish
  - Lists all restaurants serving the dish with prices
  - Example: Restaurants offering "Falafel con Hummus"

### Part 2: Delivery Route Calculations

Tests 5 delivery orders with the following output for each:

```
Test N: Order from 'Restaurant' to (x, y)
  Date/Time: Date Time
  Dish: Dish Name ($Price)
  Restaurant location: (x, y)
  Number of edges: N
  Total distance: N
  Path: (x1,y1) -> (x2,y2) -> ... -> (xn,yn)
  ✓ PASS - Path calculated successfully
```

**Test Validation**:
- ✅ Path exists from restaurant to destination
- ✅ Distance is positive and valid
- ✅ Path starts at restaurant location
- ✅ Path ends at customer destination
- ✅ All coordinates are valid nodes in the graph

Results are color-coded:
- **Green (✓ PASS)**: Test successful
- **Red (✗ FAIL)**: Test failed

## Complexity Analysis

### Menu Operations

| Operation | Time Complexity | Space Complexity |
|-----------|----------------|------------------|
| Load menu data | O(n) | O(n) |
| Get restaurant menu | O(1) + O(k) | O(1) |
| Get dish restaurants | O(1) + O(m) | O(1) |

Where:
- n = total menu items (8268)
- k = dishes per restaurant
- m = restaurants per dish

### City Graph Operations

| Operation | Time Complexity | Space Complexity |
|-----------|----------------|------------------|
| Load city graph | O(E) | O(V + E) |
| Dijkstra's algorithm | O((V + E) log V) | O(V) |

Where:
- V = vertices (900)
- E = edges (1730)

### Overall Performance

- **Menu loading**: ~8,268 operations
- **Graph loading**: ~1,730 edge insertions
- **Path calculation**: ~1,500-3,000 operations per query (worst case)
- **Total execution time**: < 1 second on modern hardware

## Key Features

1. **Generic Graph Template**: Reusable for different node types
2. **Efficient Data Structures**: Hash maps and adjacency lists for O(1) access
3. **Optimal Pathfinding**: Dijkstra's algorithm guarantees shortest path
4. **Robust Parsing**: Handles various input formats with error checking
5. **Visual Feedback**: Color-coded test results with ANSI escape codes
6. **Comprehensive Testing**: Automated validation of all functionality

## Implementation Highlights

### Memory Efficiency

- Adjacency list representation: O(V + E) space vs O(V²) for matrix
- Unordered maps for O(1) average-case lookups
- Efficient coordinate hashing for spatial data

### Code Organization

1. **Graph Class** (Lines ~20-75): Generic template implementation
2. **Helper Structures** (Lines ~80-150): Parsing and utility functions
3. **Menu Graph** (Lines ~155-230): Bipartite graph for menu queries
4. **City Graph** (Lines ~235-320): Spatial graph for routing
5. **Dijkstra's Algorithm** (Lines ~325-400): Shortest path implementation
6. **Order Processing** (Lines ~405-450): Test case handling
7. **Main Function** (Lines ~455-550): Orchestration and output

### Design Decisions

1. **Bipartite Graph for Menus**: Enables efficient bidirectional queries
2. **Template Graph Class**: Allows reuse for different node types
3. **Coordinate Struct**: Type-safe representation of spatial data
4. **Priority Queue**: Ensures optimal performance for Dijkstra's algorithm
5. **Single File Implementation**: Simplifies compilation and distribution

## Testing Strategy

The program includes 5 automated test cases covering:

1. **Different restaurants**: Various locations across the grid
2. **Different distances**: Short to long delivery routes
3. **Edge cases**: Corner locations, central locations
4. **Data validation**: Ensures all components work together
5. **Path correctness**: Validates start/end points and continuity

Each test verifies:
- Path existence
- Correct start and end nodes
- Valid distance calculation
- Path connectivity

## Future Enhancements

Possible improvements for extended functionality:

1. **Multiple Delivery Optimization**: Route planning for multiple orders
2. **Time-based Constraints**: Consider delivery time windows
3. **Dynamic Updates**: Handle real-time road closures or traffic
4. **Interactive Mode**: User input for custom queries
5. **Graph Visualization**: Export paths to visualization format
6. **A* Algorithm**: Even faster pathfinding with heuristics
7. **Capacity Constraints**: Model delivery vehicle limitations

## License

Academic project for TC1031 - Data Structures course.

## Contact

Daniel Medina | A01286980

