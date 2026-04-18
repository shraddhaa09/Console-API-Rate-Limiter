# Console-API-Rate-Limiter # SentinelDS – A High‑Performance Traffic Control Engine

SentinelDS is a **C++ traffic control engine** that simulates an API gateway for a high‑traffic system (like Swiggy or Amazon).  
Instead of using standard library containers, it is built from scratch using **seven core data structures** to manage path validation, IP banning, rate limiting, logging, and analytics.

---

## 🧩 Technical Overview

- **Language:** C++ (no external libraries).  
- **Key data structures:**  
  - Trie & Skip List  
  - Red‑Black Tree & B+ Tree  
  - Binomial Heap, DSU, Segment Tree  

This project was developed as a **Semester‑4 Advanced Data Structures** assignment to demonstrate how each data structure solves one specific sub‑problem in a real‑world traffic control system.

---

## 🚀 Features

- Path‑based rate limiting using a **Trie** for fast, prefix‑aware checks.  
- IP‑level banning using **Disjoint‑Set Union (DSU)** for botnet‑like grouping.  
- User‑level traffic control via **Red‑Black Tree** and **Skip List** cache.  
- Penalty queue on a **Binomial Heap** for multi‑server‑friendly merge semantics.  
- Persistent logs stored in a **B+ Tree** for efficient range queries.  
- Real‑time analytics (e.g., “requests per second”) using a **Segment Tree**.

---

## 📌 How to Build

```bash
git clone https://github.com/<your-username>/Console-API-Rate-Limiter.git
cd Console-API-Rate-Limiter
g++ -std=c++17 main.cpp -o sentinelds
./sentinelds
```

The program reads simulated requests from `requests.txt` and outputs a trace like:

```text
[Trie]   Path validated: /api/v1/login (limit: 5 req/min)
[Verdict] ALLOWED — count: 2/5
[B+Tree] Request logged at timestamp 1700000005
```

## 📄 License & Attribution

This project is for **educational use**.  
If you reuse or adapt it, please **credit the original author** and the project team.

---

_SentinelDS – a console‑based API‑gateway–style traffic controller._