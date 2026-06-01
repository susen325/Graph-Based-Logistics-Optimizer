#include<bits/stdc++.h>
#include<ext/pb_ds/assoc_container.hpp>
#include<ext/pb_ds/tree_policy.hpp>

#pragma GCC optimize("Ofast,unroll-loops")

#define int long long
#define all(v) v.begin(), v.end()
#define rall(v) v.rbegin(), v.rend()

using namespace std;
using namespace __gnu_pbds;

template<class T> using ordered_set = tree<T, null_type, less<T>, rb_tree_tag, tree_order_statistics_node_update>; // find_by_order, order_of_key

int n, t, m, k, f; // input variables
vector<vector<int>> v; // graph matrix
vector<int> h, d, fs;

vector<int> hash_h, hash_d, hash_fs; // hashing hubs, houses, fuel stations
vector<vector<int>> floyd_minm; // floyd warshall matrix
vector<vector<int>> floyd_minm_tree; // floyd warshall tree (pi), parent array basically - to compute path towards the root(ith row of matrix)
vector<vector<int>> closest_fuel_station; // it is what its name says, closest fuel stations to ith hub
map<int, int> mp; // maps house to hub

vector<int> temp_path; // temp path to store the path from start to end
vector<int> path; // path for a particular starting hub
vector<int> final_path; // final path for a test case
int temp_cost; 
int final_cost;

// resets global variables
void reset(){
    v.clear();
    h.clear();
    d.clear();
    fs.clear();
    v.resize(t + 1, vector<int>(t + 1, 1e18));
    h.resize(n + 1);
    d.resize(n + 1);
    fs.resize(k + 1);

    hash_h.clear();
    hash_h.resize(t + 1, 0);
    hash_d.clear();
    hash_d.resize(t + 1, 0);
    hash_fs.clear();
    hash_fs.resize(t + 1, 0);

    floyd_minm.clear();
    floyd_minm.resize(t + 1, vector<int>(t + 1, 1e18));

    floyd_minm_tree.clear();
    floyd_minm_tree.resize(t + 1, vector<int>(t + 1, -1));

    closest_fuel_station.clear();
    closest_fuel_station.resize(t + 1, vector<int>(k, 1e18));

    mp.clear();

    final_path.clear();
    final_cost = 1e18;
}

// input function to take input
void input(){
    cin >> n >> t >> m >> k >> f;

    reset();

    for(int i = 0; i < n; i++){
        cin >> h[i];
        hash_h[h[i]] = 1;
    }
    for(int i = 0; i < n; i++){
        cin >> d[i];
        hash_d[d[i]] = 1;
        mp[d[i]] = h[i];
    }

    for(int i = 0; i < k; i++){
        cin >> fs[i];
        hash_fs[fs[i]] = 1;
    }

    for(int i = 0; i < m; i++){
        int a, b, c;
        cin >> a >> b >> c;
        v[a][b] = c;
        v[b][a] = c;
    }
    for(int i = 0; i <= t; i++){
        v[i][i] = 0;
    }
}

// computes floyd warshall matrix and pi matrix(minm tree)
void compute_floyd(){
    vector<vector<vector<int>>> temp(t + 1, vector<vector<int>>(t + 1, vector<int>(t + 1, 1e18)));
    vector<vector<vector<int>>> tree(t + 1, vector<vector<int>>(t + 1, vector<int>(t + 1)));

    for(int i = 0; i <= t; i++){
        for(int j = 0; j <= t; j++){
            temp[i][j][0] = v[i][j];
            if(v[i][j] != 1e18){
                tree[i][j][0] = i;
            }
            else{
                tree[i][j][0] = -1;
            }
        }
    }

    for(int k = 1; k <= t; k++){
        for(int i = 0; i <= t; i++){
            for(int j = 0; j <= t; j++){
                temp[i][j][k] = temp[i][j][k - 1];
                tree[i][j][k] = tree[i][j][k - 1];
                
                if(temp[i][j][k] > temp[i][k][k - 1] + temp[k][j][k - 1]){
                    tree[i][j][k] = tree[k][j][k - 1];
                    temp[i][j][k] = temp[i][k][k - 1] + temp[k][j][k - 1];
                }
            }
        }
    }

    for(int i = 0; i <= t; i++){
        for(int j = 0; j <= t; j++){
            floyd_minm[i][j] = temp[i][j][t];
            floyd_minm_tree[i][j] = tree[i][j][t];
        }
    }
}

