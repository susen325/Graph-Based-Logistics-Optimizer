#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include <limits>

using namespace std;

// Use explicit types instead of macros
using ll = long long;
const ll INF = numeric_limits<ll>::max();

class RouteValidator {
private:
    int num_hubs, total_nodes, num_edges, num_stations;
    ll max_fuel;

    vector<vector<ll>> adj_matrix;
    unordered_set<int> is_hub;
    unordered_set<int> is_destination;
    unordered_set<int> is_fuel_station;
    unordered_map<int, int> dest_to_hub; 

public:
    // Returns true if the graph loaded successfully
    bool load_graph(const string& filepath) {
        ifstream fin(filepath);
        if (!fin.is_open()) {
            cerr << "Error: Could not open input file " << filepath << "\n";
            return false;
        }

        fin >> num_hubs >> total_nodes >> num_edges >> num_stations >> max_fuel;

        // Initialize adjacency matrix with infinity
        adj_matrix.assign(total_nodes + 1, vector<ll>(total_nodes + 1, INF));
        for (int i = 0; i <= total_nodes; i++) {
            adj_matrix[i][i] = 0;
        }

        // Read Hubs
        for (int i = 0; i < num_hubs; i++) {
            int hub;
            fin >> hub;
            is_hub.insert(hub);
        }

        // Read Destinations (Houses)
        // Assuming the order of hubs and destinations matches input mapping
        vector<int> temp_hubs(is_hub.begin(), is_hub.end()); 
        for (int i = 0; i < num_hubs; i++) {
            int dest;
            fin >> dest;
            is_destination.insert(dest);
            // Assuming the i-th destination maps to the i-th hub read
            dest_to_hub[dest] = temp_hubs[i]; 
        }

        // Read Fuel Stations
        for (int i = 0; i < num_stations; i++) {
            int station;
            fin >> station;
            is_fuel_station.insert(station);
        }

        // Read Edges
        for (int i = 0; i < num_edges; i++) {
            int u, v;
            ll cost;
            fin >> u >> v >> cost;
            adj_matrix[u][v] = cost;
            adj_matrix[v][u] = cost;
        }

        return true;
    }

    void validate_route(const string& filepath) {
        ifstream fin(filepath);
        if (!fin.is_open()) {
            cerr << "Error: Could not open output file " << filepath << "\n";
            return;
        }

        int path_length;
        fin >> path_length; // Skipping the first variable 'tempp' from your original code
        fin >> path_length;

        if (path_length <= 0) return;

        vector<int> path(path_length);
        for (int i = 0; i < path_length; i++) {
            fin >> path[i];
        }

        int current_node = path[0];
        ll current_fuel = max_fuel;
        ll total_cost = 0;

        unordered_set<int> visited_hubs;
        unordered_set<int> pending_deliveries = is_destination;

        for (int i = 1; i < path_length; i++) {
            int next_node = path[i];

            // 1. Check if edge exists
            if (adj_matrix[current_node][next_node] == INF) {
                cout << "INVALID: No edge between " << current_node << " and " << next_node << "\n";
                return;
            }

            ll travel_cost = adj_matrix[current_node][next_node];

            // 2. Check fuel constraints
            if (travel_cost > current_fuel) {
                cout << "INVALID: Not enough fuel to travel from " << current_node << " to " << next_node << "\n";
                return;
            }

            // Travel to next node
            total_cost += travel_cost;
            current_fuel -= travel_cost;
            current_node = next_node;

            // 3. Process node actions
            if (is_fuel_station.count(current_node)) {
                current_fuel = max_fuel; // Refuel
            }

            if (is_hub.count(current_node)) {
                visited_hubs.insert(current_node); // Pick up package
            }

            if (is_destination.count(current_node)) {
                int required_hub = dest_to_hub[current_node];
                if (visited_hubs.count(required_hub)) {
                    pending_deliveries.erase(current_node); // Successfully delivered
                }
            }
        }

        // 4. Final Validation Check
        cout << "\n--- Route Validation Result ---\n";
        if (!pending_deliveries.empty()) {
            cout << "STATUS: FAILED (Not all houses were delivered to)\n";
        } else {
            cout << "STATUS: OK\n";
            cout << "Total Fuel Cost: " << total_cost << "\n";
        }
    }
};

int main() {
    // Fast I/O
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    for (int i = 0; i < 3; i++) {
        string input_file = "tc/input_" + to_string(i) + ".txt";
        string output_file = "output/output_" + to_string(i) + ".txt";

        cout << "Testing Case " << i << "...\n";

        RouteValidator validator;
        if (validator.load_graph(input_file)) {
            validator.validate_route(output_file);
        }
        cout << "------------------------------\n";
    }

    return 0;
}
