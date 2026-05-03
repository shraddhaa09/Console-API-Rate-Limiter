#ifndef SKIPLIST_H
#define SKIPLIST_H

#include "Common.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <string>

using namespace std;

struct SkipNode {
    Request data;
    SkipNode** forward;
    int level;

    SkipNode(int level, const Request& req) {
        this->data = req;
        this->level = level;
        forward = new SkipNode*[level + 1];
        for (int i = 0; i <= level; i++) forward[i] = nullptr;
    }
};

class SkipList {
    int maxLevel;
    float probability;
    int currentLevel;
    SkipNode* header;

    int randomLevel() {
        int lvl = 0;
        while (((double)rand() / RAND_MAX) < probability && lvl < maxLevel) {
            lvl++;
        }
        return lvl;
    }

    bool isLess(const Request& a, int uid, const string& p) {
        if (a.userID != uid) return a.userID < uid;
        return a.path < p;
    }

    bool isEqual(const Request& a, int uid, const string& p) {
        return a.userID == uid && a.path == p;
    }

public:
    SkipList(int maxLevel = 5, float probability = 0.5) {
        this->maxLevel = maxLevel;
        this->probability = probability;
        this->currentLevel = 0;
        header = new SkipNode(maxLevel, Request{0,"","",0});
        srand((unsigned)time(nullptr));
    }

    void insert(const Request& req) {
        SkipNode* update[maxLevel + 1];
        SkipNode* x = header;

        for (int i = currentLevel; i >= 0; i--) {
            while (x->forward[i] && isLess(x->forward[i]->data, req.userID, req.path)) {
                x = x->forward[i];
            }
            update[i] = x;
        }

        x = x->forward[0];
        if (!x || !isEqual(x->data, req.userID, req.path)) {
            int lvl = randomLevel();
            if (lvl > currentLevel) {
                for (int i = currentLevel + 1; i <= lvl; i++) {
                    update[i] = header;
                }
                currentLevel = lvl;
            }
            SkipNode* newNode = new SkipNode(lvl, req);
            for (int i = 0; i <= lvl; i++) {
                newNode->forward[i] = update[i]->forward[i];
                update[i]->forward[i] = newNode;
            }
        } else {
            // Update existing entry with new timestamp
            x->data.timestamp = req.timestamp;
        }
    }

    Request* search(int userID, const string& path) {
        SkipNode* x = header;
        for (int i = currentLevel; i >= 0; i--) {
            while (x->forward[i] && isLess(x->forward[i]->data, userID, path)) {
                x = x->forward[i];
            }
        }
        x = x->forward[0];
        if (x && isEqual(x->data, userID, path)) {
            return &x->data;
        }
        return nullptr;
    }

    int getSize() const {
        int count = 0;
        SkipNode* curr = header->forward[0];
        while (curr) {
            count++;
            curr = curr->forward[0];
        }
        return count;
    }
};

#endif
