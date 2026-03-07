#pragma once

// =============================================================
// RateLimitConfig.h
// This file defines:
//   1. The Algorithm enum — names of all 6 algorithms
//   2. RateLimitConfig struct — settings for the limiter
//   3. RateLimitStats struct — stats per user (allowed/denied)
// =============================================================


// ── Algorithm Enum ────────────────────────────────────────────
// This is just a list of names for our 6 algorithms.
// We use this so we can say Algorithm::TOKEN_BUCKET
// instead of using a number like 1, 2, 3...

enum class Algorithm {
    TOKEN_BUCKET,
    FIXED_WINDOW,
    SLIDING_WINDOW_LOG,
    SLIDING_WINDOW_COUNTER,
    SEGMENT_TREE,
    HYBRID
};


// ── RateLimitConfig Struct ────────────────────────────────────
// This holds all the settings.
// When you create a rate limiter, you pass this config to it.
// Example: max 10 requests per 60 seconds, refill 1 token/sec

struct RateLimitConfig {
    int    max_requests   = 10;    // max requests allowed per window
    int    window_size    = 60;    // window size in seconds
    int    burst_capacity = 15;    // token bucket only: max tokens
    double refill_rate    = 1.0;   // token bucket only: tokens per second
    Algorithm algorithm   = Algorithm::TOKEN_BUCKET;
};


// ── RateLimitStats Struct ─────────────────────────────────────
// This holds the live stats for one user.
// After every request, we update these numbers.

#include <string>

struct RateLimitStats {
    std::string client_id;        // who is this user? e.g. "user_42"
    int  total_requests = 0;      // total requests sent so far
    int  allowed        = 0;      // how many were allowed
    int  denied         = 0;      // how many were denied
    double deny_rate    = 0.0;    // denied / total as percentage
};