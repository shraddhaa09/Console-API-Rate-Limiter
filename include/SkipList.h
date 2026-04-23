#ifndef SKIPLIST_H
#define SKIPLIST_H

#include "Common.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <iomanip>   // for setw()
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
            while (x->forward[i] && x->forward[i]->data.userID < req.userID) {
                x = x->forward[i];
            }
            update[i] = x;
        }

        x = x->forward[0];
        if (!x || x->data.userID != req.userID) {
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
        }
    }

    Request* search(int userID) {
        SkipNode* x = header;
        for (int i = currentLevel; i >= 0; i--) {
            while (x->forward[i] && x->forward[i]->data.userID < userID) {
                x = x->forward[i];
            }
        }
        x = x->forward[0];
        if (x && x->data.userID == userID) {
            return &x->data;
        }
        return nullptr;
    }

    void printList() {
        cout << "\n===========================================\n";
        cout << "         SKIP LIST CONTENTS              \n";
        cout << "===========================================\n";

        for (int i = 0; i <= currentLevel; i++) {
            cout << "\nLevel " << i << " -->\n";
            cout << "+---------+------------------+---------------+------------+\n";
            cout << "| UserID  | Path             | IP Address    | Timestamp  |\n";
            cout << "+---------+------------------+---------------+------------+\n";

            SkipNode* node = header->forward[i];
            while (node) {
                cout << "| " << setw(7) << node->data.userID
                     << " | " << setw(16) << node->data.path
                     << " | " << setw(13) << node->data.ip
                     << " | " << setw(10) << node->data.timestamp << " |\n";
                node = node->forward[i];
            }

            cout << "+---------+------------------+---------------+------------+\n";
        }
    }
};

#endif
