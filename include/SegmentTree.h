#pragma once
#include "Common.h"
#include <iostream>
#include <cstring>  

using namespace std;

class SegmentTree {
private:
    static const int WINDOW_SIZE = 3600;
    static const int TREE_SIZE = 4 * WINDOW_SIZE;
    
    int sumTree[TREE_SIZE];
    int maxTree[TREE_SIZE];
    
    int getMid(int start, int end) {
        return start + (end - start) / 2;
    }
    
    void buildSumTree(int node, int start, int end) {
        if (start == end) {
            sumTree[node] = 0;
        } else {
            int mid = getMid(start, end);
            int leftChild = 2 * node;
            int rightChild = 2 * node + 1;
            
            buildSumTree(leftChild, start, mid);
            buildSumTree(rightChild, mid + 1, end);
            
            sumTree[node] = sumTree[leftChild] + sumTree[rightChild];
        }
    }
    
    void buildMaxTree(int node, int start, int end) {
        if (start == end) {
            maxTree[node] = 0;
        } else {
            int mid = getMid(start, end);
            int leftChild = 2 * node;
            int rightChild = 2 * node + 1;
            
            buildMaxTree(leftChild, start, mid);
            buildMaxTree(rightChild, mid + 1, end);
            
            maxTree[node] = max(maxTree[leftChild], maxTree[rightChild]);
        }
    }
    
    void updateSumTree(int node, int start, int end, int idx, int delta) {
        if (start == end) {
            sumTree[node] += delta;
        } else {
            int mid = getMid(start, end);
            int leftChild = 2 * node;
            int rightChild = 2 * node + 1;
            
            if (idx <= mid) {
                updateSumTree(leftChild, start, mid, idx, delta);
            } else {
                updateSumTree(rightChild, mid + 1, end, idx, delta);
            }
            
            sumTree[node] = sumTree[leftChild] + sumTree[rightChild];
        }
    }
    
    void updateMaxTree(int node, int start, int end, int idx, int delta) {
        if (start == end) {
            maxTree[node] += delta;
        } else {
            int mid = getMid(start, end);
            int leftChild = 2 * node;
            int rightChild = 2 * node + 1;
            
            if (idx <= mid) {
                updateMaxTree(leftChild, start, mid, idx, delta);
            } else {
                updateMaxTree(rightChild, mid + 1, end, idx, delta);
            }
            
            maxTree[node] = max(maxTree[leftChild], maxTree[rightChild]);
        }
    }
    
    int querySumTree(int node, int start, int end, int L, int R) {
        if (R < start || L > end) {
            return 0;
        }
        
        if (L <= start && end <= R) {
            return sumTree[node];
        }
        
        int mid = getMid(start, end);
        int leftChild = 2 * node;
        int rightChild = 2 * node + 1;
        
        int leftSum = querySumTree(leftChild, start, mid, L, R);
        int rightSum = querySumTree(rightChild, mid + 1, end, L, R);
        
        return leftSum + rightSum;
    }
    
    int queryMaxTree(int node, int start, int end, int L, int R) {
        if (R < start || L > end) {
            return 0;
        }
        
        if (L <= start && end <= R) {
            return maxTree[node];
        }
        
        int mid = getMid(start, end);
        int leftChild = 2 * node;
        int rightChild = 2 * node + 1;
        
        int leftMax = queryMaxTree(leftChild, start, mid, L, R);
        int rightMax = queryMaxTree(rightChild, mid + 1, end, L, R);
        
        return max(leftMax, rightMax);
    }
    
public:
    SegmentTree() {
        memset(sumTree, 0, sizeof(sumTree));
        memset(maxTree, 0, sizeof(maxTree));
        
        buildSumTree(1, 0, WINDOW_SIZE - 1);
        buildMaxTree(1, 0, WINDOW_SIZE - 1);
    }
    
    void update(long timestamp, int delta = 1) {
        int idx = timestamp % WINDOW_SIZE;
        
        updateSumTree(1, 0, WINDOW_SIZE - 1, idx, delta);
        updateMaxTree(1, 0, WINDOW_SIZE - 1, idx, delta);
    }
    
    
    
    

};