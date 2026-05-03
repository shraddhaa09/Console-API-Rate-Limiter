// Sharwill_BplusTree.cpp
#include "Sharwill_BplusTree.h"
#include <iostream>

using namespace std;

int main() {
    BPlusTree tree;

    cout << "=== B+ Tree unit test for Sharwill ===\n";

    // Insert 10 normal entries
    for (long ts = 1000; ts < 1010; ++ts) {
        tree.insert(ts, 100, "/api/v1/login", (ts % 2 == 0));
    }
    // One extra entry
    tree.insert(1005, 200, "/api/v1/secure", true);

    cout << "\n1. All logs (in order):\n";
    tree.printAllLogs();

    cout << "\n2. Range query [1004, 1007]:\n";
    vector<LogEntry*> rangeLogs;
    tree.rangeQuery(1004, 1007, rangeLogs);
    for (LogEntry* e : rangeLogs) {
        cout << "  ts=" << e->timestamp
             << " | User " << e->userID
             << " | " << e->path
             << " | " << (e->allowed ? "ALLOWED" : "DENIED") << "\n";
    }

    cout << "\nSuccess! Test completed.\n";
    return 0;
}