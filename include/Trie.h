#ifndef TRIE_H
#define TRIE_H

#include "Common.h"
#include <iostream>
#include <string>
#include <ctime>
using namespace std;

// Node structure for Trie
struct TrieNode {
    TrieNode* children[128];
    bool isEndOfPath;
    bool isBlocked;
    RateLimit policy;

    TrieNode() {
        for (int i = 0; i < 128; i++) children[i] = nullptr;
        isEndOfPath = false;
        isBlocked = false;
    }
};

class Trie {
    TrieNode* root;

public:
    Trie() { root = new TrieNode(); }

    // Insert a valid API path with its rate limit policy
    void insertPath(const string& path, int maxReq, int windowSec) {
        TrieNode* node = root;
        for (char c : path) {
            if (!node->children[(int)c])
                node->children[(int)c] = new TrieNode();
            node = node->children[(int)c];
        }
        node->isEndOfPath = true;
        node->policy.maxRequests = maxReq;
        node->policy.windowSeconds = windowSec;
        node->policy.currentCount = 0;
        node->policy.startTime = time(nullptr);
    }

    // Search for a path and return its policy if found
    RateLimit* search(const string& path) {
        TrieNode* node = root;
        for (char c : path) {
            if (!node->children[(int)c]) return nullptr;
            node = node->children[(int)c];
            if (node->isBlocked) return nullptr;
        }

        if (node->isEndOfPath) {
            return &node->policy;
        }
        return nullptr;
    }

    // Block all paths under a given prefix
    void prefixBlock(const string& prefix) {
        TrieNode* node = root;
        for (char c : prefix) {
            if (!node->children[(int)c]) return;
            node = node->children[(int)c];
        }
        node->isBlocked = true;
    }

    // Print all stored paths (for final snapshot)
    void printAllPaths(string current = "", TrieNode* node = nullptr) {
        if (!node) node = root;
        if (node->isEndOfPath)
            cout << "  [PATH] " << left << setw(20) << current 
                 << " | Limit: " << node->policy.maxRequests << " req / "
                 << node->policy.windowSeconds << " sec" << endl;
        for (int i = 0; i < 128; i++) {
            if (node->children[i])
                printAllPaths(current + (char)i, node->children[i]);
        }
    }
};

#endif
