// Sharwill_BplusTree.h
#pragma once
#include <iostream>
#include <vector>   // only for return type, not for core structure
#include "Common.h"

using namespace std;

const int MAX_KEYS = 4;  // small order for demo

struct BPlusNode {
    bool isLeaf;
    int numKeys;
    long keys[MAX_KEYS];
    LogEntry* data[MAX_KEYS];
    BPlusNode* children[MAX_KEYS + 1];
    BPlusNode* next;  // links leaf nodes
    BPlusNode* parent;  

    BPlusNode(bool leaf = false);
    ~BPlusNode();
};

struct BPlusNode {
    bool isLeaf;
    int numKeys;
    long keys[MAX_KEYS];
    LogEntry* data[MAX_KEYS];
    BPlusNode* children[MAX_KEYS + 1];
    BPlusNode* next;

    BPlusNode(bool leaf = false);
    ~BPlusNode();
};

class BPlusTree {
private:
    BPlusNode* root;
    BPlusNode* headLeaf;

    void splitNode(BPlusNode* node, BPlusNode*& newNode, long& promotedKey);
    void insertNonRoot(BPlusNode* node, long key, LogEntry* value);
    void rangeQueryHelper(BPlusNode* node, long t1, long t2, vector<LogEntry*>& results);
    void inorderPrintHelper(BPlusNode* node);

    void clean(BPlusNode* node);

public:
    BPlusTree();
    ~BPlusTree();

    // Public API
    void insert(long timestamp, int userID, const string& path, bool allowed);
    void rangeQuery(long t1, long t2, vector<LogEntry*>& results);
    void printAllLogs();
};

// Inline implementations in the header (as per your rule)

inline BPlusNode::BPlusNode(bool leaf)
    : isLeaf(leaf), numKeys(0), next(nullptr)
{
    for (int i = 0; i < MAX_KEYS; ++i) {
        keys[i] = 0;
        data[i] = nullptr;
        children[i] = nullptr;
    }
    children[MAX_KEYS] = nullptr;
}

inline BPlusNode::~BPlusNode() {
    for (int i = 0; i < numKeys; ++i) {
        delete data[i];
    }
}

inline void BPlusTree::splitNode(BPlusNode* node, BPlusNode*& newNode, long& promotedKey) {
    int mid = (MAX_KEYS + 1) / 2;
    promotedKey = node->keys[mid];

    newNode = new BPlusNode(node->isLeaf);

    if (node->isLeaf) {
        for (int i = mid; i < node->numKeys; ++i) {
            newNode->keys[newNode->numKeys] = node->keys[i];
            newNode->data[newNode->numKeys] = node->data[i];
            node->data[i] = nullptr;
            node->keys[i] = 0;
            ++newNode->numKeys;
        }
        node->numKeys = mid;

        newNode->next = node->next;
        node->next = newNode;

        if (headLeaf == newNode) {
            headLeaf = node;
        }
    } else {
        for (int i = mid + 1; i < node->numKeys; ++i) {
            newNode->keys[newNode->numKeys] = node->keys[i];
            node->keys[i] = 0;
            ++newNode->numKeys;
        }
        node->numKeys = mid;

        for (int i = mid + 1; i <= node->numKeys + 1; ++i) {
            node->children[i] = nullptr;
        }
    }
}

inline void BPlusTree::insertNonRoot(BPlusNode* node, long key, LogEntry* value) {
    if (node->isLeaf) {
        int i = node->numKeys - 1;
        while (i >= 0 && key < node->keys[i]) {
            node->keys[i + 1] = node->keys[i];
            node->data[i + 1] = node->data[i];
            --i;
        }
        node->keys[i + 1] = key;
        node->data[i + 1] = value;
        node->numKeys++;

        if (node->numKeys >= MAX_KEYS) {
            BPlusNode* newSibling = nullptr;
            long promotedKey;
            splitNode(node, newSibling, promotedKey);

            if (node->parent == nullptr) {
                BPlusNode* newRoot = new BPlusNode(false);
                node->parent = newRoot;
                newSibling->parent = newRoot;
                newRoot->children[0] = node;
                newRoot->children[1] = newSibling;
                newRoot->keys[0] = promotedKey;
                newRoot->numKeys = 1;
                root = newRoot;
            } else {
                insertNonRoot(node->parent, promotedKey, nullptr);
            }
        }
        return;
    }

    int i = 0;
    while (i < node->numKeys && key >= node->keys[i]) ++i;

    insertNonRoot(node->children[i], key, value);
}

inline void BPlusTree::rangeQueryHelper(BPlusNode* node, long t1, long t2, vector<LogEntry*>& results) {
    if (node->isLeaf) {
        for (int i = 0; i < node->numKeys; ++i) {
            if (node->keys[i] >= t1 && node->keys[i] <= t2) {
                results.push_back(node->data[i]);
            } else if (node->keys[i] > t2) {
                break;
            }
        }
        if (node->next) {
            rangeQueryHelper(node->next, t1, t2, results);
        }
    } else {
        for (int i = 0; i <= node->numKeys; ++i) {
            if (i < node->numKeys && node->keys[i] > t2) break;
            if (i == 0 || (i > 0 && node->keys[i - 1] < t1)) {
                rangeQueryHelper(node->children[i], t1, t2, results);
            }
        }
    }
}

inline void BPlusTree::inorderPrintHelper(BPlusNode* node) {
    if (!node) return;
    if (node->isLeaf) {
        for (int i = 0; i < node->numKeys; ++i) {
            LogEntry* e = node->data[i];
            cout << "ts=" << node->keys[i] << " | "
                 << "User " << e->userID << " | "
                 << e->path << " | "
                 << (e->allowed ? "ALLOWED" : "DENIED") << "\n";
        }
    } else {
        for (int i = 0; i < node->numKeys + 1; ++i) {
            inorderPrintHelper(node->children[i]);
        }
    }
}

inline void BPlusTree::clean(BPlusNode* node) {
    if (!node) return;
    if (!node->isLeaf) {
        for (int i = 0; i < node->numKeys + 1; ++i) {
            clean(node->children[i]);
        }
    }
    delete node;
}

inline BPlusTree::BPlusTree() : root(nullptr), headLeaf(nullptr) {
    root = new BPlusNode(true);
}

inline BPlusTree::~BPlusTree() {
    clean(root);
}

inline void BPlusTree::insert(long timestamp, int userID, const string& path, bool allowed) {
    LogEntry* entry = new LogEntry{timestamp, userID, path, allowed};
    insertNonRoot(root, timestamp, entry);

    BPlusNode* leaf = root;
    while (!leaf->isLeaf && leaf->children[0]) {
        leaf = leaf->children[0];
    }
    headLeaf = leaf;
}

inline void BPlusTree::rangeQuery(long t1, long t2, vector<LogEntry*>& results) {
    results.clear();
    if (root) {
        rangeQueryHelper(root, t1, t2, results);
    }
}

inline void BPlusTree::printAllLogs() {
    if (headLeaf) {
        BPlusNode* leaf = headLeaf;
        while (leaf) {
            inorderPrintHelper(leaf);
            leaf = leaf->next;
        }
    }
}