#pragma once
#include "Common.h"
#include <iostream>
#include <cstring>  // for memset

using namespace std;

// ============================================================================
// SegmentTree.h — Segment Tree for Real-Time Analytics
// Member 4: The Detective
// ============================================================================
// 
// PURPOSE:
// Track request counts per second over a sliding time window (e.g., last hour).
// Support efficient range queries like "how many requests in the last 5 minutes?"
// or "what was the peak second in the last 10 minutes?"
//
// STRUCTURE:
// - Array-based binary tree built over an array of time slots
// - Each leaf = one second's request count
// - Each internal node = aggregate (sum/max) of its children
//
// KEY OPERATIONS:
// - update(timestamp, delta): Add delta requests to a specific timestamp
// - rangeSum(L, R): Get total requests between timestamps L and R
// - rangeMax(L, R): Get maximum single-second count in range [L, R]
//
// TIME COMPLEXITY: O(log n) for all operations, where n = window size
// ============================================================================

class SegmentTree {
private:
    static const int WINDOW_SIZE = 3600;  // 3600 seconds = 1 hour
    static const int TREE_SIZE = 4 * WINDOW_SIZE;  // Array-based tree needs 4*n space
    
    int sumTree[TREE_SIZE];   // Tree for range sum queries
    int maxTree[TREE_SIZE];   // Tree for range max queries
    
    // Helper: Get the middle of a range
    int getMid(int start, int end) {
        return start + (end - start) / 2;
    }
    
    // ========================================================================
    // buildSumTree: Recursively build the sum tree (called during initialization)
    // ========================================================================
    void buildSumTree(int node, int start, int end) {
        if (start == end) {
            // Leaf node: initialize to 0 (no requests yet)
            sumTree[node] = 0;
        } else {
            int mid = getMid(start, end);
            int leftChild = 2 * node;
            int rightChild = 2 * node + 1;
            
            // Recursively build left and right subtrees
            buildSumTree(leftChild, start, mid);
            buildSumTree(rightChild, mid + 1, end);
            
            // Internal node = sum of children
            sumTree[node] = sumTree[leftChild] + sumTree[rightChild];
        }
    }
    
    // ========================================================================
    // buildMaxTree: Recursively build the max tree
    // ========================================================================
    void buildMaxTree(int node, int start, int end) {
        if (start == end) {
            maxTree[node] = 0;
        } else {
            int mid = getMid(start, end);
            int leftChild = 2 * node;
            int rightChild = 2 * node + 1;
            
            buildMaxTree(leftChild, start, mid);
            buildMaxTree(rightChild, mid + 1, end);
            
            // Internal node = max of children
            maxTree[node] = max(maxTree[leftChild], maxTree[rightChild]);
        }
    }
    
    // ========================================================================
    // updateSumTree: Update a leaf node and propagate changes upward
    // ========================================================================
    void updateSumTree(int node, int start, int end, int idx, int delta) {
        if (start == end) {
            // Leaf node: add delta to current count
            sumTree[node] += delta;
        } else {
            int mid = getMid(start, end);
            int leftChild = 2 * node;
            int rightChild = 2 * node + 1;
            
            if (idx <= mid) {
                // Update left subtree
                updateSumTree(leftChild, start, mid, idx, delta);
            } else {
                // Update right subtree
                updateSumTree(rightChild, mid + 1, end, idx, delta);
            }
            
            // Recalculate this node's value
            sumTree[node] = sumTree[leftChild] + sumTree[rightChild];
        }
    }
    
    // ========================================================================
    // updateMaxTree: Update max tree (similar to sum tree)
    // ========================================================================
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
    
    // ========================================================================
    // querySumTree: Get sum of elements in range [L, R]
    // ========================================================================
    int querySumTree(int node, int start, int end, int L, int R) {
        // No overlap
        if (R < start || L > end) {
            return 0;
        }
        
        // Complete overlap
        if (L <= start && end <= R) {
            return sumTree[node];
        }
        
        // Partial overlap: query both children and combine
        int mid = getMid(start, end);
        int leftChild = 2 * node;
        int rightChild = 2 * node + 1;
        
        int leftSum = querySumTree(leftChild, start, mid, L, R);
        int rightSum = querySumTree(rightChild, mid + 1, end, L, R);
        
        return leftSum + rightSum;
    }
    
    // ========================================================================
    // queryMaxTree: Get maximum element in range [L, R]
    // ========================================================================
    int queryMaxTree(int node, int start, int end, int L, int R) {
        // No overlap
        if (R < start || L > end) {
            return 0;  // Return 0 for no overlap (not -infinity)
        }
        
        // Complete overlap
        if (L <= start && end <= R) {
            return maxTree[node];
        }
        
        // Partial overlap
        int mid = getMid(start, end);
        int leftChild = 2 * node;
        int rightChild = 2 * node + 1;
        
        int leftMax = queryMaxTree(leftChild, start, mid, L, R);
        int rightMax = queryMaxTree(rightChild, mid + 1, end, L, R);
        
        return max(leftMax, rightMax);
    }
    
public:
    // ========================================================================
    // Constructor: Initialize both trees
    // ========================================================================
    SegmentTree() {
        memset(sumTree, 0, sizeof(sumTree));
        memset(maxTree, 0, sizeof(maxTree));
        
        // Build initial trees
        buildSumTree(1, 0, WINDOW_SIZE - 1);
        buildMaxTree(1, 0, WINDOW_SIZE - 1);
    }
    
