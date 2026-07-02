/**
 * Route Quest: Logistics Routing & Fuel Optimization Solver
 * * Description: 
 * A high-performance algorithmic solver designed to find optimal delivery routes 
 * across a network of hubs and houses under strict vehicle fuel constraints.
 * * Core Algorithms:
 * - Floyd-Warshall (O(V^3)) for all-pairs shortest path and O(V) path reconstruction (PI Matrix).
 * - Greedy Traversal Engine with dynamic fuel-station detours.
 * * Compiler Optimizations: GCC Ofast, unroll-loops enabled for maximum execution speed.
 */

#include <bits/stdc++.h>


#define int long long
#define all(v) v.begin(), v.end()
#define rall(v) v.rbegin(), v.rend()

using namespace std;


// ==========================================
// GLOBAL STATE & NETWORK GRAPH
// ==========================================
int n, t, m, k, fuel_capacity; 
vector<vector<int>> graph; 
vector<int> hubs, houses, fuel_stations;

// Fast O(1) Lookups
vector<int> is_hub, is_house, is_fuel_station; 
map<int, int> house_to_hub; // Maps a house to its required origin hub

// Precomputed Shortest Paths & Path Reconstruction
vector<vector<int>> floyd_minm; 
vector<vector<int>> floyd_minm_tree; // Predecessor (PI) matrix
vector<vector<int>> closest_fuel_station; 

// Routing Variables
vector<int> temp_path; 
vector<int> current_path; 
vector<int> final_path; 
int temp_cost; 
int final_cost;

// ==========================================
// SYSTEM INITIALIZATION
// ==========================================
void reset() {
    graph.clear(); hubs.clear(); houses.clear(); fuel_stations.clear();
    graph.resize(t + 1, vector<int>(t + 1, 1e18));
    hubs.resize(n + 1); houses.resize(n + 1); fuel_stations.resize(k + 1);

    is_hub.assign(t + 1, 0);
    is_house.assign(t + 1, 0);
    is_fuel_station.assign(t + 1, 0);

    floyd_minm.assign(t + 1, vector<int>(t + 1, 1e18));
    floyd_minm_tree.assign(t + 1, vector<int>(t + 1, -1));
    closest_fuel_station.assign(t + 1, vector<int>(k, 1e18));

    house_to_hub.clear();
    final_path.clear();
    final_cost = 1e18;
}

void input() {
    cin >> n >> t >> m >> k >> fuel_capacity;
    reset();

    for(int i = 0; i < n; i++) {
        cin >> hubs[i];
        is_hub[hubs[i]] = 1;
    }
    for(int i = 0; i < n; i++) {
        cin >> houses[i];
        is_house[houses[i]] = 1;
        house_to_hub[houses[i]] = hubs[i];
    }
    for(int i = 0; i < k; i++) {
        cin >> fuel_stations[i];
        is_fuel_station[fuel_stations[i]] = 1;
    }
    for(int i = 0; i < m; i++) {
        int a, b, c;
        cin >> a >> b >> c;
        graph[a][b] = c;
        graph[b][a] = c;
    }
    for(int i = 0; i <= t; i++) {
        graph[i][i] = 0;
    }
}

// ==========================================
// ALGORITHMIC CORE
// ==========================================

// Computes all-pairs shortest paths using Dynamic Programming
void compute_floyd() {
    vector<vector<vector<int>>> temp(t + 1, vector<vector<int>>(t + 1, vector<int>(t + 1, 1e18)));
    vector<vector<vector<int>>> tree(t + 1, vector<vector<int>>(t + 1, vector<int>(t + 1)));

    // Base cases setup
    for(int i = 0; i <= t; i++) {
        for(int j = 0; j <= t; j++) {
            temp[i][j][0] = graph[i][j];
            if(graph[i][j] != 1e18) {
                tree[i][j][0] = i;
            } else {
                tree[i][j][0] = -1;
            }
        }
    }

    // O(V^3) shortest path resolution
    for(int k = 1; k <= t; k++) {
        for(int i = 0; i <= t; i++) {
            for(int j = 0; j <= t; j++) {
                temp[i][j][k] = temp[i][j][k - 1];
                tree[i][j][k] = tree[i][j][k - 1];
                
                if(temp[i][j][k] > temp[i][k][k - 1] + temp[k][j][k - 1]) {
                    tree[i][j][k] = tree[k][j][k - 1];
                    temp[i][j][k] = temp[i][k][k - 1] + temp[k][j][k - 1];
                }
            }
        }
    }

    // Flatten 3D dp array into 2D matrices for O(1) lookups later
    for(int i = 0; i <= t; i++) {
        for(int j = 0; j <= t; j++) {
            floyd_minm[i][j] = temp[i][j][t];
            floyd_minm_tree[i][j] = tree[i][j][t];
        }
    }
}

