#pragma once

// =============================================================
// IRateLimiter.h
// This is the Abstract Base Class (interface).
//
// What does "abstract" mean?
// → This class defines WHAT functions must exist
// → But does NOT write HOW they work
// → Each algorithm (TokenBucket, FixedWindow etc.)
//   writes their own version of these functions
//
// Think of it like a job description:
// "You must be able to allow_request() and reset()"
// Each algorithm does it differently.
// =============================================================

#include <string>
#include "RateLimitConfig.h"

class IRateLimiter {
public:

    // ── The main function ─────────────────────────────────────
    // Call this every time a user sends a request.
    // Returns: true  = ALLOW  ✅
    //          false = DENY   ❌
    virtual bool allow_request(const std::string& client_id) = 0;

    // ── Get stats for a user ──────────────────────────────────
    // Returns how many requests were allowed/denied for this user
    virtual RateLimitStats get_stats(const std::string& client_id) const = 0;

    // ── Reset a user ──────────────────────────────────────────
    // Clears all state for this user (used in testing)
    virtual void reset(const std::string& client_id) = 0;

    // ── Virtual destructor ────────────────────────────────────
    // Always needed when using inheritance in C++
    // Without this, memory can leak when deleting child objects
    virtual ~IRateLimiter() = default;
};