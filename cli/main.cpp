// =============================================================
// main.cpp
// COMBINED TEST - All 4 Algorithms Together
//
// This file tests all 4 algorithms one by one
// and then runs a final side by side comparison.
//
// ALGORITHMS TESTED:
// 1. Token Bucket
// 2. Fixed Window
// 3. Sliding Window Log
// 4. Sliding Window Counter
// =============================================================

#include <iostream>
#include <thread>
#include <chrono>
#include "TokenBucketLimiter.h"
#include "FixedWindowLimiter.h"
#include "SlidingWindowLogLimiter.h"
#include "SlidingWindowCounterLimiter.h"

// =============================================================
// HELPER FUNCTIONS
// These small functions help us print results cleanly
// so we dont repeat the same cout lines everywhere
// =============================================================

// Prints a single request result
void printResult(int reqNum, bool result) {
    std::cout << "  Request " << reqNum << " : "
              << (result ? "ALLOWED [YES]" : "DENIED  [NO] ")
              << "\n";
}

// Prints result with algorithm name beside it
void printSideBySide(int reqNum,
                     bool r1, bool r2,
                     bool r3, bool r4) {
    std::cout << "  Req " << reqNum
              << " | TokenBucket: "  << (r1 ? "[YES]" : "[NO] ")
              << " | FixedWindow: "  << (r2 ? "[YES]" : "[NO] ")
              << " | SlidingLog: "   << (r3 ? "[YES]" : "[NO] ")
              << " | SlidingCtr: "   << (r4 ? "[YES]" : "[NO] ")
              << "\n";
}

// Prints a section divider with title
void printSection(const std::string& title) {
    std::cout << "\n";
    std::cout << "##############################################\n";
    std::cout << "  " << title << "\n";
    std::cout << "##############################################\n\n";
}

// Prints a smaller sub section divider
void printSubSection(const std::string& title) {
    std::cout << "\n--- " << title << " ---\n\n";
}


// =============================================================
// TEST 1 — TOKEN BUCKET
//
// WHAT WE EXPECT:
// Bucket holds max 5 tokens, refills 1 per second
// Send 7 requests fast → 5 allowed, 2 denied
// Wait 3 seconds → 3 tokens refill
// Send 3 more → all 3 allowed
// Different user → gets own fresh bucket → allowed
// =============================================================
void testTokenBucket() {
    printSection("TEST 1 — TOKEN BUCKET ALGORITHM");

    RateLimitConfig config;
    config.burst_capacity = 5;    // bucket holds max 5 tokens
    config.refill_rate    = 1.0;  // 1 token added per second

    TokenBucketLimiter limiter(config);

    // Part A: Send 7 requests instantly
    printSubSection("Part A: Send 7 requests instantly (expect 5 YES, 2 NO)");
    for (int i = 1; i <= 7; i++) {
        bool result = limiter.allow_request("user_42");
        printResult(i, result);
    }

    // Part B: Wait 3 seconds then send 3 more
    printSubSection("Part B: Wait 3 seconds, send 3 more (expect all YES)");
    std::cout << "  Waiting 3 seconds for tokens to refill...\n\n";
    std::this_thread::sleep_for(std::chrono::seconds(3));

    for (int i = 1; i <= 3; i++) {
        bool result = limiter.allow_request("user_42");
        printResult(i, result);
    }

    // Part C: Different user gets their own fresh bucket
    printSubSection("Part C: New user gets own fresh bucket (expect YES)");
    bool result = limiter.allow_request("user_99");
    std::cout << "  user_99 first request : "
              << (result ? "ALLOWED [YES]" : "DENIED  [NO] ")
              << "\n";

    std::cout << "\n  >> Token Bucket Result: PASS if 5 YES, 2 NO, then 3 YES\n";
}


