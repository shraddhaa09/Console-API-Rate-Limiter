# Console API Rate Limiter

A console-based C++ application that simulates an in-memory API rate limiting system.
Supports 4 rate limiting algorithms with multi-client simulation and real-time allow/deny decisions.

---

## What This Project Does

API Rate Limiting controls how many requests a user can make in a given time window.
This project simulates that system in the terminal — you can send requests, switch algorithms,
and see which requests are allowed or denied in real time.

---

## Algorithms Implemented

| Algorithm | How It Works | Fixes Boundary Burst |
|-----------|-------------|----------------------|
| Token Bucket | Bucket of tokens refills over time. 1 token = 1 request. | Yes |
| Fixed Window | Counts requests per fixed time window. Resets at boundary. | No |
| Sliding Window Log | Stores exact timestamp of every request in a deque. | Yes |
| Sliding Window Counter | Uses weighted formula with 2 counters to estimate count. | Yes |

---
