# 🚚 Graph-Based Logistics Optimizer

A C++ implementation of an optimized fuel-constrained delivery route solver using graph algorithms.

## 📋 Overview

This project addresses the problem of optimizing delivery routes in a city represented as a weighted undirected graph. The vehicle must visit delivery hubs before delivering to their corresponding houses, all while managing limited fuel capacity and refueling constraints.

### 🎯 Problem Statement

- **Graph Structure**: A weighted undirected graph representing city infrastructure with nodes and edges
- **Delivery Pairs**: Each package starts at a hub node and must be delivered to a corresponding house node
- **Fuel Management**: 
  - Vehicle starts with a full tank (capacity: `f`)
  - Can only refuel at designated fuel stations
  - Fuel cost equals edge weight
- **Objective**: Minimize total fuel cost while satisfying all constraints

### 🔒 Key Constraints

✓ A house can only be delivered to after its hub is visited  
✓ Fuel never drops below zero  
✓ Refueling only occurs at designated fuel stations  
✓ Tank capacity is fixed and cannot be exceeded  
✓ Find the optimal starting hub for minimum cost  

## 📊 Input Format

```
n t m k f
h[0] h[1] ... h[n-1]        // n hub nodes
d[0] d[1] ... d[n-1]        // n destination (house) nodes
fs[0] fs[1] ... fs[k-1]     // k fuel station nodes
m lines of: a b c           // bidirectional edge between node a and b with fuel cost c
```

### Parameters:
- `n`: Number of delivery pairs (hubs and houses)
- `t`: Total number of nodes in the graph
- `m`: Number of edges
- `k`: Number of fuel stations
- `f`: Fuel tank capacity

## 📤 Output Format

```
<length of the path>
<space-separated sequence of node indices>
```

## 🚀 Compilation and Execution

### Step 1: Update Input/Output Paths
Edit the file paths in `main()`:

```cpp
string inp = "tc/input_.txt";      // Change to your input file
string otp = "output/output_.txt"; // Change to your output file
```

### Step 2: Compile and Run
```bash
g++ -o optimizer main.cpp validator.cpp
./optimizer
```

## 🧠 Algorithm Design

### Core Algorithms

| Algorithm | Purpose |
|-----------|---------|
| **Floyd-Warshall** | Compute all-pairs shortest paths; reconstruct paths via parent tree |
| **Greedy Heuristic** | Select nearest unvisited hub or deliverable house with fuel feasibility check |
| **Simulation** | Validate fuel feasibility between nodes |

### Algorithm Flow

1. **Initialization**: Parse input and build graph
2. **Shortest Paths**: Compute all-pairs shortest paths using Floyd-Warshall
3. **Fuel Station Mapping**: Precompute nearest fuel station for each node
4. **Route Selection**: For each node, greedily choose:
   - Nearest unvisited hub, OR
   - Nearest deliverable house (hub already visited)
   - **Constraint**: Sufficient fuel to reach destination + nearest fuel station
5. **Hub Selection**: Try all hubs as starting points; return minimum cost route

## 📁 Project Structure

```
Graph-Based-Logistics-Optimizer/
├── main.cpp              # Main solver implementation
├── validator.cpp         # Solution validator
├── readme.md            # This file
├── tc/                  # Test cases directory
│   └── input_*.txt      # Input test files
└── output/              # Output directory
    └── output_*.txt     # Generated solution files
```

## 🔧 Core Functions

### Data Initialization
- **`reset()`** - Initialize all data structures and matrices

### Input/Output
- **`input()`** - Parse and load graph from input file
- **`generate_path(int u, int v)`** - Reconstruct path between nodes using parent tree

### Graph Preprocessing
- **`compute_floyd()`** - Build all-pairs shortest path matrices
- **`compute_closest_fuel_station()`** - Precompute nearest fuel station for each node

### Route Feasibility
- **`support_check(int u, int v, int fuel)`** - Check if direct path is fuel-feasible
- **`check_path(int u, int v, int fuel)`** - Verify reachability including fuel station access

### Solver
- **`solve(int start)`** - Execute greedy algorithm starting from hub `start`

## 💡 Strategy

The algorithm uses a **greedy nearest-neighbor approach with fuel constraints**:

1. **Current Position**: Track vehicle location and remaining fuel
2. **Next Node Selection**:
   - Identify all reachable nodes (unvisited hubs or deliverable houses)
   - Select the closest reachable node
   - **If no nodes reachable**: Navigate to nearest fuel station and refuel
3. **Fuel Management**:
   - Before each move, verify: `fuel >= distance + buffer_to_station`
   - Refuel to full capacity at stations
4. **Hub Iteration**: Repeat process starting from each hub; return best solution

## ✅ Constraints Handled

- ✓ **Dependency Management**: Houses only marked delivered after hub visitation
- ✓ **Fuel Safety**: Fuel never negative; refueling validated at stations only
- ✓ **Tank Capacity**: Respects maximum fuel capacity
- ✓ **Optimal Start**: Tests all hubs; selects starting point for minimum cost
- ✓ **Route Completeness**: Visits all hubs and corresponding houses

## 📈 Output

The program generates:
- **Path Length**: Total number of nodes in the delivery route
- **Route Sequence**: Complete node index sequence from start to end

### Example Output
```
15
0 5 12 8 3 7 14 2 9 1 6 11 4 13 10
```

## 📝 Notes

- Graph is **undirected** with weighted edges (fuel costs)
- All node indices are **0-indexed**
- Solution validity can be verified using `validator.cpp`
- Large graphs may benefit from optimized data structures

## 🤝 Contributing

Feel free to extend this project with:
- Advanced heuristics (A*, genetic algorithms)
- Dynamic programming solutions
- Visualization tools
- Performance optimization

---

**Language**: C++  
**Algorithm Complexity**: O(n³) for Floyd-Warshall + O(n²) for greedy selection  
**Space Complexity**: O(n²) for distance and parent matrices
