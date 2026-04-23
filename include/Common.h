#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <ctime>

// The Request: flows through the entire system
struct Request {
    int userID;
    std::string path;
    std::string ip;
    long timestamp; // Unix timestamp
};

// The Policy: returned by Trie after checking a path
struct RateLimit {
    int maxRequests;     // maximum allowed requests
    int windowSeconds;   // time window in seconds
    int currentCount;    // how many requests used in current window
    time_t startTime;    // when the current window started

    RateLimit() {
        maxRequests = 0;
        windowSeconds = 0;
        currentCount = 0;
        startTime = 0;
    }
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
    int penaltyLevel;   // 1 = first violation, 2 = second, etc.
};

#endif
