#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <chrono>
#include <algorithm>

#include "../include/Common.h"
#include "../include/Trie.h"
#include "../include/DSU.h"
#include "../include/SkipList.h"
#include "../include/RBTree.h"
#include "../include/BinomialHeap.h"
#include "../include/BPlusTree.h"
#include "../include/SegmentTree.h"

using namespace std;

// ANSI Color Codes
const string C_GREEN = "\033[1;32m";
const string C_RED = "\033[1;31m";
const string C_CYAN = "\033[1;36m";
const string C_GRAY = "\033[1;90m";
const string C_WHITE = "\033[1;37m";
const string C_YELLOW = "\033[1;33m";
const string C_RESET = "\033[0m";

// ============================================================================
// Sanitization Helpers
// ============================================================================
string cleanInput(string s) {
    s.erase(remove_if(s.begin(), s.end(), [](unsigned char c) {
        return !isprint(c) || isspace(c);
    }), s.end());
    return s;
}

bool isValidPath(const string& path) {
    if (path.empty() || path[0] != '/') return false;
    if (path.find("//") != string::npos) return false;
    return true;
}

bool isValidIP(const string& ip) {
    if (ip.empty()) return false;
    stringstream ss(ip);
    string segment;
    int count = 0;
    while (getline(ss, segment, '.')) {
        if (segment.empty() || segment.length() > 3) return false;
        for (char c : segment) if (!isdigit(c)) return false;
        try {
            int val = stoi(segment);
            if (val < 0 || val > 255) return false;
        } catch (...) { return false; }
        count++;
    }
    return count == 4;
}

struct SentinelContext {
    Trie pathValidator;
    DSU ipReputation;
    SkipList l1Cache;
    RBTree masterRegistry;
    BinomialHeap penaltyHeap;
    BPlusTree logSystem;
    SegmentTree analytics;
    long currentTime;
    int totalAllowed;
    int totalBlocked;

    SentinelContext() : currentTime(0), totalAllowed(0), totalBlocked(0) {}
};

void bootstrap(SentinelContext& ctx) {
    ctx.pathValidator.insertPath("/api/v1/login", 5, 60);
    ctx.pathValidator.insertPath("/api/v1/data", 10, 60);
    ctx.pathValidator.insertPath("/api/v1/register", 3, 60);
    ctx.pathValidator.insertPath("/api/admin/config", 2, 60);
    
    ctx.masterRegistry.insert(101, "/api/v1/login", 5, 0);
}

bool parseRequest(const string& line, Request& req, bool& uidValid, bool& tsValid) {
    stringstream ss(line);
    string uidStr, pathStr, ipStr, tsStr;
    if (!(ss >> uidStr >> pathStr >> ipStr >> tsStr)) return false;
    
    req.path = cleanInput(pathStr);
    req.ip = cleanInput(ipStr);
    
    uidValid = true;
    try {
        req.userID = stoi(cleanInput(uidStr));
    } catch (...) {
        uidValid = false;
        req.userID = 0;
    }
    
    tsValid = true;
    try {
        req.timestamp = stol(cleanInput(tsStr));
    } catch (...) {
        tsValid = false;
        req.timestamp = 0;
    }
    
    return true;
}

