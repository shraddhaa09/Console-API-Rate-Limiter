// =============================================================
// SlidingWindowCounterLimiter.cpp
// Assigned to: Member D
//
// WHAT THIS DOES:
// Uses 2 counters and a formula to ESTIMATE how many requests
// happened in the last 60 seconds.
//
// WHY ESTIMATE AND NOT EXACT?
// Exact = store every timestamp = lots of memory (Sliding Log)
// Estimate = just 3 variables per user = tiny memory
// The estimate is within 5% of exact — good enough.
//
// THE FORMULA:
// position  = how far into current window we are (0.0 to 1.0)
// estimated = prev_count x (1.0 - position) + curr_count
//
// EXAMPLE:
// prev_count = 8, curr_count = 3, position = 0.25 (25% through)
// estimated = 8 x (1 - 0.25) + 3
//           = 8 x 0.75 + 3
//           = 6 + 3 = 9
// If max = 10: 9 < 10 so ALLOW
//
// FIXES BOUNDARY BURST? YES
// The weighted formula smooths the transition between windows.
// No sharp reset = no burst opportunity.
// =============================================================

#include "SlidingWindowCounterLimiter.h"
#include <iostream>

using Clock = std::chrono::steady_clock;


// =============================================================
// CONSTRUCTOR
// =============================================================
SlidingWindowCounterLimiter::SlidingWindowCounterLimiter(
    const RateLimitConfig& config)
    : config_(config)
{
}


// =============================================================
// REGISTER CLIENT
// Creates fresh counters for a new user
// =============================================================
void SlidingWindowCounterLimiter::register_client(
    const std::string& client_id)
{
    if (clients_.count(client_id) == 0) {

        SlidingCounterState state;
        state.prev_count   = 0;            // no previous requests
        state.curr_count   = 0;            // no current requests
        state.window_start = Clock::now(); // window starts now

        clients_[client_id] = state;
    }
}


// =============================================================
// ALLOW REQUEST — Main Logic
//
// STEP BY STEP:
// 1. Register user if new
// 2. Get current time
// 3. Check if current window has expired
//    If yes: roll over (prev = curr, curr = 0, reset timer)
// 4. Calculate position (how far into current window)
// 5. Calculate estimated count using formula
// 6. If estimated < max_requests: count++ and ALLOW
// 7. Else: DENY
// =============================================================
bool SlidingWindowCounterLimiter::allow_request(
    const std::string& client_id)
{
    // Step 1
    register_client(client_id);

    SlidingCounterState& state = clients_[client_id];

    // Step 2: Current time
    auto now = Clock::now();

    // Step 3: Check if window expired
    // Convert window_size (int seconds) to a duration for comparison
    auto window_dur = std::chrono::seconds(config_.window_size);
    auto elapsed    = now - state.window_start;

    if (elapsed >= window_dur) {
        // Window expired — roll over
        // Previous window becomes the old current window
        state.prev_count   = state.curr_count; // old current → previous
        state.curr_count   = 0;                // reset current
        state.window_start = now;              // new window starts
    }

    // Step 4: Calculate position
    // How far are we through the current window? (0.0 = start, 1.0 = end)
    // elapsed.count() gives nanoseconds so we convert to seconds
    double elapsed_sec = std::chrono::duration<double>(
                         now - state.window_start).count();
    double position    = elapsed_sec / config_.window_size;

    // Make sure position stays between 0.0 and 1.0
    if (position > 1.0) position = 1.0;
    if (position < 0.0) position = 0.0;

    // Step 5: Calculate estimated requests in sliding window
    // This formula weighs the previous window by how much of it
    // still overlaps with our current window
    double estimated = (state.prev_count * (1.0 - position))
                       + state.curr_count;

    // Step 6 & 7: Allow or deny
    if (estimated < config_.max_requests) {
        state.curr_count++; // count this request
        return true;        // ALLOW ✅
    } else {
        return false;       // DENY ❌
    }
}


// =============================================================
// GET STATS
// =============================================================
RateLimitStats SlidingWindowCounterLimiter::get_stats(
    const std::string& client_id) const
{
    RateLimitStats stats;
    stats.client_id = client_id;
    return stats;
}


// =============================================================
// RESET
// =============================================================
void SlidingWindowCounterLimiter::reset(const std::string& client_id)
{
    clients_.erase(client_id);
}