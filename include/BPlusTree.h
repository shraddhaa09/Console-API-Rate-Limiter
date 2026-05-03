#pragma once
#include <iostream>
#include <vector>   
#include "Common.h"

using namespace std;

const int MAX_KEYS = 4;

struct BPlusNode {
    bool isLeaf;
    int numKeys;
    long keys[MAX_KEYS];
    LogEntry* data[MAX_KEYS];
    BPlusNode* children[MAX_KEYS + 1];
    BPlusNode* next;
    BPlusNode* parent;

    BPlusNode(bool leaf = false);
    ~BPlusNode();
};

class BPlusTree {
private:
    BPlusNode* root;
    BPlusNode* headLeaf;

    void splitNode(BPlusNode* node, BPlusNode*& newNode, long& promotedKey);
    void insertNonRoot(BPlusNode* node, long key, LogEntry* value);
    void clean(BPlusNode* node);

public:
    BPlusTree();
    ~BPlusTree();

    void insert(long timestamp, int userID, const string& path, bool allowed);
};

inline BPlusNode::BPlusNode(bool leaf)
    : isLeaf(leaf), numKeys(0), next(nullptr), parent(nullptr)
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