    // ========================================================================
    // update(timestamp, delta): Increment request count at a specific timestamp
    // In practice, we map timestamp to index (timestamp % WINDOW_SIZE)
    // ========================================================================
    void update(long timestamp, int delta = 1) {
        // Map timestamp to array index (circular buffer)
        int idx = timestamp % WINDOW_SIZE;
        
        // Update both trees
        updateSumTree(1, 0, WINDOW_SIZE - 1, idx, delta);
        updateMaxTree(1, 0, WINDOW_SIZE - 1, idx, delta);
    }
    
    // ========================================================================
    // rangeSum(L, R): Get total requests in index range [L, R]
    // ========================================================================
    int rangeSum(int L, int R) {
        if (L < 0 || R >= WINDOW_SIZE || L > R) {
            cerr << "Invalid range: [" << L << ", " << R << "]" << endl;
            return 0;
        }
        
        return querySumTree(1, 0, WINDOW_SIZE - 1, L, R);
    }
    
    // ========================================================================
    // rangeMax(L, R): Get peak single-second count in range [L, R]
    // ========================================================================
    int rangeMax(int L, int R) {
        if (L < 0 || R >= WINDOW_SIZE || L > R) {
            cerr << "Invalid range: [" << L << ", " << R << "]" << endl;
            return 0;
        }
        
        return queryMaxTree(1, 0, WINDOW_SIZE - 1, L, R);
    }
    
    // ========================================================================
    // rangeSumByTimestamp(t1, t2): Get total requests between timestamps
    // ========================================================================
    int rangeSumByTimestamp(long t1, long t2) {
        int idx1 = t1 % WINDOW_SIZE;
        int idx2 = t2 % WINDOW_SIZE;
        
        // Handle wrap-around case
        if (idx1 <= idx2) {
            return rangeSum(idx1, idx2);
        } else {
            // Wrapped around: [idx1, WINDOW_SIZE-1] + [0, idx2]
            return rangeSum(idx1, WINDOW_SIZE - 1) + rangeSum(0, idx2);
        }
    }
    
    // ========================================================================
    // rangeMaxByTimestamp(t1, t2): Get peak in timestamp range
    // ========================================================================
    int rangeMaxByTimestamp(long t1, long t2) {
        int idx1 = t1 % WINDOW_SIZE;
        int idx2 = t2 % WINDOW_SIZE;
        
        if (idx1 <= idx2) {
            return rangeMax(idx1, idx2);
        } else {
            // Wrapped around
            return max(rangeMax(idx1, WINDOW_SIZE - 1), rangeMax(0, idx2));
        }
    }
    
    // ========================================================================
    // getTotalRequests(): Get sum of all requests in the window
    // ========================================================================
    int getTotalRequests() {
        return sumTree[1];  // Root node contains sum of entire array
    }
    
    // ========================================================================
    // getPeakSecond(): Get maximum single-second count in entire window
    // ========================================================================
    int getPeakSecond() {
        return maxTree[1];  // Root node contains max of entire array
    }
    
    // ========================================================================
    // reset(idx): Clear a specific time slot (useful for sliding window)
    // ========================================================================
    void reset(int idx) {
        if (idx < 0 || idx >= WINDOW_SIZE) return;
        
        // Get current value at this index
        int currentSum = rangeSum(idx, idx);
        
        // Update both trees to subtract current value (set to 0)
        if (currentSum > 0) {
            updateSumTree(1, 0, WINDOW_SIZE - 1, idx, -currentSum);
            updateMaxTree(1, 0, WINDOW_SIZE - 1, idx, -currentSum);
        }
    }
    
    // ========================================================================
    // debugPrint(): Print current state (useful for testing)
    // ========================================================================
    void debugPrint(int numSlots = 60) {
        cout << "\n=== Segment Tree State (showing first " << numSlots << " slots) ===" << endl;
        cout << "Total requests in window: " << getTotalRequests() << endl;
        cout << "Peak single-second count: " << getPeakSecond() << endl;
        
        cout << "\nRequest distribution:" << endl;
        for (int i = 0; i < numSlots && i < WINDOW_SIZE; i++) {
            int count = rangeSum(i, i);
            if (count > 0) {
                cout << "  Slot " << i << ": " << count << " requests" << endl;
            }
        }
        cout << "======================" << endl;
    }
};

