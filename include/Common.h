#pragma once
#include <string>
#include <iostream>

// The Request: What flows through the entire system
struct Request {
    int userID;
    std::string path;
    std::string ip;
    long timestamp; // Use Unix timestamp (e.g., 1711843200)
};

// The Policy: What the Trie returns after checking a path
struct RateLimit {
    int maxRequests;
    int windowSeconds;
};

// The Log: What the B+ Tree stores for history
struct LogEntry {
    long ts;
    int uid;
    std::string path;
    bool isAllowed;
};

// The Penalty: What the Binomial Heap stores for banned users
struct PenaltyRecord {
    int userID;
    long unlockTime;
};