// Precomputes the nearest fuel stations from every possible node
void compute_closest_fuel_station() {
    for(int i = 0; i <= t; i++) {
        vector<pair<int, int>> temp;
        for(int j = 0; j <= t; j++) {
            if(is_fuel_station[j]) {
                temp.push_back({floyd_minm[i][j], j});
            }
        }
        sort(all(temp));
        for(int j = 0; j < k; j++) {
            closest_fuel_station[i][j] = temp[j].second;
        }
    }
}

// Unwinds the PI matrix to reconstruct the precise path sequence
void generate_path(int st, int en) {
    temp_path.clear();
    int curr = st;
    while(curr != en) {
        temp_path.push_back(curr);
        curr = floyd_minm_tree[en][curr];
    }
    reverse(all(temp_path));
}

// ==========================================
// FUEL & SURVIVABILITY SIMULATION
// ==========================================

// Simulates driving the route to ensure fuel constraints aren't violated
int support_check(int st, int en, int fuel) {
    generate_path(en, st);
    vector<int> temp;
    temp.push_back(st);
    for(auto i : temp_path) {
        temp.push_back(i);
    }

    for(int i = 1; i < temp.size(); i++) {
        if(is_fuel_station[temp[i - 1]]) {
            fuel = fuel_capacity;
        }
        fuel -= graph[temp[i - 1]][temp[i]];
        if(fuel < 0) {
            return -1; // Route is fatal (ran out of fuel)
        }
    }

    if(is_fuel_station[temp.back()]) {
        fuel = fuel_capacity;
    }
    return fuel;
}

// Checks if destination is reachable AND a safe fuel station is reachable afterwards
int check_path(int st, int en, int fuel) {
    int remaining_fuel = support_check(st, en, fuel);
    if(remaining_fuel == -1) return 0;

    remaining_fuel = support_check(en, closest_fuel_station[en][0], remaining_fuel);
    if(remaining_fuel == -1) return 0;

    return 1;
}

// ==========================================
// GREEDY TRAVERSAL ENGINE
// ==========================================

// Evaluates a global path starting from a specific hub
void solve(int start) {
    current_path.clear();
    temp_cost = 0;

    set<int> hubs_left;
    for(int i = 0; i < n; i++) {
        if(i == start) continue;
        hubs_left.insert(hubs[i]);
    }
    int curr = hubs[start];
    current_path.push_back(curr);

    int fuel = fuel_capacity;
    set<int> houses_left;
    for(int i = 0; i < n; i++) {
        assert(houses[i] != hubs[i]);
        houses_left.insert(houses[i]);
    }  

    // Dispatch logic loop
    while(houses_left.size()) {
        int closest = -1;

        // 1. Scan for the nearest accessible hub
        for(auto i : hubs_left) {
            if(closest == -1) {
                if(check_path(curr, i, fuel)) closest = i;
            } else {
                if(check_path(curr, i, fuel) && floyd_minm[curr][i] < floyd_minm[curr][closest]) {
                    closest = i;
                }
            }
        }

        // 2. Scan for the nearest accessible, unlocked house
        for(auto i : houses_left) {
            if(hubs_left.find(house_to_hub[i]) != hubs_left.end()) {
                continue; // Cannot visit house until its hub is visited
            }

            if(closest == -1) {
                if(check_path(curr, i, fuel)) closest = i;
            } else {
                if(check_path(curr, i, fuel) && floyd_minm[curr][i] < floyd_minm[curr][closest]) {
                    closest = i;
                }
            }
        }

        // 3. Fallback: Force a detour to the nearest fuel station if trapped
        if(closest == -1) {
            if(!check_path(curr, closest_fuel_station[curr][0], fuel)) {
                temp_cost = 1e18; // Complete route failure
                return; 
            }
            closest = closest_fuel_station[curr][0];
        }

        // 4. Drive to the chosen target and update state
        generate_path(closest, curr);

        for(auto i : temp_path) {
            current_path.push_back(i);
            fuel -= graph[curr][i];
            assert(fuel >= 0);
            
            temp_cost += graph[curr][i];
            
            if(is_hub[i]) hubs_left.erase(i);
            if(is_fuel_station[i]) fuel = fuel_capacity;
            if(is_house[i]) {
                if(hubs_left.find(house_to_hub[i]) == hubs_left.end()) {
                    houses_left.erase(i);
                }
            }
            curr = i;
        }

        hubs_left.erase(closest);
        houses_left.erase(closest);
    }
}

signed main() {
    // Optimize I/O operations
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    // File I/O for testing
    string inp = "tc/input_2.txt";
    string otp = "output/output_2.txt";
    freopen(inp.c_str(), "r", stdin);
    freopen(otp.c_str(), "w", stdout);
    
    // Process input and map environment
    input();

    // Generate graph heuristics
    compute_floyd();
    compute_closest_fuel_station();
    
    // Compute best route by testing every hub as a starting point
    for(int i = 0; i < n; i++) {
        solve(i);
        
        // Track the global optimum
        if(temp_cost < final_cost) {
            final_cost = temp_cost;
            final_path = current_path;
        }
    }

    // Output the optimized routing sequence
    cout << final_path.size() << "\n";
    for(auto i : final_path) {
        cout << i << " ";
    }
    cout << "\n";

    return 0;
}