// computes closest fuel stations to each hub
void compute_closest_fuel_station(){
    for(int i = 0; i <= t; i++){
        vector<pair<int, int>> temp;
        for(int j = 0; j <= t; j++){
            if(hash_fs[j]){
                temp.push_back({floyd_minm[i][j], j});
            }
        }
        sort(all(temp));
        for(int j = 0; j < k; j++){
            closest_fuel_station[i][j] = temp[j].second;
        }
    }
}

// generates a path from en to st (reversed because in floyd warshall, we retrieve path from a child node towards the root)
void generate_path(int st, int en){
    temp_path.clear();
    int curr = st;
    while(curr != en){
        temp_path.push_back(curr);
        curr = floyd_minm_tree[en][curr];
    }
    reverse(all(temp_path));
}

// used inside check_path, checks if a valid path exists from st to en with the given fuel and returns the fuel left at the end of path
int support_check(int st, int en, int fuel){
    generate_path(en, st);
    vector<int> temp;
    temp.push_back(st);
    for(auto i : temp_path){
        temp.push_back(i);
    }

    for(int i = 1; i < temp.size(); i++){
        if(hash_fs[temp[i - 1]]){
            fuel = f;
        }
        fuel -= v[temp[i - 1]][temp[i]];
        if(fuel < 0){
            return -1;
        }
    }

    if(hash_fs[temp.back()]){
        fuel = f;
    }
    return fuel;
}

// checks if a valid path exists from st to en with the given fuel
int check_path(int st, int en, int fuel){
    int x = support_check(st, en, fuel);
    if(x == -1){
        return 0;
    }

    x = support_check(en, closest_fuel_station[en][0], x);
    if(x == -1){
        return 0;
    }

    return 1;
}

// solves the problem for a particular starting hub
void solve(int start){
    path.clear();
    temp_cost = 0;

    set<int> hubs_left;
    for(int i = 0; i < n; i++){
        if(i == start) continue;
        hubs_left.insert(h[i]);
    }
    int curr = h[start];
    path.push_back(curr);

    int fuel = f;
    set<int> houses_left;
    for(int i = 0; i < n; i++){
        assert(d[i] != h[i]);
        houses_left.insert(d[i]);
    }  

    while(houses_left.size()){
        int closest = -1;

        for(auto i : hubs_left){
            if(closest == -1){
                int x = check_path(curr, i, fuel);
                if(x == 0){
                    continue;
                }
                closest = i;
            }
            else{
                int x = check_path(curr, i, fuel);
                if(x == 0){
                    continue;
                }
                if(floyd_minm[curr][i] < floyd_minm[curr][closest]){
                    closest = i;
                }
            }
        }

        for(auto i : houses_left){
            if(hubs_left.find(mp[i]) != hubs_left.end()){
                continue;
            }

            if(closest == -1){
                int x = check_path(curr, i, fuel);
                if(x == 0){
                    continue;
                }
                closest = i;
            }
            else{
                int x = check_path(curr, i, fuel);
                if(x == 0){
                    continue;
                }
                if(floyd_minm[curr][i] < floyd_minm[curr][closest]){
                    closest = i;
                }
            }
        }

        if(closest == -1){
            int x = check_path(curr, closest_fuel_station[curr][0], fuel);
            if(x == 0){
                temp_cost = 1e18;
                return;
            }

            closest = closest_fuel_station[curr][0];
        }

        generate_path(closest, curr);

        for(auto i : temp_path){
            path.push_back(i);
            fuel -= v[curr][i];
            assert(fuel >= 0);
            temp_cost += v[curr][i];
            if(hash_h[i]){
                hubs_left.erase(i);
            }
            if(hash_fs[i]){
                fuel = f;
            }
            if(hash_d[i]){
                if(hubs_left.find(mp[i]) == hubs_left.end()){
                    houses_left.erase(i);
                }
            }
            curr = i;
        }

        hubs_left.erase(closest);
        houses_left.erase(closest);
    }
}

signed main(){

    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    //change test case number here, make sure to enter the correct path
    string inp = "tc/input_2.txt";
    string otp = "output/output_2.txt";
    freopen(inp.c_str(), "r", stdin);
    freopen(otp.c_str(), "w", stdout);
    input();

    compute_floyd();
    compute_closest_fuel_station();
    
    for(int i = 0; i < n; i++){
        solve(i);
        if(temp_cost < final_cost){
            final_cost = temp_cost;
            final_path = path;
        }
    }

    // cout << final_cost << endl;
    cout << final_path.size() << endl;
    for(auto i : final_path){
        cout << i << " ";
    }
    cout << endl;

    return 0;
}
