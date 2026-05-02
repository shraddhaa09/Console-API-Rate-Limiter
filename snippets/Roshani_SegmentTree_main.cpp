#include <iostream>
#include <cstring>  // for memset
#include "Common.h"
#include "SegmentTree.h"

using namespace std;

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
