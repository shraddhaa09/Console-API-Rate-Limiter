#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <ctime>

struct Request {
    int userID;
    std::string path;
    std::string ip;
    long timestamp;
};

struct RateLimit {
    int maxRequests;
    int windowSeconds;
    int currentCount;
    time_t startTime;

    RateLimit() {
        maxRequests = 0;
        windowSeconds = 0;
        currentCount = 0;
        startTime = 0;
    }
};

struct LogEntry {
    long timestamp;
    int userID;
    std::string path;
    bool allowed;
};

struct PenaltyRecord {
    int userID;
    long unlockTimestamp;
    int penaltyLevel;
};

#endif
