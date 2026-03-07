#pragma once

// =============================================================
// TokenBucketLimiter.h
// Assigned to: Member A
//
// This class implements the Token Bucket algorithm.
// It inherits from IRateLimiter — so it MUST implement
// allow_request(), get_stats(), and reset().
//
// Data stored per user:
//   tokens      → how many tokens available right now
//   capacity    → max tokens allowed
//   refill_rate → tokens added per second
//   last_refill → last time tokens were added
// =============================================================

#include "IRateLimiter.h"
#include "RateLimitConfig.h"
#include <string>
#include <unordered_map>
#include <chrono>

// We use this shortcut to avoid writing
// std::chrono::steady_clock::time_point every time
using TimePoint = std::chrono::steady_clock::time_point;


// ── Per-user state ────────────────────────────────────────────
// Every user gets their own copy of this struct
struct TokenBucketState {
    double    tokens;        // current token count
    double    capacity;      // max tokens
    double    refill_rate;   // tokens per second
    TimePoint last_refill;   // last time refill happened
};


// ── The class ─────────────────────────────────────────────────
class TokenBucketLimiter : public IRateLimiter {
private:
    // One state entry per user, looked up by client_id
    std::unordered_map<std::string, TokenBucketState> clients_;

    // The config (max_requests, burst_capacity etc.)
    RateLimitConfig config_;

public:
    // Constructor — takes a config when created
    explicit TokenBucketLimiter(const RateLimitConfig& config);

    // These 3 are required because IRateLimiter demands them
    bool           allow_request(const std::string& client_id) override;
    RateLimitStats get_stats    (const std::string& client_id) const override;
    void           reset        (const std::string& client_id) override;

private:
    // Helper: create a new entry for a user we haven't seen before
    void register_client(const std::string& client_id);
};