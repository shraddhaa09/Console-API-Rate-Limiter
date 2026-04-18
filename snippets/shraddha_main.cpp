// // shraddha_main.cpp
// // Use this as a template for your final main.cpp
// // Integrates: BinomialHeap + Common.h + requests.txt

// #include <iostream>
// #include <fstream>
// #include <vector>
// #include <sstream>
// #include <string>
// #include <cstdlib>
// #include "Common.h"
// #include "BinomialHeap.h"

// using namespace std;

// // Simulation constants
// constexpr long WINDOW_SECONDS = 60;  // 1 minute
// constexpr long CHECK_INTERVAL = 1;   // 1 second between checks

// // Example helper: parse a line from requests.txt
// // Format: "userID path ip timestamp"
// // Example line: "101 /api/v1/login 192.168.1.5 1700000005"
// Request parseRequestLine(const string& line) {
//     Request r;
//     stringstream ss(line);
//     ss >> r.userID >> r.path;
//     string ip_str;
//     ss >> ip_str;
//     r.ip = ip_str;
//     long ts;
//     ss >> ts;
//     r.timestamp = ts;
//     return r;
// }

// // Example: hard‑coded valid paths and their limits
// // Member 3 (Trie) will ultimately provide this via trie_search
// RateLimit getMockRateLimit(const string& path) {
//     if (path == "/api/v1/login")
//         return {5, 60};  // 5 requests per minute
//     else if (path == "/api/v1/register")
//         return {3, 60};
//     else if (path == "/api/admin/users")
//         return {10, 60};
//     else
//         return {-1, -1};  // invalid path
// }

// // Example: simulate DSU check (Member 4)
// bool isBannedMock(const string& ip) {
//     // In your team’s DSU, this will be: return DSU.isBanned(ip);
//     // For now, hard‑code a banned IP to test
//     if (ip == "10.0.0.1" || ip == "10.0.0.2" || ip == "10.0.0.3") {
//         return true;
//     }
//     return false;
// }

// // Example: simulated user profile in RB‑Tree / Skip‑List
// struct UserProfile {
//     int userID;
//     int requestCount;
//     long windowStart;

//     UserProfile(int uid)
//         : userID(uid), requestCount(0), windowStart(0) {}
// };

// // Simple mock for "user registry" (Member 1 + 3)
// UserProfile* getMockUser(int userID, long timestamp) {
//     // In your final version, this will come from:
//     // 1) Skip‑List HIT  → Member 3
//     // 2) RB‑Tree MISS → Member 1
//     // For now, keep a tiny mock registry
//     static vector<UserProfile> users = {
//         UserProfile(101),
//         UserProfile(102),
//         UserProfile(103),
//         UserProfile(202)
//     };

//     for (UserProfile& u : users) {
//         if (u.userID == userID) {
//             if (timestamp - u.windowStart >= WINDOW_SECONDS) {
//                 // reset window after 60s
//                 u.requestCount = 0;
//                 u.windowStart = timestamp;
//             }
//             return &u;
//         }
//     }
//     return nullptr;
// }

// // Example: mock Segment Tree / B+ Tree logging (Members 1 + 4)
// void recordLog(const Request& r, bool allowed) {
//     LogEntry entry = {r.timestamp, r.userID, r.path, allowed};
//     // Later, Member 1 (B+ Tree) will insert this into B+ Tree
//     // For now, just print
//     cout << "[B+ Tree] Request logged at timestamp " << entry.ts << endl;
// }

// void updateAnalytics(long currentTime) {
//     // Later, Member 4 (Segment Tree) will update an array + segment tree
//     // For now, do nothing or just print
//     // cout << "[Segment Tree] Updated analytics for time " << currentTime << endl;
// }

// int main() {
//     BinomialHeap penaltyHeap;

//     // Read requests from file
//     ifstream fin("requests.txt");
//     if (!fin.is_open()) {
//         cerr << "Error: cannot open requests.txt" << endl;
//         return 1;
//     }

//     string line;
//     long currentTime = 0;

//     while (getline(fin, line)) {
//         if (line.empty())
//             continue;

//         Request req = parseRequestLine(line);

//         // Simulate clock: requests are in increasing timestamp
//         if (req.timestamp > currentTime) {
//             currentTime = req.timestamp;

//             // Every tick, check if any bans are over
//             vector<PenaltyRecord> released = penaltyHeap.processUnlocks(currentTime);
//             if (!released.empty()) {
//                 cout << "=== Time " << currentTime << " ===" << endl;
//                 for (const PenaltyRecord& pr : released) {
//                     cout << "[BinomialHeap] User " << pr.userID
//                          << " unbanned at " << pr.unlockTime << endl;
//                 }
//             }
//         }

//         cout << "=== Incoming request ===" << endl;
//         cout << "User: " << req.userID
//              << ", Path: " << req.path
//              << ", IP: " << req.ip
//              << ", Time: " << req.timestamp << endl;

//         // --- Phase 1: Path validation (Trie, Member 3) ---
//         RateLimit rl = getMockRateLimit(req.path);
//         if (rl.maxRequests == -1) {
//             cout << "[Trie] 	Path validation FAILED: invalid path " << req.path << " 404" << endl;
//             continue;
//         }
//         cout << "[Trie] 	Path validated: " << req.path
//              << " (limit: " << rl.maxRequests << " req/min)" << endl;

//         // --- Phase 1: DSU check — IP ban (Member 4) ---
//         if (isBannedMock(req.ip)) {
//             cout << "[DSU]  	IP " << req.ip << ": BANNED" << endl;
//             cout << "[Verdict]  DENIED (IP ban)" << endl;
//             recordLog(req, false);
//             continue;
//         }
//         cout << "[DSU]  	IP " << req.ip << ": CLEAN" << endl;

//         // --- Phase 2: Find user profile (RB‑Tree / Skip‑List, Members 1 + 3) ---
//         UserProfile* user = getMockUser(req.userID, req.timestamp);
//         if (!user) {
//             cout << "[RB‑Tree]  User " << req.userID << " not found" << endl;
//             continue;
//         }
//         cout << "[SkipList] Cache HIT — User " << user->userID << " found" << endl;

//         // --- Phase 3: Verdict — allow or deny? ---
//         if (user->requestCount < rl.maxRequests) {
//             user->requestCount++;
//             cout << "[Verdict]  ALLOWED — count: " << user->requestCount << "/"
//                  << rl.maxRequests << endl;

//             // Record analytics
//             updateAnalytics(req.timestamp);

//         } else {
//             long unlockTime = req.timestamp + rl.windowSeconds;
//             cout << "[Verdict]  DENIED (rate limit) — will be allowed at " << unlockTime << endl;

//             // Push to Binomial Heap (Member 2)
//             BinomialNode* node = penaltyHeap.insert(req.userID, unlockTime);
//             if (node) {
//                 cout << "[BinomialHeap] User " << req.userID
//                      << " added to penalty queue, unlock at " << unlockTime << endl;
//             }
//         }

//         // Log every request (Member 1, B+ Tree)
//         recordLog(req, user->requestCount <= rl.maxRequests);

//         cout << endl;
//     }

//     // Final internal state dump (for debugging)
//     if (!penaltyHeap.empty()) {
//         cout << "=== Final Penalty Queue State ===" << endl;
//         penaltyHeap.printRootList();
//         penaltyHeap.printHeap();
//     } else {
//         cout << "=== Final Penalty Queue State ===" << endl;
//         cout << "Penalty Heap is empty." << endl;
//     }

//     fin.close();
//     return 0;
// }