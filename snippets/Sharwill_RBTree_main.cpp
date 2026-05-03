// Sharwill_RBTree.cpp
#include "Sharwill_RBTree.h"
#include <iostream>

using namespace std;

int main() {
    RBTree tree;

    cout << "=== RB Tree unit test for Sharwil (The Librarian) ===\n";

    // Insert 5 users
    tree.insert(50);
    tree.insert(30);
    tree.insert(70);
    tree.insert(20);
    tree.insert(40);

    cout << "\n1. Inorder traversal after inserts:\n";
    tree.inorderPrint();

    // Simulate a request: increment count
    RBNode* u = tree.search(30);
    if (u) {
        cout << "\n2. User 30 found; current count = " << u->requestCount << "\n";
        u->requestCount++;  // within window, before rate‑limit check
    }

    cout << "\n3. Inserting user 100:\n";
    tree.insert(100);

    cout << "\n4. Inorder after more inserts:\n";
    tree.inorderPrint();

    cout << "\n5. Trying to insert user 50 again (should only increment count):\n";
    tree.insert(50);

    u = tree.search(50);
    if (u) {
        cout << "User 50 now has count = " << u->requestCount << "\n";
    }

    cout << "\n6. Removing user 50:\n";
    tree.erase(50);

    cout << "\n7. Inorder after erase 50:\n";
    tree.inorderPrint();

    cout << "\nSuccess! RBTree test completed.\n";
    return 0;
}