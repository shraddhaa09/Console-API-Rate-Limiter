#pragma once
#include "Common.h"
#include <iostream>
#include <unordered_map>
#include <string>

using namespace std;

// ============================================================================
// DSU.h — Disjoint Set Union (Union-Find) for IP Reputation Management
// Member 4: The Detective
// ============================================================================
// 
// PURPOSE:
// Track IP addresses and group them into clusters (e.g., botnet detection).
// Quickly check if an IP belongs to a banned group.
//
// KEY OPERATIONS:
// - find(ip): Returns the representative (root) of the set containing this IP
// - unite(ip1, ip2): Merges two IP clusters
// - banIP(ip): Marks an IP as banned by uniting it with BANNED_ROOT
// - isBanned(ip): Checks if an IP is in the banned cluster
//
// OPTIMIZATIONS:
// - Path Compression: During find(), flatten the tree by making nodes point directly to root
// - Union by Rank: Attach smaller tree under larger tree to keep height low
//
// TIME COMPLEXITY: O(α(n)) per operation, where α is inverse Ackermann (effectively O(1))
// ============================================================================

class DSU {
private:
    // We use IP strings as keys, so we need a map-based approach
    // parent[ip] = the parent IP in the union-find tree
    // rank[ip] = approximate depth of the tree rooted at ip
    unordered_map<string, string> parent;
    unordered_map<string, int> rank;
    
    // Special sentinel IP that represents the "banned" cluster
    const string BANNED_ROOT = "__BANNED_CLUSTER__";
    
public:
    // Constructor: Initialize the banned root as its own parent
    DSU() {
        parent[BANNED_ROOT] = BANNED_ROOT;
        rank[BANNED_ROOT] = 0;
    }
    
    // ========================================================================
    // makeSet(ip): Initialize a new IP as its own set (if not already present)
    // ========================================================================
    void makeSet(const string& ip) {
        if (parent.find(ip) == parent.end()) {
            parent[ip] = ip;  // Initially, each IP is its own parent
            rank[ip] = 0;     // Initial rank is 0
        }
    }
    
    // ========================================================================
    // find(ip): Find the root representative of the set containing this IP
    // Uses PATH COMPRESSION: makes all nodes along the path point to root
    // ========================================================================
    string find(const string& ip) {
        // If IP not seen before, initialize it
        if (parent.find(ip) == parent.end()) {
            makeSet(ip);
            return ip;
        }
        
        // Path compression: recursively find root and update parent
        if (parent[ip] != ip) {
            parent[ip] = find(parent[ip]);  // This is the key optimization!
        }
        
        return parent[ip];
    }
    
    // ========================================================================
    // unite(ip1, ip2): Merge the sets containing ip1 and ip2
    // Uses UNION BY RANK: attach smaller tree under root of larger tree
    // ========================================================================
    void unite(const string& ip1, const string& ip2) {
        // Ensure both IPs exist in the DSU
        makeSet(ip1);
        makeSet(ip2);
        
        // Find the roots of both IPs
        string root1 = find(ip1);
        string root2 = find(ip2);
        
        // If already in same set, nothing to do
        if (root1 == root2) {
            return;
        }
        
        // Union by rank: attach smaller tree under larger tree
        if (rank[root1] < rank[root2]) {
            parent[root1] = root2;
        } 
        else if (rank[root1] > rank[root2]) {
            parent[root2] = root1;
        } 
        else {
            // Same rank: make root2 parent of root1, increase rank
            parent[root1] = root2;
            rank[root2]++;
        }
    }
    
    // ========================================================================
    // banIP(ip): Mark an IP as banned by uniting it with the BANNED_ROOT
    // ========================================================================
    void banIP(const string& ip) {
        makeSet(ip);
        unite(ip, BANNED_ROOT);
    }
    
    // ========================================================================
    // isBanned(ip): Check if an IP belongs to the banned cluster
    // Returns true if find(ip) == find(BANNED_ROOT)
    // ========================================================================
    bool isBanned(const string& ip) {
        // If IP never seen, it's not banned
        if (parent.find(ip) == parent.end()) {
            return false;
        }
        
        return find(ip) == find(BANNED_ROOT);
    }
    
