// =============================================================
// TokenBucketLimiter.cpp
// Assigned to: Member A (Shraddha)
//
// This file contains the ACTUAL LOGIC of Token Bucket.
// The header file (TokenBucketLimiter.h) said WHAT functions
// exist. This file writes HOW they actually work.
// =============================================================

#include "TokenBucketLimiter.h"
#include <algorithm>   // for std::min
#include <iostream>    // for std::cout

// We use this to work with time easily
using Clock    = std::chrono::steady_clock;
using Duration = std::chrono::duration<double>;


// =============================================================
// CONSTRUCTOR
// Called when we create a TokenBucketLimiter object.
// Just saves the config settings.
// =============================================================
TokenBucketLimiter::TokenBucketLimiter(const RateLimitConfig& config)
    : config_(config)
{
    // Nothing else needed here.
    // Clients are registered lazily (when first request arrives).
}


// =============================================================
// REGISTER CLIENT (private helper)
//
// What it does:
//   Creates a fresh bucket for a user we have never seen before.
//   Called automatically on first request from that user.
//
// Example:
//   User "user_42" sends their very first request.
//   We create a full bucket (capacity tokens) for them.
// =============================================================
void TokenBucketLimiter::register_client(const std::string& client_id)
{
    // Only register if this user does not exist yet
    if (clients_.count(client_id) == 0) {

        TokenBucketState state;
        state.tokens      = config_.burst_capacity; // start with full bucket
        state.capacity    = config_.burst_capacity; // max tokens
        state.refill_rate = config_.refill_rate;    // tokens per second
        state.last_refill = Clock::now();            // current time

        clients_[client_id] = state;
    }
}


// =============================================================
// ALLOW REQUEST — The Main Function
//
// This is called every time a user sends a request.
// Returns: true  = ALLOW ✅
//          false = DENY  ❌
//
// What happens inside (step by step):
//   1. Register user if first time
//   2. Get current time
//   3. Calculate how much time passed since last refill
//   4. Add (time passed × refill_rate) tokens to bucket
//   5. Cap tokens at capacity (bucket cannot overflow)
//   6. Update last_refill to now
//   7. If tokens >= 1 → subtract 1 token → ALLOW
//   8. If tokens < 1  → DENY
// =============================================================
bool TokenBucketLimiter::allow_request(const std::string& client_id)
{
    // Step 1: Register if first time
    register_client(client_id);

    // Get this user's bucket state
    TokenBucketState& state = clients_[client_id];

    // Step 2: Get current time
    TimePoint now = Clock::now();

    // Step 3: Calculate how many SECONDS have passed
    // Duration<double> gives us seconds as a decimal number
    // e.g. 1.5 seconds, 0.3 seconds
    Duration elapsed = now - state.last_refill;
    double seconds_passed = elapsed.count();

    // Step 4: Add tokens based on time passed
    // Example: 2.5 seconds passed, refill_rate = 1.0
    //          new tokens = 2.5 × 1.0 = 2.5 tokens added
    state.tokens += seconds_passed * state.refill_rate;

    // Step 5: Cap tokens at maximum capacity
    // std::min returns the smaller of two values
    // So if tokens = 12 but capacity = 10, it becomes 10
    state.tokens = std::min(state.tokens, state.capacity);

    // Step 6: Update the refill timer to now
    state.last_refill = now;

    // Step 7 & 8: Check if enough tokens available
    if (state.tokens >= 1.0) {
        state.tokens -= 1.0;   // use one token
        return true;           // ALLOW ✅
    } else {
        return false;          // DENY ❌
    }
}


// =============================================================
// GET STATS
//
// Returns current stats for a user.
// Shows: total requests, allowed, denied, tokens remaining.
// =============================================================
RateLimitStats TokenBucketLimiter::get_stats(const std::string& client_id) const
{
    RateLimitStats stats;
    stats.client_id = client_id;

    // Check if this user exists
    if (clients_.count(client_id) == 0) {
        // User not found — return empty stats
        return stats;
    }

    const TokenBucketState& state = clients_.at(client_id);

    // We track allowed/denied in the stats map
    // (We will add that tracking in the manager later)
    // For now just return tokens remaining
    stats.total_requests = 0;  // will be tracked by manager
    stats.allowed        = 0;
    stats.denied         = 0;

    return stats;
}


// =============================================================
// RESET
//
// Clears all state for a user.
// Used in testing to start fresh.
// =============================================================
void TokenBucketLimiter::reset(const std::string& client_id)
{
    // Simply remove the user from our map
    // Next request from them will start fresh
    clients_.erase(client_id);
}