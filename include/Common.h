#pragma once
#include <string>

// The Request: flows through the entire system
struct Request {
    int userID;
    std::string path;
    std::string ip;
    long timestamp; // Unix timestamp
};

// The Policy: returned by Trie after checking a path
struct RateLimit {
    int maxRequests;
    int windowSeconds;
};

// The Log: stored in B+ Tree for history
struct LogEntry {
    long timestamp;
    int userID;
    std::string path;
    bool allowed;
};

// The Penalty: stored in Binomial Heap for banned users
struct PenaltyRecord {
    int userID;
    long unlockTimestamp;
};