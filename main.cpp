#include <bits/stdc++.h>
using namespace std;

#define int long long
const int INF = 1e18;

int n, t, m, k, max_endurance;
vector<int> c, s, sz; // Caches, Camps, Safe Zones
vector<vector<int>> adj, dist, parent;
vector<int> is_safe_zone, cache_for_camp;
map<int, int> camp_for_cache;

int best_total_cost = INF;
vector<int> best_route;

void reset() {
    c.resize(n); s.resize(n); sz.resize(k);
    adj.assign(t + 1, vector<int>(t + 1, INF));
    dist.assign(t + 1, vector<int>(t + 1, INF));
    parent.assign(t + 1, vector<int>(t + 1, -1));
    is_safe_zone.assign(t + 1, 0);
    cache_for_camp.assign(t + 1, -1);
    camp_for_cache.clear();
}

void input() {
    cin >> n >> t >> m >> k >> max_endurance;
    reset();

    for (int i = 0; i < n; i++) cin >> c[i];
    for (int i = 0; i < n; i++) {
        cin >> s[i];
        camp_for_cache[c[i]] = s[i];
        cache_for_camp[s[i]] = c[i];
    }
    for (int i = 0; i < k; i++) {
        cin >> sz[i];
        is_safe_zone[sz[i]] = 1;
    }

    for (int i = 0; i < m; i++) {
        int u, v, weight;
        cin >> u >> v >> weight;
        adj[u][v] = min(adj[u][v], weight);
        adj[v][u] = min(adj[v][u], weight);
    }
    for (int i = 0; i <= t; i++) adj[i][i] = 0;
}

void compute_floyd() {
    for (int i = 0; i <= t; i++) {
        for (int j = 0; j <= t; j++) {
            dist[i][j] = adj[i][j];
            if (adj[i][j] != INF && i != j) parent[i][j] = i;
        }
    }
    for (int k_node = 0; k_node <= t; k_node++) {
        for (int i = 0; i <= t; i++) {
            for (int j = 0; j <= t; j++) {
                if (dist[i][k_node] < INF && dist[k_node][j] < INF) {
                    if (dist[i][j] > dist[i][k_node] + dist[k_node][j]) {
                        dist[i][j] = dist[i][k_node] + dist[k_node][j];
                        parent[i][j] = parent[k_node][j];
                    }
                }
            }
        }
    }
}

int compute_closest_safe_zone(int node) {
    int min_dist = INF;
    for (int zone : sz) {
        min_dist = min(min_dist, dist[node][zone]);
    }
    return min_dist;
}

// Reconstructs path from u to v using parent matrix
vector<int> generate_path(int u, int v) {
    vector<int> path;
    if (dist[u][v] == INF) return path;
    int curr = v;
    while (curr != u) {
        path.push_back(curr);
        curr = parent[u][curr];
    }
    reverse(path.begin(), path.end());
    return path;
}

// Checks if we can reach the target and still have enough endurance to reach ANY safe zone
bool check_path(int curr, int target, int current_endurance) {
    if (dist[curr][target] > current_endurance) return false;
    int safe_zone_dist = compute_closest_safe_zone(target);
    return (dist[curr][target] + safe_zone_dist <= current_endurance);
}

void solve_for_start(int start_cache) {
    vector<int> current_route;
    int current_cost = 0;
    int current_endurance = max_endurance;
    int curr = start_cache;

    current_route.push_back(curr);

    set<int> unvisited_caches(c.begin(), c.end());
    set<int> unreached_camps(s.begin(), s.end());
    set<int> camps_ready; 

    unvisited_caches.erase(curr);
    camps_ready.insert(camp_for_cache[curr]);

    while (!unreached_camps.empty()) {
        int next_target = -1;
        int min_dist_to_target = INF;

        // Try to find the closest valid cache or ready camp
        for (int candidate : unvisited_caches) {
            if (check_path(curr, candidate, current_endurance) && dist[curr][candidate] < min_dist_to_target) {
                min_dist_to_target = dist[curr][candidate];
                next_target = candidate;
            }
        }
        for (int candidate : camps_ready) {
            if (check_path(curr, candidate, current_endurance) && dist[curr][candidate] < min_dist_to_target) {
                min_dist_to_target = dist[curr][candidate];
                next_target = candidate;
            }
        }

        // If no target is safely reachable, we MUST go to a safe zone
        if (next_target == -1) {
            int best_safe_zone = -1;
            int min_sz_dist = INF;
            for (int zone : sz) {
                if (dist[curr][zone] <= current_endurance && dist[curr][zone] < min_sz_dist) {
                    min_sz_dist = dist[curr][zone];
                    best_safe_zone = zone;
                }
            }
            if (best_safe_zone == -1) return; // Mission Failed: Stranded
            next_target = best_safe_zone;
        }

        // Move vehicle to next_target
        current_cost += dist[curr][next_target];
        current_endurance -= dist[curr][next_target];
        
        vector<int> path_segment = generate_path(curr, next_target);
        current_route.insert(current_route.end(), path_segment.begin(), path_segment.end());
        
        curr = next_target;

        // Update states based on where we arrived
        if (is_safe_zone[curr]) {
            current_endurance = max_endurance; // Refuel / Repair
        }
        if (unvisited_caches.count(curr)) {
            unvisited_caches.erase(curr);
            camps_ready.insert(camp_for_cache[curr]); // Corresponding camp is now valid
        }
        if (camps_ready.count(curr)) {
            camps_ready.erase(curr);
            unreached_camps.erase(curr); // Delivery successful
        }
    }

    if (current_cost < best_total_cost) {
        best_total_cost = current_cost;
        best_route = current_route;
    }
}

signed main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    // Update index for different test cases
    string inp = "tc/input_0.txt";
    string otp = "output/output_0.txt";

    freopen(inp.c_str(), "r", stdin);
    freopen(otp.c_str(), "w", stdout);

    input();
    compute_floyd();

    // Try starting the mission from every possible supply cache
    for (int start_cache : c) {
        solve_for_start(start_cache);
    }

    // Output strictly matching the checker's expectations
    cout << best_total_cost << "\n";
    cout << best_route.size() << "\n";
    for (int node : best_route) {
        cout << node << " ";
    }
    cout << "\n";

    return 0;
}
