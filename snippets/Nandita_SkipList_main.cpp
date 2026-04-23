#include <iostream>
#include "Common.h"
#include "SkipList.h"
using namespace std;

int main() {
    SkipList skiplist;

    cout << "\n===========================================\n";
    cout << "          SKIP LIST DEMONSTRATION        \n";
    cout << "===========================================\n";

    int n;
    cout << "Enter number of requests to insert: ";
    cin >> n;
    cin.ignore();

    for (int i = 0; i < n; i++) {
        Request req;
        cout << "\n-------------------------------------------\n";
        cout << " Request " << (i+1) << "\n";
        cout << "-------------------------------------------\n";
        cout << "UserID       : ";
        cin >> req.userID;
        cin.ignore();
        cout << "Path         : ";
        getline(cin, req.path);
        cout << "IP Address   : ";
        getline(cin, req.ip);

        // Auto-generate timestamp
        req.timestamp = time(nullptr);

        // Show the generated timestamp immediately
        cout << "Timestamp    : " << req.timestamp << "\n";

        skiplist.insert(req);
    }

    skiplist.printList();

    cout << "\n===========================================\n";
    cout << "         SEARCH IN SKIP LIST             \n";
    cout << "===========================================\n";

    while (true) {
        int uid;
        cout << "\nEnter UserID to search (or -1 to quit): ";
        cin >> uid;
        if (uid == -1) {
            cout << "\n-------------------------------------------\n";
            cout << "  Program terminated successfully.\n";
            cout << "-------------------------------------------\n";
            break;
        }

        Request* res = skiplist.search(uid);
        cout << "-------------------------------------------\n";
        if (res) {
            cout << " [FOUND]\n";
            cout << " UserID    : " << res->userID << "\n";
            cout << " Path      : " << res->path << "\n";
            cout << " IP        : " << res->ip << "\n";
            cout << " Timestamp : " << res->timestamp << "\n";
        } else {
            cout << " [NOT FOUND] UserID=" << uid << " does not exist.\n";
        }
        cout << "-------------------------------------------\n";
    }

    return 0;
}
