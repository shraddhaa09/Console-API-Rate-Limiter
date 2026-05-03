#ifndef TRIE_H
#define TRIE_H

#include "Common.h"
#include <iostream>
#include <string>
#include <ctime>
using namespace std;

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

};

#endif
