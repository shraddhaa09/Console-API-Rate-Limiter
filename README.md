# Console-API-Rate-Limiter
## API_rate_limiter – A High-Performance Traffic Control Engine

**API_rate_limiter** is a C++ traffic control engine that simulates the core behavior of an API gateway for a high-traffic system like Swiggy, Amazon, or any large-scale web platform.

Instead of relying on standard library containers, this project is built from scratch using **seven core data structures** to manage path validation, IP banning, user lookup, rate limiting, logging, and analytics.

---

## Overview

This project was developed as a **Semester 4 Advanced Data Structures** assignment to demonstrate how different data structures can solve specific sub-problems inside one real-world traffic control pipeline.

Each request passes through a fixed lifecycle:

1. **Entry Checks**  
   Validate the path and check the IP reputation.

2. **Identification**  
   Search the user in cache and master registry.

3. **Verdict & Penalty**  
   Decide whether the request is allowed or blocked and manage penalties.

4. **Logging & Analytics**  
   Store logs and update real-time statistics.

---

## Features

- **Path-based request validation** using a **Trie**.
- **IP reputation and banned-group detection** using **DSU (Disjoint Set Union)**.
- **Fast cache lookup** using a **Skip List**.
- **Master user registry** using a **Red-Black Tree**.
- **Penalty queue management** using a **Binomial Heap**.
- **Persistent log storage** using a **B+ Tree**.
- **Real-time request analytics** using a **Segment Tree**.
- **Colored console output** for a more structured and readable audit trace.
- **Lifecycle-based request processing** for easy demonstration during review or viva.

---

## Project Goal

The goal of this project is to simulate how an API gateway handles incoming traffic in a structured and efficient way.

A request should:
- be validated quickly,
- identify the user efficiently,
- be rate-limited fairly,
- be logged properly,
- and contribute to live analytics.

This makes the project a good example of how multiple data structures can be integrated into one practical system.

---

## Data Structures Used

| Data Structure | Purpose |
|---|---|
| Trie | Validates API paths and supports prefix-based route lookup |
| DSU | Groups banned IPs and handles reputation-based blocking |
| Skip List | Works as a fast L1 cache for recent users |
| Red-Black Tree | Stores the master registry of users and request counts |
| Binomial Heap | Manages penalty users by unlock time |
| B+ Tree | Stores logs for persistent and ordered access |
| Segment Tree | Tracks real-time request counts and range analytics |

---

## Workflow

### Phase 1 – Entry Checks
- The system checks whether the request path is valid.
- It checks whether the IP belongs to a banned group.
- If validation fails, the request is dropped early.

### Phase 2 – Identification
- The system first checks the L1 cache.
- If not found, it searches the master registry.
- If the user is new, the system inserts them.

### Phase 3 – Verdict & Penalty
- The request count is compared with the allowed limit.
- If the limit is exceeded, the user is placed into the penalty heap.
- If not, the request is allowed.

### Phase 4 – Logging & Analytics
- The request is stored in the B+ Tree log system.
- The Segment Tree updates live analytics.
- The system can later query request trends over time ranges.

---

## Team Members

This project was developed by four members, each owning a specific part of the lifecycle:

- **Shraddha** – Binomial Heap and main lifecycle flow in `main.cpp`
- **Nandita** – Trie and Skip List
- **Sharwil** – Red-Black Tree and B+ Tree
- **Roshani** – DSU and Segment Tree

---

## Console Output

The project prints a step-by-step console audit for each request, such as:

- request details,
- validation status,
- route match result,
- rate-limit decision,
- final verdict,
- and monitoring updates.

At the end, it also shows a final summary containing:
- total requests processed,
- total allowed requests,
- total blocked requests.

---

## Folder Structure

```bash
Console-API-Rate-Limiter/
│
├── cli/
│   └── main.cpp
├── include/
│   ├── Common.h
│   ├── Trie.h
│   ├── DSU.h
│   ├── SkipList.h
│   ├── RBTree.h
│   ├── BinomialHeap.h
│   ├── BPlusTree.h
│   └── SegmentTree.h
├── requests.txt
├── CMakeLists.txt
└── README.md
```

---

## How to Build

### Using CMake and MinGW

```bash
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
./api_rate_limiter.exe
```

### If using a different compiler

```bash
g++ -std=c++17 cli/main.cpp -o api_rate_limiter
./api_rate_limiter
```

---

## Input Format

The program reads requests from `requests.txt`.

Each line should follow this format:

```text
<userID> <path> <ip> <timestamp>
```

### Example

```text
101 /api/v1/login 192.168.1.5 1700000001
102 /api/v1/register 192.168.2.10 1700000002
103 /api/admin/config 10.0.0.1 1700000003
```

---

## Sample Output

```text
[API_rate_limiter] REQUEST #01
[REQ] /api/v1/login | IP: 192.168.1.5 | UID: 101
[L1: VALIDATION] PASS
[L2: ROUTE MATCH] PASS | Trie: HIT | Route: /api/v1/login
[L3: LIMIT CHECK] PASS | Requests: 2/5 | Cache: HIT
[L4: MONITORING] RECORDED | Stats Updated | Last 5s Load: 3
FINAL VERDICT: ALLOWED
```

---

## Why This Project is Useful

This project shows how a real traffic control system can be broken into smaller responsibilities and handled by different data structures.

It also demonstrates:
- modular design,
- runtime efficiency,
- request lifecycle handling,
- and practical integration of advanced data structures.

---

## Future Improvements

Possible future enhancements:
- add multi-threaded request handling,
- export logs to files or JSON,
- add a GUI dashboard,
- support multiple rate-limiting strategies,
- improve analytics visualization,
- add unit tests for each data structure.

---

## License

This project is for educational use.

If you reuse or modify it, please credit the original authors and the project team.

---

## Team Credit

Project by:
- [Nandita](https://github.com/kharade-nandita)
- [Sharwil](https://github.com/SharwillKhisti)
- [Roshani](https://github.com/rosh1149)
- [Shraddha](https://github.com/shraddhaa09)
