#pragma once
#include <iostream>
#include <vector>
#include <climits>
#include "Common.h"
using namespace std;

struct BinomialNode {
    int userID;
    long unlockTime;
    int degree;
    BinomialNode* parent;
    BinomialNode* child;
    BinomialNode* sibling;

    BinomialNode(int uid, long time) {
        userID = uid;
        unlockTime = time;
        degree = 0;
        parent = nullptr;
        child = nullptr;
        sibling = nullptr;
    }
};

class BinomialHeap {
private:
    BinomialNode* head;

    BinomialNode* linkTrees(BinomialNode* a, BinomialNode* b) {
        if (!a) return b;
        if (!b) return a;

        if (a->unlockTime > b->unlockTime)
            swap(a, b);

        b->parent = a;
        b->sibling = a->child;
        a->child = b;
        a->degree++;

        return a;
    }

    BinomialNode* mergeRootLists(BinomialNode* h1, BinomialNode* h2) {
        if (!h1) return h2;
        if (!h2) return h1;

        BinomialNode* newHead = nullptr;
        BinomialNode* tail = nullptr;

        if (h1->degree <= h2->degree) {
            newHead = h1;
            h1 = h1->sibling;
        } else {
            newHead = h2;
            h2 = h2->sibling;
        }

        tail = newHead;

        while (h1 && h2) {
            if (h1->degree <= h2->degree) {
                tail->sibling = h1;
                h1 = h1->sibling;
            } else {
                tail->sibling = h2;
                h2 = h2->sibling;
            }
            tail = tail->sibling;
        }

        tail->sibling = (h1 ? h1 : h2);
        return newHead;
    }

    BinomialNode* unionHeaps(BinomialNode* h1, BinomialNode* h2) {
        BinomialNode* newHead = mergeRootLists(h1, h2);
        if (!newHead) return nullptr;

        BinomialNode* prev = nullptr;
        BinomialNode* curr = newHead;
        BinomialNode* next = curr->sibling;

        while (next != nullptr) {
            if ((curr->degree != next->degree) ||
                (next->sibling != nullptr && next->sibling->degree == curr->degree)) {
                prev = curr;
                curr = next;
            } else {
                if (curr->unlockTime <= next->unlockTime) {
                    curr->sibling = next->sibling;
                    curr = linkTrees(curr, next);
                } else {
                    if (prev == nullptr)
                        newHead = next;
                    else
                        prev->sibling = next;

                    curr = linkTrees(next, curr);
                }
            }
            next = curr->sibling;
        }

        return newHead;
    }

    BinomialNode* reverseChildren(BinomialNode* node) {
        BinomialNode* prev = nullptr;
        BinomialNode* curr = node;

        while (curr != nullptr) {
            BinomialNode* next = curr->sibling;
            curr->sibling = prev;
            curr->parent = nullptr;
            prev = curr;
            curr = next;
        }

        return prev;
    }

    void collectUnlocked(BinomialNode* root, vector<PenaltyRecord>& out) {
        if (!root) return;

        out.push_back({root->userID, root->unlockTime});
        collectUnlocked(root->child, out);
        collectUnlocked(root->sibling, out);
    }

public:
    BinomialHeap() {
        head = nullptr;
    }

    bool empty() const {
        return head == nullptr;
    }

    BinomialNode* getHead() const {
        return head;
    }

    void merge(BinomialHeap& other) {
        head = unionHeaps(head, other.head);
        other.head = nullptr;
    }

    BinomialNode* insert(int userID, long unlockTime) {
        BinomialHeap temp;
        temp.head = new BinomialNode(userID, unlockTime);
        head = unionHeaps(head, temp.head);
        temp.head = nullptr;
        return findNodeByUserID(userID);
    }

    BinomialNode* findMin() const {
        if (!head) return nullptr;

        BinomialNode* minNode = head;
        BinomialNode* curr = head->sibling;

        while (curr != nullptr) {
            if (curr->unlockTime < minNode->unlockTime)
                minNode = curr;
            curr = curr->sibling;
        }

        return minNode;
    }

    PenaltyRecord extractMin() {
        if (!head) return {-1, -1};

        BinomialNode* minNode = head;
        BinomialNode* minPrev = nullptr;
        BinomialNode* prev = nullptr;
        BinomialNode* curr = head;

        long best = head->unlockTime;

        while (curr != nullptr) {
            if (curr->unlockTime < best) {
                best = curr->unlockTime;
                minNode = curr;
                minPrev = prev;
            }
            prev = curr;
            curr = curr->sibling;
        }

        if (minPrev == nullptr)
            head = minNode->sibling;
        else
            minPrev->sibling = minNode->sibling;

        BinomialNode* childHeap = reverseChildren(minNode->child);
        head = unionHeaps(head, childHeap);

        PenaltyRecord ans = {minNode->userID, minNode->unlockTime};
        delete minNode;
        return ans;
    }

    BinomialNode* findNodeByUserID(int userID) const {
        if (!head) return nullptr;

        vector<BinomialNode*> st;
        BinomialNode* curr = head;

        while (curr != nullptr) {
            st.push_back(curr);
            curr = curr->sibling;
        }

        while (!st.empty()) {
            BinomialNode* node = st.back();
            st.pop_back();

            if (node->userID == userID)
                return node;

            BinomialNode* child = node->child;
            while (child != nullptr) {
                st.push_back(child);
                child = child->sibling;
            }
        }

        return nullptr;
    }

    bool decreaseKey(BinomialNode* node, long newUnlockTime) {
        if (!node) return false;
        if (newUnlockTime > node->unlockTime) return false;

        node->unlockTime = newUnlockTime;
        BinomialNode* curr = node;
        BinomialNode* parent = curr->parent;

        while (parent != nullptr && curr->unlockTime < parent->unlockTime) {
            swap(curr->unlockTime, parent->unlockTime);
            swap(curr->userID, parent->userID);

            curr = parent;
            parent = curr->parent;
        }

        return true;
    }

    vector<PenaltyRecord> processUnlocks(long currentTime) {
        vector<PenaltyRecord> unlocked;

        while (!empty()) {
            BinomialNode* minNode = findMin();
            if (!minNode || minNode->unlockTime > currentTime)
                break;

            unlocked.push_back(extractMin());
        }

        return unlocked;
    }

    void printTree(BinomialNode* root, int depth) const {
        while (root != nullptr) {
            for (int i = 0; i < depth; i++) cout << "  ";
            cout << "(UID=" << root->userID
                 << ", unlock=" << root->unlockTime
                 << ", deg=" << root->degree << ")\n";

            if (root->child)
                printTree(root->child, depth + 1);

            root = root->sibling;
        }
    }

    void printHeap() const {
        if (!head) {
            cout << "  [PENALTY] Queue is empty.\n";
            return;
        }
        BinomialNode* curr = head;
        while (curr != nullptr) {
            printTree(curr, 1);
            curr = curr->sibling;
        }
    }
};