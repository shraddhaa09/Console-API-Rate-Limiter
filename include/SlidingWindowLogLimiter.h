#pragma once

// =============================================================
// SlidingWindowLogLimiter.h
// Assigned to: Member C
//
// Implements Sliding Window Log algorithm.
// Stores per user:
//   timestamps → a deque of exact request times
//
// Why deque?
//   push_back (add new) → O(1)
//   pop_front (remove old) → O(1)
//   std::vector would be O(n) for pop_front — too slow
// =============================================================

#include "IRateLimiter.h"
#include "RateLimitConfig.h"
#include <string>
#include <unordered_map>
#include <deque>
#include <chrono>

using TimePoint = std::chrono::steady_clock::time_point;

struct SlidingLogState {
    std::deque<TimePoint> timestamps; // exact time of each request
};

class SlidingWindowLogLimiter : public IRateLimiter {
private:
    std::unordered_map<std::string, SlidingLogState> clients_;
    RateLimitConfig config_;

public:
    explicit SlidingWindowLogLimiter(const RateLimitConfig& config);

    bool           allow_request(const std::string& client_id) override;
    RateLimitStats get_stats    (const std::string& client_id) const override;
    void           reset        (const std::string& client_id) override;

private:
    void register_client(const std::string& client_id);
};