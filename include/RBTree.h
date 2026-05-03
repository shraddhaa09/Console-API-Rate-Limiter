#pragma once
#include <iostream>
#include <string>
#include "Common.h"

using namespace std;

enum Color { RED, BLACK };

struct RBNode {
    int userID;
    string path;
    int tokens;
    long lastRefill;
    Color color;
    RBNode* left;
    RBNode* right;
    RBNode* parent;

    RBNode(int uid, string p, int t, long lr)
        : userID(uid), path(p), tokens(t), lastRefill(lr), color(RED), left(nullptr), right(nullptr), parent(nullptr) {}
};

class RBTree {
private:
    RBNode* root;
    RBNode* NIL;

    RBNode* createNode(int uid, string p, int t, long lr) {
        return new RBNode(uid, p, t, lr);
    }

    bool isLess(int uid1, const string& p1, int uid2, const string& p2) {
        if (uid1 != uid2) return uid1 < uid2;
        return p1 < p2;
    }

    bool isEqual(int uid1, const string& p1, int uid2, const string& p2) {
        return uid1 == uid2 && p1 == p2;
    }

    void leftRotate(RBNode* x);
    void rightRotate(RBNode* y);
    void fixInsert(RBNode* z);
    RBNode* treeMin(RBNode* x);
    void transplant(RBNode* u, RBNode* v);
    void fixDelete(RBNode* x);
    void deleteNode(RBNode* z);
    void inorderHelper(RBNode* node);
    void clean(RBNode* node);

public:
    RBTree();
    ~RBTree();

    RBNode* search(int userID, const string& path);
    bool insert(int userID, const string& path, int initialTokens, long timestamp);
    void inorderPrint();
};

inline void RBTree::inorderHelper(RBNode* node) {
    if (node == NIL) return;
    inorderHelper(node->left);
    cout << "  [USER] ID: " << node->userID 
         << " | Path: " << node->path 
         << " | Tokens: " << node->tokens << endl;
    inorderHelper(node->right);
}

inline void RBTree::inorderPrint() {
    inorderHelper(root);
}

inline void RBTree::leftRotate(RBNode* x) {
    RBNode* y = x->right;
    x->right = y->left;
    if (y->left != NIL) y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == NIL) root = y;
    else if (x == x->parent->left) x->parent->left = y;
    else x->parent->right = y;
    y->left = x;
    x->parent = y;
}

inline void RBTree::rightRotate(RBNode* y) {
    RBNode* x = y->left;
    y->left = x->right;
    if (x->right != NIL) x->right->parent = y;
    x->parent = y->parent;
    if (y->parent == NIL) root = x;
    else if (y == y->parent->left) y->parent->left = x;
    else y->parent->right = x;
    x->right = y;
    y->parent = x;
}

inline void RBTree::fixInsert(RBNode* z) {
    RBNode* y;
    while (z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) {
            y = z->parent->parent->right;
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    leftRotate(z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rightRotate(z->parent->parent);
            }
        } else {
            y = z->parent->parent->left;
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    rightRotate(z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                leftRotate(z->parent->parent);
            }
        }
    }
    root->color = BLACK;
}

inline RBNode* RBTree::treeMin(RBNode* x) {
    while (x->left != NIL) x = x->left;
    return x;
}

inline void RBTree::transplant(RBNode* u, RBNode* v) {
    if (u->parent == NIL) root = v;
    else if (u == u->parent->left) u->parent->left = v;
    else u->parent->right = v;
    v->parent = u->parent;
}

inline void RBTree::fixDelete(RBNode* x) {
    while (x != root && x->color == BLACK) {
        RBNode* w;
        if (x == x->parent->left) {
            w = x->parent->right;
            if (w->color == RED) {
                w->color = BLACK; x->parent->color = RED;
                leftRotate(x->parent); w = x->parent->right;
            }
            if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED; x = x->parent;
            } else {
                if (w->right->color == BLACK) {
                    w->left->color = BLACK; w->color = RED;
                    rightRotate(w); w = x->parent->right;
                }
                w->color = x->parent->color; x->parent->color = BLACK;
                w->right->color = BLACK; leftRotate(x->parent); x = root;
            }
        } else {
            w = x->parent->left;
            if (w->color == RED) {
                w->color = BLACK; x->parent->color = RED;
                rightRotate(x->parent); w = x->parent->left;
            }
            if (w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED; x = x->parent;
            } else {
                if (w->left->color == BLACK) {
                    w->right->color = BLACK; w->color = RED;
                    leftRotate(w); w = x->parent->left;
                }
                w->color = x->parent->color; x->parent->color = BLACK;
                w->left->color = BLACK; rightRotate(x->parent); x = root;
            }
        }
    }
    x->color = BLACK;
}

inline void RBTree::deleteNode(RBNode* z) {
    RBNode* y = z;
    RBNode* x;
    Color yOrigColor = y->color;
    if (z->left == NIL) {
        x = z->right; transplant(z, z->right);
    } else if (z->right == NIL) {
        x = z->left; transplant(z, z->left);
    } else {
        y = treeMin(z->right); yOrigColor = y->color;
        x = y->right;
        if (y->parent == z) x->parent = y;
        else {
            transplant(y, y->right); y->right = z->right; y->right->parent = y;
        }
        transplant(z, y); y->left = z->left; y->left->parent = y; y->color = z->color;
    }
    delete z;
    if (yOrigColor == BLACK) fixDelete(x);
}

inline void RBTree::clean(RBNode* node) {
    if (node == NIL) return;
    clean(node->left);
    clean(node->right);
    delete node;
}

inline RBTree::RBTree() {
    NIL = new RBNode(0, "", 0, 0);
    NIL->color = BLACK;
    NIL->left = NIL->right = NIL->parent = NIL;
    root = NIL;
}

inline RBTree::~RBTree() {
    clean(root);
    delete NIL;
}

inline RBNode* RBTree::search(int userID, const string& path) {
    RBNode* x = root;
    while (x != NIL && !isEqual(x->userID, x->path, userID, path)) {
        if (isLess(userID, path, x->userID, x->path)) x = x->left;
        else x = x->right;
    }
    return (x != NIL) ? x : nullptr;
}

inline bool RBTree::insert(int userID, const string& path, int initialTokens, long timestamp) {
    RBNode* z = createNode(userID, path, initialTokens, timestamp);
    RBNode* y = NIL;
    RBNode* x = root;

    while (x != NIL) {
        y = x;
        if (isLess(z->userID, z->path, x->userID, x->path)) x = x->left;
        else if (isLess(x->userID, x->path, z->userID, z->path)) x = x->right;
        else {
            delete z;
            return false;
        }
    }

    z->parent = y;
    if (y == NIL) root = z;
    else if (isLess(z->userID, z->path, y->userID, y->path)) y->left = z;
    else y->right = z;

    z->left = NIL;
    z->right = NIL;
    z->color = RED;
    fixInsert(z);
    return true;
}