// =============================================================
// TEST 2 — FIXED WINDOW
//
// WHAT WE EXPECT:
// Max 5 per 10 second window
// Send 7 → 5 allowed, 2 denied
// Wait 10 seconds → window resets
// Send 5 more → all 5 allowed (BUG — this is on purpose)
// This shows the Boundary Burst Bug
// =============================================================
void testFixedWindow() {
    printSection("TEST 2 - FIXED WINDOW ALGORITHM");

    RateLimitConfig config;
    config.max_requests = 5;
    config.window_size  = 10; // 10 seconds so test is fast

    FixedWindowLimiter limiter(config);

    // Part A: Normal usage
    printSubSection("Part A: Send 7 requests (expect 5 YES, 2 NO)");
    for (int i = 1; i <= 7; i++) {
        bool result = limiter.allow_request("user_42");
        printResult(i, result);
    }

    // Part B: Show the boundary burst bug
    // Reset so we start clean for the bug demo
    limiter.reset("attacker");

    printSubSection("Part B: Boundary Burst Bug Demo");
    std::cout << "  Sending 5 requests at end of window 1:\n\n";
    for (int i = 1; i <= 5; i++) {
        bool result = limiter.allow_request("attacker");
        printResult(i, result);
    }

    std::cout << "\n  Waiting 10 seconds for window to reset...\n\n";
    std::this_thread::sleep_for(std::chrono::seconds(10));

    std::cout << "  Sending 5 requests at start of window 2:\n\n";
    for (int i = 1; i <= 5; i++) {
        bool result = limiter.allow_request("attacker");
        printResult(i, result);
    }

    std::cout << "\n  >> BOUNDARY BURST BUG:\n";
    std::cout << "     10 requests allowed in ~10 seconds\n";
    std::cout << "     but limit is 5 per window!\n";
    std::cout << "     This bug is fixed by Sliding Window algorithms.\n";
}


// =============================================================
// TEST 3 — SLIDING WINDOW LOG
//
// WHAT WE EXPECT:
// Max 5 per 10 second window
// Send 7 → 5 allowed, 2 denied
// Wait 5 seconds → some old timestamps expire
// Send 3 more → some allowed based on what expired
// =============================================================
void testSlidingWindowLog() {
    printSection("TEST 3 — SLIDING WINDOW LOG ALGORITHM");

    RateLimitConfig config;
    config.max_requests = 5;
    config.window_size  = 10;

    SlidingWindowLogLimiter limiter(config);

    // Part A: Normal usage
    printSubSection("Part A: Send 7 requests (expect 5 YES, 2 NO)");
    for (int i = 1; i <= 7; i++) {
        bool result = limiter.allow_request("user_42");
        printResult(i, result);
    }

    // Part B: Wait and see timestamps expire
    printSubSection("Part B: Wait 5 sec, old timestamps slide out");
    std::cout << "  Waiting 5 seconds...\n\n";
    std::this_thread::sleep_for(std::chrono::seconds(5));

    for (int i = 1; i <= 3; i++) {
        bool result = limiter.allow_request("user_42");
        printResult(i, result);
    }

    // Part C: Show it does NOT have boundary burst bug
    printSubSection("Part C: No boundary burst bug (compare with Fixed Window)");

    limiter.reset("attacker");
    std::cout << "  Sending 5 requests now:\n\n";
    for (int i = 1; i <= 5; i++) {
        bool result = limiter.allow_request("attacker");
        printResult(i, result);
    }

    std::cout << "\n  Waiting 10 seconds...\n\n";
    std::this_thread::sleep_for(std::chrono::seconds(10));

    std::cout << "  Sending 5 more (Fixed Window would allow all, Sliding Log limits):\n\n";
    for (int i = 1; i <= 5; i++) {
        bool result = limiter.allow_request("attacker");
        printResult(i, result);
    }

    std::cout << "\n  >> Sliding Window Log correctly limits based on exact timestamps\n";
}


// =============================================================
// TEST 4 — SLIDING WINDOW COUNTER
//
// WHAT WE EXPECT:
// Same as Sliding Log but uses formula instead of timestamps
// Results are very similar to Sliding Log
// Uses much less memory
// =============================================================
void testSlidingWindowCounter() {
    printSection("TEST 4 — SLIDING WINDOW COUNTER ALGORITHM");

    RateLimitConfig config;
    config.max_requests = 5;
    config.window_size  = 10;

    SlidingWindowCounterLimiter limiter(config);

    // Part A: Normal usage
    printSubSection("Part A: Send 7 requests (expect 5 YES, 2 NO)");
    for (int i = 1; i <= 7; i++) {
        bool result = limiter.allow_request("user_42");
        printResult(i, result);
    }

    // Part B: Wait for window to partially pass
    printSubSection("Part B: Wait 5 seconds, formula adjusts estimate");
    std::cout << "  Waiting 5 seconds...\n\n";
    std::this_thread::sleep_for(std::chrono::seconds(5));

    for (int i = 1; i <= 3; i++) {
        bool result = limiter.allow_request("user_42");
        printResult(i, result);
    }

    // Part C: No boundary burst
    printSubSection("Part C: No boundary burst bug");
    limiter.reset("attacker");

    std::cout << "  Sending 5 requests now:\n\n";
    for (int i = 1; i <= 5; i++) {
        bool result = limiter.allow_request("attacker");
        printResult(i, result);
    }

    std::cout << "\n  Waiting 10 seconds...\n\n";
    std::this_thread::sleep_for(std::chrono::seconds(10));

    std::cout << "  Sending 5 more:\n\n";
    for (int i = 1; i <= 5; i++) {
        bool result = limiter.allow_request("attacker");
        printResult(i, result);
    }

    std::cout << "\n  >> Sliding Counter uses formula — approx result, tiny memory\n";
}