// ============================================================================
// TESTING CODE (compile with -DTEST flag to enable)
// ============================================================================
#ifdef TEST

int main() {
    cout << "=== Segment Tree Unit Tests ===" << endl;
    
    SegmentTree segTree;
    
    // Test 1: Basic update and query
    cout << "\nTest 1: Basic Operations" << endl;
    segTree.update(100, 5);   // Add 5 requests at timestamp 100
    segTree.update(101, 3);   // Add 3 requests at timestamp 101
    segTree.update(102, 7);   // Add 7 requests at timestamp 102
    
    int idx100 = 100 % 3600;
    int idx102 = 102 % 3600;
    int total = segTree.rangeSum(idx100, idx102);
    
    cout << "Added requests at timestamps 100, 101, 102" << endl;
    cout << "Total in range [" << idx100 << ", " << idx102 << "]: " << total << endl;
    
    if (total == 15) {
        cout << "✓ Range sum correct (5 + 3 + 7 = 15)" << endl;
    } else {
        cout << "✗ Range sum incorrect! Expected 15, got " << total << endl;
    }
    
    // Test 2: Range max query
    cout << "\nTest 2: Range Max Query" << endl;
    int maxVal = segTree.rangeMax(idx100, idx102);
    
    if (maxVal == 7) {
        cout << "✓ Range max correct (peak = 7)" << endl;
    } else {
        cout << "✗ Range max incorrect! Expected 7, got " << maxVal << endl;
    }
    
    // Test 3: Multiple updates to same timestamp
    cout << "\nTest 3: Incremental Updates" << endl;
    segTree.update(200, 1);
    segTree.update(200, 1);
    segTree.update(200, 1);
    
    int idx200 = 200 % 3600;
    int count200 = segTree.rangeSum(idx200, idx200);
    
    if (count200 == 3) {
        cout << "✓ Incremental updates working (3 requests at slot " << idx200 << ")" << endl;
    } else {
        cout << "✗ Incremental update failed! Expected 3, got " << count200 << endl;
    }
    
    // Test 4: Large range query
    cout << "\nTest 4: Large Range Query" << endl;
    // Add requests across 60 seconds
    for (int t = 1000; t < 1060; t++) {
        segTree.update(t, 2);  // 2 requests per second
    }
    
    int idx1000 = 1000 % 3600;
    int idx1059 = 1059 % 3600;
    int rangeTotal = segTree.rangeSum(idx1000, idx1059);
    
    cout << "Added 2 requests/second for 60 seconds" << endl;
    cout << "Total in 60-second window: " << rangeTotal << endl;
    
    if (rangeTotal == 120) {
        cout << "✓ Large range query correct (60 * 2 = 120)" << endl;
    } else {
        cout << "✗ Large range query failed! Expected 120, got " << rangeTotal << endl;
    }
    
    // Test 5: Peak detection
    cout << "\nTest 5: Peak Detection" << endl;
    segTree.update(2000, 100);  // Spike!
    
    int peakOverall = segTree.getPeakSecond();
    cout << "Peak single-second count in entire window: " << peakOverall << endl;
    
    if (peakOverall >= 100) {
        cout << "✓ Peak detection working" << endl;
    } else {
        cout << "✗ Peak detection failed!" << endl;
    }
    
    // Test 6: Timestamp-based queries
    cout << "\nTest 6: Timestamp-Based Queries" << endl;
    segTree.update(5000, 10);
    segTree.update(5001, 20);
    segTree.update(5002, 30);
    
    int sumByTimestamp = segTree.rangeSumByTimestamp(5000, 5002);
    cout << "Sum from timestamp 5000 to 5002: " << sumByTimestamp << endl;
    
    if (sumByTimestamp == 60) {
        cout << "✓ Timestamp-based query working" << endl;
    } else {
        cout << "✗ Expected 60, got " << sumByTimestamp << endl;
    }
    
    // Test 7: Reset functionality
    cout << "\nTest 7: Reset Slot" << endl;
    int idx5000 = 5000 % 3600;
    int beforeReset = segTree.rangeSum(idx5000, idx5000);
    cout << "Before reset: slot " << idx5000 << " = " << beforeReset << endl;
    
    segTree.reset(idx5000);
    int afterReset = segTree.rangeSum(idx5000, idx5000);
    cout << "After reset: slot " << idx5000 << " = " << afterReset << endl;
    
    if (afterReset == 0) {
        cout << "✓ Reset working" << endl;
    } else {
        cout << "✗ Reset failed!" << endl;
    }
    
    // Test 8: Overall statistics
    cout << "\nTest 8: Overall Statistics" << endl;
    cout << "Total requests tracked: " << segTree.getTotalRequests() << endl;
    cout << "Peak second: " << segTree.getPeakSecond() << endl;
    
    // Debug print (show current state)
    segTree.debugPrint(20);
    
    cout << "\n=== All Tests Complete ===" << endl;
    
    return 0;
}

#endif