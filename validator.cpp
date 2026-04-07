#include <bits/stdc++.h>

#pragma GCC optimize("Ofast,unroll-loops")

#define int long long
using namespace std;

int n, t, m, k, max_endurance;
vector<vector<int>> adj;
vector<int> c, s, sz; // Caches, Camps, Safe Zones

vector<int> is_cache, is_camp, is_safe_zone;
map<int, int> cache_for_camp;

void reset() {
    c.clear(); s.clear(); sz.clear();
    adj.assign(t + 1, vector<int>(t + 1, 1e18));
    c.resize(n + 1);
    s.resize(n + 1);
    sz.resize(k + 1);

    is_cache.assign(t + 1, 0);
    is_camp.assign(t + 1, 0);
    is_safe_zone.assign(t + 1, 0);
    cache_for_camp.clear();
}

void input() {
    cin >> n >> t >> m >> k >> max_endurance;
    reset();

    for (int i = 0; i < n; i++) {
        cin >> c[i];
        is_cache[c[i]] = 1;
    }
    for (int i = 0; i < n; i++) {
        cin >> s[i];
        is_camp[s[i]] = 1;
        cache_for_camp[s[i]] = c[i];
    }
    for (int i = 0; i < k; i++) {
        cin >> sz[i];
        is_safe_zone[sz[i]] = 1;
    }

    for (int i = 0; i < m; i++) {
        int u, v, weight;
        cin >> u >> v >> weight;
        adj[u][v] = weight;
        adj[v][u] = weight;
    }
    for (int i = 0; i <= t; i++) {
        adj[i][i] = 0;
    }
}

void validate_mission(int tc) {
    string op = "output/output_" + to_string(tc) + ".txt";
    freopen(op.c_str(), "r", stdin);

    int claimed_cost;
    cin >> claimed_cost;

    int path_length;
    cin >> path_length;

    vector<int> route(path_length);
    for (int i = 0; i < path_length; i++) {
        cin >> route[i];
    }

    int curr = route[0];
    int current_endurance = max_endurance;

    set<int> visited_caches, unreached_camps;
    for (int i = 0; i < n; i++) {
        unreached_camps.insert(s[i]);
    }

    int actual_cost = 0;
    for (int i = 1; i < path_length; i++) {
        int next_node = route[i];

        // Process Cache pickup
        if (is_cache[curr]) {
            visited_caches.insert(curr);
        }

        // Process Camp delivery (only if corresponding cache was visited)
        if (is_camp[curr]) {
            if (visited_caches.find(cache_for_camp[curr]) != visited_caches.end()) {
                unreached_camps.erase(curr);
            }
        }

        if (adj[curr][next_node] == 1e18) {
            cout << "Mission Failed: Impassable route taken.\n";
            return;
        }
        if (adj[curr][next_node] > current_endurance) {
            cout << "Mission Failed: Insufficient vehicle endurance.\n";
            return;
        }
        
        actual_cost += adj[curr][next_node];
        current_endurance -= adj[curr][next_node];
        curr = next_node;

        if (is_safe_zone[next_node]) {
            current_endurance = max_endurance; // Restock and Repair
        }
    }

    // Check the final node arrived at
    if (is_camp[curr]) {
        if (visited_caches.find(cache_for_camp[curr]) != visited_caches.end()) {
            unreached_camps.erase(curr);
        }
    }

    cout << "\nTest Case " << tc << " Evaluation:\n";
    if (!unreached_camps.empty()) {
        cout << "Mission Failed: All survivor camps were not reached.\n";
    } else {
        cout << "Mission Successful!\n";
        cout << "Total Operational Cost: " << actual_cost << "\n";
    }
}

signed main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    for (int i = 0; i < 3; i++) {
        string s_inp = "tc/input_" + to_string(i) + ".txt";
        freopen(s_inp.c_str(), "r", stdin);
        
        input();
        validate_mission(i);
    }

    return 0;
}
