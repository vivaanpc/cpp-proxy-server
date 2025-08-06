# **C++ Proxy Server — PRD**

# 

# **1\. Title & Author Info**

Title: Lightweight Forward & Reverse Proxy Server in Modern C++

Author: Vivaan Chheda, Priyanshu Singh.

# **2\. Problem Statement**

Existing proxy servers are often too complex, resource-heavy, or tightly coupled to specific ecosystems. There’s a growing need for a lightweight, easily configurable proxy server that supports both forward and reverse proxy modes for development, testing, and internal routing—without unnecessary overhead or third-party bloat. This project aims to deliver a fast, modern C++ proxy server with a small footprint and modular design, suitable for a range of network-layer use cases.

# **3\. Goals / Objectives**

\- Support HTTP/1.1 forward proxy with request forwarding and HTTPS tunneling

\- Enable basic reverse proxy features:

  • Static routing to predefined upstreams  
   • Host-based routing and simple load balancing  
   • Header injection for client tracing

\- Support multiple concurrent connections using asynchronous I/O

\- Provide a JSON-configurable runtime setup

\- Enable clean logging and simple observability

# **4\. Scope**

In-Scope:

\- Forward proxy with HTTP/HTTPS support  
 \- Reverse proxy with:  
   • Static and host-based routing  
   • Round-robin backend selection  
   • Header injection (X-Forwarded-For)  
 \- JSON-based configuration  
 \- Asynchronous I/O support via `select()` (baseline) and Boost.Asio (modular extension for scalable concurrency).  
 \- Structured logging

Out-of-Scope:

\- TLS termination or decryption  
 \- HTTP/2 or QUIC support  
 \- Persistent caching or disk I/O  
 \- Auth, access control, or web dashboard

# **5\. User Stories / Use Cases**

\- As a developer, I want to route traffic through a local proxy to inspect and debug requests.

\- As a DevOps engineer, I want to use a reverse proxy for internal service routing without relying on large proxy suites.

\- As a tester, I want to simulate multi-backend routing and load balancing for integration testing.

\- As a platform engineer, I want a fast, embeddable proxy service that can be deployed with minimal dependencies.

# **6\. Wireframes / UX Inputs** 

Not applicable \- this is a backend network application without GUI. Configuration and logs will be handled via text files and command line.

\[ Client \] → \[ Proxy Server (Router, Logger, Conn Pool) \] → \[ Upstream Server \]

**7\. Timeline** 

| Week | Deliverables |
| :---- | :---- |
| Week 1 | TCP socket-based forward proxy with HTTP/HTTPS support |
| Week 2 | HTTP parser, concurrency using select()/Boost.Asio |
| Week 3 | Reverse proxy mode: static & host-based routing, header injection |
| Week 4 | Round-robin load balancing, config-driven behavior, structured logging, testing |

 