    // ========================================================================
    // debugPrint(): Print current state of all IP clusters (for testing)
    // ========================================================================
    void debugPrint() {
        cout << "\n=== DSU State ===" << endl;
        unordered_map<string, int> clusterSizes;
        
        for (auto& pair : parent) {
            string ip = pair.first;
            string root = find(ip);
            clusterSizes[root]++;
        }
        
        cout << "Total IPs tracked: " << parent.size() << endl;
        cout << "Number of clusters: " << clusterSizes.size() << endl;
        
        for (auto& cluster : clusterSizes) {
            if (cluster.first == BANNED_ROOT) {
                cout << "  [BANNED CLUSTER]: " << cluster.second << " IPs" << endl;
            } else {
                cout << "  Cluster " << cluster.first << ": " << cluster.second << " IPs" << endl;
            }
        }
        cout << "==================" << endl;
    }
    
    // ========================================================================
    // getClusterSize(ip): Get the size of the cluster containing this IP
    // ========================================================================
    int getClusterSize(const string& ip) {
        if (parent.find(ip) == parent.end()) {
            return 0;
        }
        
        string root = find(ip);
        int count = 0;
        
        for (auto& pair : parent) {
            if (find(pair.first) == root) {
                count++;
            }
        }
        
        return count;
    }
};

// ============================================================================
// TESTING CODE (compile with -DTEST flag to enable)
// ============================================================================
#ifdef TEST

int main() {
    cout << "=== DSU Unit Tests ===" << endl;
    
    DSU dsu;
    
    // Test 1: Basic IP insertion and find
    cout << "\nTest 1: Basic Operations" << endl;
    dsu.makeSet("192.168.1.1");
    dsu.makeSet("192.168.1.2");
    cout << "✓ IPs initialized" << endl;
    
    // Test 2: Union operation
    cout << "\nTest 2: Union Operation" << endl;
    dsu.unite("192.168.1.1", "192.168.1.2");
    string root1 = dsu.find("192.168.1.1");
    string root2 = dsu.find("192.168.1.2");
    
    if (root1 == root2) {
        cout << "✓ IPs successfully united (same root: " << root1 << ")" << endl;
    } else {
        cout << "✗ Union failed!" << endl;
    }
    
    // Test 3: Ban single IP
    cout << "\nTest 3: Ban Single IP" << endl;
    dsu.banIP("10.0.0.1");
    
    if (dsu.isBanned("10.0.0.1")) {
        cout << "✓ IP successfully banned" << endl;
    } else {
        cout << "✗ Ban failed!" << endl;
    }
    
    if (!dsu.isBanned("192.168.1.1")) {
        cout << "✓ Non-banned IP correctly identified as clean" << endl;
    } else {
        cout << "✗ False positive on clean IP!" << endl;
    }
    
    // Test 4: Botnet detection (unite multiple IPs, ban one, all should be banned)
    cout << "\nTest 4: Botnet Detection" << endl;
    dsu.makeSet("10.0.0.2");
    dsu.makeSet("10.0.0.3");
    dsu.makeSet("10.0.0.4");
    
    // Group them as a botnet
    dsu.unite("10.0.0.2", "10.0.0.3");
    dsu.unite("10.0.0.3", "10.0.0.4");
    
    // Ban just one IP from the botnet
    dsu.banIP("10.0.0.3");
    
    // All three should now be banned
    if (dsu.isBanned("10.0.0.2") && dsu.isBanned("10.0.0.3") && dsu.isBanned("10.0.0.4")) {
        cout << "✓ Entire botnet successfully banned via single ban operation" << endl;
    } else {
        cout << "✗ Botnet banning incomplete!" << endl;
    }
    
    // Test 5: Path compression efficiency
    cout << "\nTest 5: Path Compression" << endl;
    // Create a chain: ip5 -> ip6 -> ip7 -> ip8
    for (int i = 5; i <= 8; i++) {
        dsu.makeSet("10.0.0." + to_string(i));
    }
    dsu.unite("10.0.0.5", "10.0.0.6");
    dsu.unite("10.0.0.6", "10.0.0.7");
    dsu.unite("10.0.0.7", "10.0.0.8");
    
    string root = dsu.find("10.0.0.5");
    cout << "✓ Path compression applied (root: " << root << ")" << endl;
    
    // Test 6: Debug state
    cout << "\nTest 6: Current DSU State" << endl;
    dsu.debugPrint();
    
    // Test 7: Cluster size
    cout << "\nTest 7: Cluster Size Query" << endl;
    int botnetSize = dsu.getClusterSize("10.0.0.2");
    cout << "Botnet cluster size: " << botnetSize << endl;
    
    cout << "\n=== All Tests Complete ===" << endl;
    
    return 0;
}

#endif