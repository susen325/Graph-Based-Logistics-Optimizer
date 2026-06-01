Delivery Route Optimization
Overview
This project addresses the problem of optimizing a fuel-constrained delivery route in a city represented as a weighted undirected graph. Each delivery consists of a package that starts at a hub and must be delivered to a corresponding house. A single vehicle must deliver all packages while obeying the following constraints:

The vehicle can only deliver to a house after visiting its corresponding hub.
The vehicle starts with a full fuel tank and can refuel only at designated fuel stations.
The tank has a fixed maximum capacity f.
The total fuel cost (i.e., the weight of the traveled path) should be minimized.
Input Format
n t m k f
h[0] h[1] ... h[n-1]        // n hub nodes
d[0] d[1] ... d[n-1]        // n destination (house) nodes
fs[0] fs[1] ... fs[k-1]     // k fuel station nodes
m lines of: a b c           // bidirectional edge between node a and b with fuel cost c
Output Format
<length of the path>
<space-separated sequence of node indices>
Compilation and Execution
Ensure the input file is located in the correct directory and the following lines in main() are updated accordingly:

string inp = "tc/input_.txt";
string otp = "output/output_.txt";
Run the main.cpp function after changing the string names.

Key Components
Algorithms Used
Floyd-Warshall Algorithm: Computes all-pairs shortest paths and reconstructs the path using a tree matrix.
Greedy Heuristic: For each hub, the algorithm attempts to greedily deliver all packages while managing fuel.
Simulation: Validates the feasibility of traveling from one node to another with current fuel constraints.
Core Functions
reset(): Initializes all data structures and matrices.
input(): Reads and parses the input.
compute_floyd(): Builds shortest path matrices using the Floyd-Warshall algorithm.
compute_closest_fuel_station(): For every node, computes the nearest fuel stations.
generate_path(): Reconstructs the path between two nodes from the parent tree.
support_check(): Checks if a path between two nodes is fuel-feasible.
check_path(): Ensures the vehicle can reach a destination and still make it to a fuel station.
solve(int start): Solves the problem for a specific starting hub and tracks the minimum cost.
Idea
For a current node, the function choses the closest node which is either an unvisited hub or a house whose hub has already been visited given that there is enough fuel to do so. If there is not enough fuel, the vehicle moves to the closest fuel station.

Constraints Handled
Ensures a house is marked delivered only after its hub is visited.
Fuel never drops below 0; refuels only at specified stations.
Picks optimal hub to start the delivery for minimal cost.
Output
Prints the number of nodes in the final delivery route.
Outputs the full route in the correct sequence.
