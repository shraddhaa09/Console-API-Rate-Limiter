#pragma once

// =============================================================
// SlidingWindowCounterLimiter.h
// Assigned to: Member D
//
// WHAT THIS FILE DOES:
// This is the header file — it tells the compiler
// "this class exists and has these functions and variables"
// The actual logic is written in the .cpp file
//
// DATA STORED PER USER:
// prev_count   → how many requests in the PREVIOUS window
// curr_count   → how many requests in the CURRENT window
// window_start → when did the current window start
//
// FORMULA USED:
// position  = (now - window_start) / window_size
// estimated = prev_count x (1 - position) + curr_count
// =============================================================

#include "IRateLimiter.h"
#include "RateLimitConfig.h"
#include <string>
#include <unordered_map>
#include <chrono>

using TimePoint = std::chrono::steady_clock::time_point;


// ── Per user data ─────────────────────────────────────────────
// Every user gets their own copy of these 3 variables
struct SlidingCounterState {
    int       prev_count;    // requests in previous window
    int       curr_count;    // requests in current window so far
    TimePoint window_start;  // when current window started
};


// ── The class ─────────────────────────────────────────────────
class SlidingWindowCounterLimiter : public IRateLimiter {
private:
    // One state entry per user
    // Key   = client_id (like "user_42")
    // Value = their SlidingCounterState
    std::unordered_map<std::string, SlidingCounterState> clients_;

    // Settings (max_requests, window_size etc.)
    RateLimitConfig config_;

public:
    // Constructor — pass config when creating the object
    explicit SlidingWindowCounterLimiter(const RateLimitConfig& config);

    // These 3 are required by IRateLimiter
    bool           allow_request(const std::string& client_id) override;
    RateLimitStats get_stats    (const std::string& client_id) const override;
    void           reset        (const std::string& client_id) override;

private:
    // Creates fresh state for a user we have not seen before
    void register_client(const std::string& client_id);
};