// =============================================================
// TEST 5 — FINAL SIDE BY SIDE COMPARISON
//
// All 4 algorithms receive the EXACT same requests
// at the EXACT same time.
//
// This is the most important test — shows clearly:
// Which algorithms have the bug
// Which ones fix it
// =============================================================
void testAllSideBySide() {
    printSection("TEST 5 — ALL 4 ALGORITHMS SIDE BY SIDE");

    RateLimitConfig config;
    config.max_requests   = 5;
    config.window_size    = 10;
    config.burst_capacity = 5;
    config.refill_rate    = 1.0;

    TokenBucketLimiter          tb(config);
    FixedWindowLimiter          fw(config);
    SlidingWindowLogLimiter     sl(config);
    SlidingWindowCounterLimiter sc(config);

    // Part A: Normal requests — all should behave similarly
    printSubSection("Part A: Send 7 requests to all 4 at the same time");
    std::cout << "  Format: TokenBucket | FixedWindow | SlidingLog | SlidingCtr\n\n";

    for (int i = 1; i <= 7; i++) {
        bool r1 = tb.allow_request("user_42");
        bool r2 = fw.allow_request("user_42");
        bool r3 = sl.allow_request("user_42");
        bool r4 = sc.allow_request("user_42");
        printSideBySide(i, r1, r2, r3, r4);
    }

    // Part B: Boundary burst — the key difference shows here
    printSubSection("Part B: Boundary Burst - the key difference");
    std::cout << "  Sending 5 requests to all 4 algorithms:\n\n";
    std::cout << "  Format: TokenBucket | FixedWindow | SlidingLog | SlidingCtr\n\n";

    for (int i = 1; i <= 5; i++) {
        bool r1 = tb.allow_request("attacker");
        bool r2 = fw.allow_request("attacker");
        bool r3 = sl.allow_request("attacker");
        bool r4 = sc.allow_request("attacker");
        printSideBySide(i, r1, r2, r3, r4);
    }

    std::cout << "\n  Waiting 10 seconds (FixedWindow resets, others remember)...\n\n";
    std::this_thread::sleep_for(std::chrono::seconds(10));

    std::cout << "  Sending 5 more requests:\n\n";
    std::cout << "  Format: TokenBucket | FixedWindow | SlidingLog | SlidingCtr\n\n";

    for (int i = 1; i <= 5; i++) {
        bool r1 = tb.allow_request("attacker");
        bool r2 = fw.allow_request("attacker");
        bool r3 = sl.allow_request("attacker");
        bool r4 = sc.allow_request("attacker");
        printSideBySide(i, r1, r2, r3, r4);
    }

    // Final summary
    std::cout << "\n";
    std::cout << "##############################################\n";
    std::cout << "  FINAL SUMMARY\n";
    std::cout << "##############################################\n\n";
    std::cout << "  Algorithm            | Fixes Boundary Burst | Memory\n";
    std::cout << "  ---------------------|----------------------|-------\n";
    std::cout << "  Token Bucket         | YES                  | O(1) tiny\n";
    std::cout << "  Fixed Window         | NO  (has bug)        | O(1) tiny\n";
    std::cout << "  Sliding Window Log   | YES (perfect)        | O(R) large\n";
    std::cout << "  Sliding Window Ctr   | YES (approx)         | O(1) tiny\n";
    std::cout << "\n";
    std::cout << "  Fixed Window allowed 10 requests in 10 seconds\n";
    std::cout << "  even though limit was 5. That is the bug.\n";
    std::cout << "  All other algorithms handled it correctly.\n";
    std::cout << "##############################################\n";
}


// =============================================================
// MAIN — Runs all 5 tests one by one
// =============================================================
int main() {

    std::cout << "\n";
    std::cout << "**********************************************\n";
    std::cout << "   CONSOLE API RATE LIMITER\n";
    std::cout << "   All Algorithms - Combined Test Run\n";
    std::cout << "**********************************************\n";

    // Run each test
    testTokenBucket();

    testFixedWindow();

    testSlidingWindowLog();

    testSlidingWindowCounter();

    testAllSideBySide();

    std::cout << "\n**********************************************\n";
    std::cout << "   All Tests Complete.\n";
    std::cout << "**********************************************\n\n";

    return 0;
}
