#pragma once

// =============================================================
// FixedWindowLimiter.h
// Assigned to: Member B
//
// Implements Fixed Window algorithm.
// Stores per user:
//   count        → requests in current window
//   window_start → when current window started
// =============================================================

#include "IRateLimiter.h"
#include "RateLimitConfig.h"
#include <string>
#include <unordered_map>
#include <chrono>

using TimePoint = std::chrono::steady_clock::time_point;

struct FixedWindowState {
    int       count;         // requests so far in this window
    TimePoint window_start;  // when this window started
};

class FixedWindowLimiter : public IRateLimiter {
private:
    std::unordered_map<std::string, FixedWindowState> clients_;
    RateLimitConfig config_;

public:
    explicit FixedWindowLimiter(const RateLimitConfig& config);

    bool           allow_request(const std::string& client_id) override;
    RateLimitStats get_stats    (const std::string& client_id) const override;
    void           reset        (const std::string& client_id) override;

private:
    void register_client(const std::string& client_id);
};