int main() {
    SentinelContext ctx;
    bootstrap(ctx);

    ifstream fin("requests.txt");
    if (!fin.is_open()) fin.open("requests.txt");
    if (!fin.is_open()) {
        cerr << "Error: Could not open requests.txt" << endl;
        return 1;
    }

    string line;
    int requestID = 1;

    while (getline(fin, line)) {
        if (line.empty()) continue;

        Request req;
        bool uidValid = false, tsValid = false;
        bool parseSuccess = parseRequest(line, req, uidValid, tsValid);
        
        bool isAllowed = true;
        string denialReason = "N/A";
        string l3_cache = "MISS";
        int reqsUsed = 0;
        RateLimit* policy = nullptr;
        bool validationPassed = true;
        bool routeFound = false;
        bool trieHit = false;

        bool ipValid = parseSuccess && isValidIP(req.ip);
        bool pathValid = parseSuccess && isValidPath(req.path);

        // Handle case where parseRequest failed entirely (less than 4 fields)
        if (!parseSuccess) {
            req.path = "MALFORMED_INPUT";
            req.ip = "N/A";
            req.userID = 0;
            ipValid = false;
            pathValid = false;
            uidValid = false;
            tsValid = false;
        }

        // --- L1: VALIDATION ---
        if (!parseSuccess || !ipValid || !pathValid || !uidValid || !tsValid) {
            isAllowed = false;
            validationPassed = false;
            denialReason = "Validation Failed";
        }

        // --- L2: ROUTE MATCH ---
        if (validationPassed) {
            policy = ctx.pathValidator.search(req.path);
            if (policy) {
                routeFound = true;
                trieHit = true;
            } else {
                routeFound = false;
                trieHit = false;
                isAllowed = false;
                denialReason = "Route Not Found";
            }
        }

        // --- L3: LIMIT CHECK ---
        if (routeFound) {
            // Cache Check
            if (ctx.l1Cache.search(req.userID, req.path)) {
                l3_cache = "HIT";
            } else {
                ctx.l1Cache.insert(req);
                l3_cache = "MISS";
            }

            RBNode* userNode = ctx.masterRegistry.search(req.userID, req.path);
            if (!userNode) {
                ctx.masterRegistry.insert(req.userID, req.path, policy->maxRequests, req.timestamp);
                userNode = ctx.masterRegistry.search(req.userID, req.path);
            }

            if (req.timestamp - userNode->lastRefill >= policy->windowSeconds) {
                userNode->tokens = policy->maxRequests;
                userNode->lastRefill = req.timestamp;
            }

            if (userNode->tokens > 0) {
                userNode->tokens--;
            } else {
                isAllowed = false;
                denialReason = "Rate Limit Exceeded";
            }
            reqsUsed = policy->maxRequests - userNode->tokens;
        }

        // --- L4: MONITORING ---
        ctx.logSystem.insert(req.timestamp, req.userID, req.path, isAllowed);
        ctx.analytics.update(req.timestamp, 1);

        // --- OUTPUT ---
        cout << endl;
        cout << C_CYAN << string(70, '=') << C_RESET << endl;
        cout << C_CYAN << "[API_rate_limiter]" << C_RESET << " REQUEST #" << setfill('0') << setw(2) << requestID++ << endl;
        cout << C_CYAN << string(70, '-') << C_RESET << endl;
        cout << C_WHITE << "[REQ] " << req.path << "   | IP: " << req.ip << "   | UID: " << req.userID << C_RESET << endl;
        cout << endl;

        // L1: VALIDATION
        cout << C_CYAN << "[L1: VALIDATION]   " << C_RESET;
        if (validationPassed) {
            cout << C_GREEN << "PASS" << C_RESET;
        } else {
            cout << C_RED << "FAIL" << C_RESET << "   | ";
            cout << "URL: " << (pathValid ? C_GREEN + "VALID" : C_RED + "INVALID") << C_RESET << " | ";
            cout << "IP: " << (ipValid ? C_GREEN + "VALID" : C_RED + "INVALID") << C_RESET << " | ";
            cout << "UID: " << (uidValid ? C_GREEN + "VALID" : C_RED + "INVALID") << C_RESET << " | ";
            cout << "TS: " << (tsValid ? C_GREEN + "VALID" : C_RED + "INVALID") << C_RESET;
        }
        cout << endl;

        // L2: ROUTE MATCH
        cout << C_CYAN << "[L2: ROUTE MATCH]   " << C_RESET;
        if (!validationPassed) {
            cout << C_YELLOW << "SKIPPED" << C_RESET;
        } else if (routeFound) {
            cout << C_GREEN << "PASS" << C_RESET << "   | Trie: " << C_GREEN << "HIT" << C_RESET << "   | Route: " << C_WHITE << req.path << C_RESET;
        } else {
            cout << C_RED << "FAIL" << C_RESET << "   | Trie: " << C_RED << "MISS" << C_RESET << " | Route: " << C_WHITE << req.path << C_RESET;
        }
        cout << endl;

        // L3: LIMIT CHECK
        cout << C_CYAN << "[L3: LIMIT CHECK]   " << C_RESET;
        if (!validationPassed || !routeFound) {
            cout << C_YELLOW << "SKIPPED" << C_RESET;
        } else if (isAllowed) {
            cout << C_GREEN << "PASS" << C_RESET << "   | Requests: " << reqsUsed << "/" << policy->maxRequests 
                 << " | Cache: " << (l3_cache == "HIT" ? C_GREEN : C_RED) << l3_cache << C_RESET;
        } else {
            cout << C_RED << "LIMIT CHECK BLOCKED" << C_RESET << " | Requests: " << reqsUsed << "/" << policy->maxRequests
                 << " | Cache: " << (l3_cache == "HIT" ? C_GREEN : C_RED) << l3_cache << C_RESET;
        }
        cout << endl;

        // L4: MONITORING
        cout << C_CYAN << "[L4: MONITORING]    " << C_RESET << C_GRAY << "RECORDED" << C_RESET << " | Stats Updated" << endl;
        cout << endl;

        // FINAL VERDICT
        cout << C_WHITE << "FINAL VERDICT: " << C_RESET;
        if (isAllowed) {
            cout << C_GREEN << "ALLOWED" << C_RESET << endl;
            ctx.totalAllowed++;
        } else {
            cout << C_RED << "BLOCKED" << C_RESET << endl;
            cout << "REASON: " << denialReason << endl;
            ctx.totalBlocked++;
        }
        cout << C_CYAN << string(70, '=') << C_RESET << endl;
    }

    // --- FINAL SUMMARY ---
    cout << "\n" << C_WHITE << "=== SIMULATION SUMMARY ===" << C_RESET << endl;
    cout << "Total Requests: " << (requestID - 1) << endl;
    cout << "Allowed: " << C_GREEN << ctx.totalAllowed << C_RESET << endl;
    cout << "Blocked: " << C_RED << ctx.totalBlocked << C_RESET << endl;
    cout << C_WHITE << "==========================" << C_RESET << endl;

    fin.close();
    return 0;
}