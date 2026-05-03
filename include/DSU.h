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
    // getClusterSize(ip): Get the size of the cluster containing this IP
    // ========================================================================
    int getClusterSize(const string& ip) {
        if (parent.find(ip) == parent.end()) {
            return 0;
        }
        
        string root = find(ip);
        int count = 0;
        
        for (auto& pair : parent) {
            // Re-finding to ensure path compression and correct root comparison
            if (find(pair.first) == root) {
                count++;
            }
        }
        
        return count;
    }
};