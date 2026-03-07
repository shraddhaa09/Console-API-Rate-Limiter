// =============================================================
// FixedWindowLimiter.cpp
// Assigned to: Member B
//
// WHAT THIS DOES:
// Counts how many requests a user sends in a 60 second window.
// When 60 seconds is up — counter resets to zero.
// If count goes over max_requests — DENY.
//
// THE BUG THIS HAS (on purpose):
// User sends 10 at second 59 → ALLOWED (window 1 ends)
// User sends 10 at second 61 → ALLOWED (window 2 starts fresh)
// Result: 20 requests in 2 seconds. System was fooled.
// This is the Boundary Burst Problem.
// =============================================================

#include "FixedWindowLimiter.h"
#include <iostream>

// Shortcut for the clock we use
using Clock = std::chrono::steady_clock;


// =============================================================
// CONSTRUCTOR
// Just saves the config when object is created
// =============================================================
FixedWindowLimiter::FixedWindowLimiter(const RateLimitConfig& config)
    : config_(config)
{
    // Nothing else needed here
}


// =============================================================
// REGISTER CLIENT
//
// WHAT: Creates a fresh counter for a new user
// WHY:  First time we see a user, we need to set up their data
// WHEN: Called automatically on their first request
// =============================================================
void FixedWindowLimiter::register_client(const std::string& client_id)
{
    if (clients_.count(client_id) == 0) {

        FixedWindowState state;
        state.count        = 0;            // no requests yet
        state.window_start = Clock::now(); // window starts now

        clients_[client_id] = state;
    }
}


// =============================================================
// ALLOW REQUEST — Main Logic
//
// STEP BY STEP:
// 1. Register user if first time
// 2. Get current time
// 3. Check if 60 seconds have passed since window started
// 4. If yes → reset counter to 0, start new window
// 5. Add 1 to counter
// 6. If counter <= max → ALLOW
// 7. If counter > max  → DENY
// =============================================================
bool FixedWindowLimiter::allow_request(const std::string& client_id)
{
    // Step 1: Register if new user
    register_client(client_id);

    // Get this user's data
    FixedWindowState& state = clients_[client_id];

    // Step 2: Get current time
    auto now = Clock::now();

    // Step 3: Calculate how many seconds since window started
    // duration_cast converts time to seconds (whole number)
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>
                   (now - state.window_start).count();

    // Step 4: If window has expired → reset everything
    // config_.window_size is 60 seconds by default
    if (elapsed >= config_.window_size) {
        state.count        = 0;    // reset counter
        state.window_start = now;  // new window starts now
    }

    // Step 5: Count this request
    state.count++;

    // Step 6 & 7: Allow or deny
    if (state.count <= config_.max_requests) {
        return true;   // ALLOW ✅
    } else {
        return false;  // DENY ❌
    }
}


// =============================================================
// GET STATS
// Returns current info about a user
// =============================================================
RateLimitStats FixedWindowLimiter::get_stats(const std::string& client_id) const
{
    RateLimitStats stats;
    stats.client_id = client_id;
    return stats;
}


// =============================================================
// RESET
// Clears a user's data completely
// Used in testing to start fresh
// =============================================================
void FixedWindowLimiter::reset(const std::string& client_id)
{
    clients_.erase(client_id);
}