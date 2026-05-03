#pragma once
#include "Common.h"
#include <iostream>
#include <unordered_map>
#include <string>

using namespace std;

class DSU {
private:
    unordered_map<string, string> parent;
    unordered_map<string, int> rank;
    
    const string BANNED_ROOT = "__BANNED_CLUSTER__";
    
public:
    DSU() {
        parent[BANNED_ROOT] = BANNED_ROOT;
        rank[BANNED_ROOT] = 0;
    }
    
    void makeSet(const string& ip) {
        if (parent.find(ip) == parent.end()) {
            parent[ip] = ip;
            rank[ip] = 0;
        }
    }
    
    string find(const string& ip) {
        if (parent.find(ip) == parent.end()) {
            makeSet(ip);
            return ip;
        }
        
        if (parent[ip] != ip) {
            parent[ip] = find(parent[ip]);
        }
        
        return parent[ip];
    }
    
    void unite(const string& ip1, const string& ip2) {
        makeSet(ip1);
        makeSet(ip2);
        
        string root1 = find(ip1);
        string root2 = find(ip2);
        
        if (root1 == root2) {
            return;
        }
        
        if (rank[root1] < rank[root2]) {
            parent[root1] = root2;
        } 
        else if (rank[root1] > rank[root2]) {
            parent[root2] = root1;
        } 
        else {
            parent[root1] = root2;
            rank[root2]++;
        }
    }
    
    

};