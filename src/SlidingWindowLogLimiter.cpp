// =============================================================
// SlidingWindowLogLimiter.cpp
// Assigned to: Member C
//
// WHAT THIS DOES:
// Stores the exact timestamp of every request in a deque.
// When checking, removes all timestamps older than window_size.
// Counts remaining timestamps — if under limit, ALLOW.
//
// WHY A DEQUE?
// deque = double ended queue
// We add new timestamps to the BACK  → push_back()  O(1)
// We remove old timestamps from FRONT → pop_front() O(1)
// If we used vector, removing from front shifts
// all elements = O(n) which is slow.
//
// FIXES BOUNDARY BURST? YES — perfectly.
// Because we track exact times, there is no reset boundary.
// =============================================================

#include "SlidingWindowLogLimiter.h"
#include <iostream>

using Clock = std::chrono::steady_clock;


// =============================================================
// CONSTRUCTOR
// =============================================================
SlidingWindowLogLimiter::SlidingWindowLogLimiter(const RateLimitConfig& config)
    : config_(config)
{
}


// =============================================================
// REGISTER CLIENT
// Creates an empty timestamp list for a new user
// =============================================================
void SlidingWindowLogLimiter::register_client(const std::string& client_id)
{
    if (clients_.count(client_id) == 0) {
        SlidingLogState state;
        // timestamps deque starts empty — no requests yet
        clients_[client_id] = state;
    }
}


// =============================================================
// ALLOW REQUEST — Main Logic
//
// STEP BY STEP:
// 1. Register user if new
// 2. Get current time
// 3. Calculate cutoff time = now - window_size
//    (anything older than this is outside our window)
// 4. Remove all timestamps from FRONT of deque
//    that are older than cutoff
// 5. Count remaining timestamps in deque
// 6. If count < max_requests → add current time → ALLOW
// 7. If count >= max_requests → DENY
//
// EXAMPLE:
// Window = 60 seconds, max = 5
// Deque has: [t=10, t=20, t=30, t=40, t=50]
// New request at t=75
// Cutoff = 75 - 60 = 15
// Remove t=10 (older than 15)
// Deque now: [t=20, t=30, t=40, t=50]  size=4
// 4 < 5 so ALLOW, add t=75
// Deque: [t=20, t=30, t=40, t=50, t=75]
// =============================================================
bool SlidingWindowLogLimiter::allow_request(const std::string& client_id)
{
    // Step 1
    register_client(client_id);

    SlidingLogState& state = clients_[client_id];

    // Step 2: Current time
    auto now = Clock::now();

    // Step 3: Calculate the cutoff point
    // Any timestamp BEFORE this is too old — outside our window
    auto window_duration = std::chrono::seconds(config_.window_size);
    auto cutoff          = now - window_duration;

    // Step 4: Remove expired timestamps from the FRONT
    // We keep removing from front as long as front is too old
    // This is why deque is perfect — pop_front is O(1)
    while (!state.timestamps.empty() &&
           state.timestamps.front() < cutoff)
    {
        state.timestamps.pop_front(); // remove oldest entry
    }

    // Step 5: Count what remains
    int current_count = static_cast<int>(state.timestamps.size());

    // Step 6 & 7: Allow or deny
    if (current_count < config_.max_requests) {
        state.timestamps.push_back(now); // record this request
        return true;   // ALLOW ✅
    } else {
        return false;  // DENY ❌
    }
}


// =============================================================
// GET STATS
// =============================================================
RateLimitStats SlidingWindowLogLimiter::get_stats(const std::string& client_id) const
{
    RateLimitStats stats;
    stats.client_id = client_id;
    return stats;
}


// =============================================================
// RESET
// Clears the entire timestamp list for a user
// =============================================================
void SlidingWindowLogLimiter::reset(const std::string& client_id)
{
    clients_.erase(client_id);
}