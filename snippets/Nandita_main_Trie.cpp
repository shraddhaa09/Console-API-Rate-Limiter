#include <iostream>
#include <string>
#include "Common.h"
#include "Trie.h"
using namespace std;

int main() {
    Trie trie;

    cout << "\n=====================================\n";
    cout << "        API PATH REGISTRATION        \n";
    cout << "=====================================\n";

    int n;
    cout << "Enter number of API paths to register: ";
    cin >> n;
    cin.ignore();

    for (int i = 0; i < n; i++) {
        string path;
        int maxReq, windowSec;
        cout << "\nPath " << (i+1) << ": ";
        getline(cin, path);
        cout << "Max requests: ";
        cin >> maxReq;
        cout << "Window seconds: ";
        cin >> windowSec;
        cin.ignore();
        trie.insertPath(path, maxReq, windowSec);
    }

    cout << "\n--- Registered API Paths ---\n";
    trie.printAllPaths();

    cout << "\n=====================================\n";
    cout << "          PREFIX BLOCKING             \n";
    cout << "=====================================\n";
    string prefix;
    cout << "Enter a prefix to block (or press Enter to skip): ";
    getline(cin, prefix);
    if (!prefix.empty()) {
        trie.prefixBlock(prefix);
        cout << "[BLOCKED] Prefix '" << prefix << "' blocked successfully.\n";
    }

    cout << "\n=====================================\n";
    cout << "     REAL-TIME REQUEST VALIDATION     \n";
    cout << "=====================================\n";

    while (true) {
        string reqPath;
        cout << "\nEnter request path (or type 'exit' to quit): ";
        getline(cin, reqPath);

        if (reqPath == "exit") {
            cout << "\nProgram terminated.\n";
            break;
        }

        RateLimit* res = trie.search(reqPath);
        if (res) {
            cout << "Request: " << reqPath
                 << "  -> VALID | Used: " << res->currentCount
                 << "/" << res->maxRequests
                 << " requests in " << res->windowSeconds << "s window\n";
        } else {
            if (!prefix.empty() && reqPath.find(prefix) == 0) {
                cout << "Request: " << reqPath
                     << "  -> BLOCKED | Reason: Prefix '" << prefix << "' is restricted\n";
            } else {
                cout << "Request: " << reqPath
                     << "  -> INVALID | Reason: Path not registered or limit exceeded\n";
            }
        }
    }

    return 0;
}
