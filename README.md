🔐 C++ HTTPS Server

A lightweight HTTPS server built from scratch in modern C++20 — from TCP sockets and TLS to HTTP/1.1 parsing, routing, and concurrent request handling.

This project is an exploration of what actually happens under the hood when you open an HTTPS connection.

Instead of relying on a high-level web framework, the server builds the networking stack layer by layer:

TCP → TLS → HTTP/1.1 → Routing → Request Handling → Response

🚧 Status: Work in Progress

🚀 What Makes This Project Interesting?

Most web servers hide networking behind a framework.

This project does the opposite.

The goal is to understand the journey of a request from the moment a client connects to the server until an HTTP response is returned.

For example:

Browser
   │
   │ HTTPS connection
   ▼
┌─────────────────┐
│   TCP Socket    │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  TLS / OpenSSL  │
└────────┬────────┘
         │ decrypted bytes
         ▼
┌─────────────────┐
│ HTTP/1.1 Parser │
└────────┬────────┘
         │ HTTP Request
         ▼
┌─────────────────┐
│     Router      │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ Request Handler │
└────────┬────────┘
         │ HTTP Response
         ▼
       Client


The project is intentionally modular so each layer can be developed, tested, and improved independently.

✨ Features
🌐 Networking
C++20 implementation
TCP socket abstraction
Concurrent connection handling
Connection limits
Connection timeouts
Graceful shutdown
🔒 TLS / HTTPS
OpenSSL integration
TLS 1.2+
Configurable certificates
Configurable private keys
Secure TLS resource management
📡 HTTP/1.1
HTTP request parsing
HTTP request/response abstractions
HTTP response serialization
Persistent connections / Keep-Alive
Request size limits
Request timeouts
🧭 Routing
HTTP method + path based routing
Parameterized routes
Independent request handlers
Routes can be added without touching the networking layer
⚙️ Server Infrastructure
Thread-pool based concurrency
Configuration system
Structured logging
CMake build system
Unit tests
Integration tests
Performance benchmarking
AddressSanitizer support
UndefinedBehaviorSanitizer support
🏗️ Architecture

The server follows a layered architecture where each component has a focused responsibility.

🌍 Client / BrowserTCP ListenerTCP ConnectionTLS LayerOpenSSLHTTP/1.1 ParserHTTP RequestRouterRequest HandlerHTTP ResponseHTTP Serializer
Why layers?

The idea is simple:

Networking shouldn't need to know about HTTP, and HTTP shouldn't need to know how TCP works.

This separation makes the code easier to reason about and allows individual components to be tested independently.

🔄 Request Lifecycle

Here's what happens when a client requests:

GET /health HTTP/1.1
Host: localhost:8443

1. TCP Connection

The client establishes a TCP connection with the server.

Client
  │
  │ SYN
  ▼
Server
  │
  │ SYN-ACK
  ▼
Client
  │
  │ ACK
  ▼
Connected

2. TLS Handshake

Because this is HTTPS, the connection is upgraded into a secure TLS session.

Client                         Server
  │                              │
  │──── ClientHello ────────────►│
  │                              │
  │◄─── ServerHello ─────────────│
  │◄─── Certificate ─────────────│
  │                              │
  │──── Key Exchange ───────────►│
  │                              │
  │◄════ Encrypted Channel ═════►│


OpenSSL handles the cryptographic protocol while the server manages the TLS connection lifecycle.

3. HTTP Parsing

Once the TLS layer provides decrypted bytes, the HTTP parser turns them into a structured request.

Raw bytes
   │
   ▼
"GET /health HTTP/1.1\r\n..."
   │
   ▼
┌─────────────────────┐
│ HTTP Request Object │
├─────────────────────┤
│ Method: GET         │
│ Path: /health       │
│ Version: HTTP/1.1   │
│ Headers: ...        │
└─────────────────────┘

4. Routing

The router determines which handler should process the request.

GET /health
     │
     ▼
   Router
     │
     ├── GET /
     ├── GET /health ──────► Health Handler
     ├── GET /api/users
     └── POST /api/users

5. Response

The handler generates an HTTP response.

HTTP/1.1 200 OK
Content-Type: text/plain
Content-Length: 2

OK


The response travels back through the HTTP, TLS, and TCP layers before reaching the client.

🧵 Concurrency Model

The server uses a thread pool to handle multiple connections concurrently.

Instead of creating a new thread for every connection:

Connection 1 ──┐
Connection 2 ──┤
Connection 3 ──┼──► Thread Pool
Connection 4 ──┤
Connection 5 ──┘


A simplified model looks like:

TCP ListenerAccepted ConnectionsConnection QueueWorker 1Worker 2Worker 3Worker 4HTTP/TLS ProcessingHTTP/TLS ProcessingHTTP/TLS ProcessingHTTP/TLS Processing

This allows the server to process multiple clients without continuously creating and destroying operating-system threads.

📁 Project Structure
cpp-https-server/
│
├── CMakeLists.txt
├── README.md
├── LICENSE
├── .gitignore
│
├── cmake/
│   └── FindOpenSSL.cmake
│
├── config/
│   ├── server.conf
│   └── tls.conf
│
├── certs/
│   ├── .gitkeep
│   └── README.md
│
├── include/
│   ├── server/
│   ├── net/
│   ├── tls/
│   ├── http/
│   ├── thread/
│   └── utils/
│
├── src/
│   ├── main.cpp
│   ├── server/
│   ├── net/
│   ├── tls/
│   ├── http/
│   ├── thread/
│   └── utils/
│
├── tests/
├── examples/
├── benchmarks/
├── scripts/
└── docs/


The directory structure mirrors the architecture, keeping networking, TLS, HTTP, threading, and utility code separated.

🛠️ Requirements

Currently developed and tested primarily on Linux.

Recommended environment:

Linux
CMake 3.20+
GCC 11+ or Clang 14+
OpenSSL 3.x
pthreads
Git
Install dependencies

Debian / Ubuntu:

sudo apt update

sudo apt install \
    build-essential \
    cmake \
    git \
    libssl-dev

🔨 Building

Clone the repository:

git clone <repository-url>
cd cpp-https-server


Configure:

cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release


Build:

cmake --build build -j


The resulting executable:

build/cpp_https_server

🐛 Debug Build

For development:

cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Debug

cmake --build build -j

🧪 Sanitizers

The project supports both AddressSanitizer and UndefinedBehaviorSanitizer.

cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Debug \
    -DENABLE_SANITIZERS=ON

cmake --build build -j


This helps catch issues such as:

Memory leaks
Use-after-free
Buffer overflows
Invalid memory access
Undefined behavior
🔐 TLS Certificates

The server expects a TLS certificate and corresponding private key.

For local development:

./scripts/generate_certs.sh


Expected files:

certs/
├── server.crt
└── server.key


⚠️ Never commit private keys to Git.

For production deployments, certificates should be issued by a trusted Certificate Authority.

▶️ Running the Server

Start the server from the project root:

./build/cpp_https_server


Current development configuration:

Host:        0.0.0.0
Port:        8443
Protocol:    HTTPS
TLS:         1.2+
Certificate: certs/server.crt
Private key: certs/server.key


Once running, visit:

https://127.0.0.1:8443/


The development server provides a simple form that sends text to:

POST /print


Health check:

https://127.0.0.1:8443/health

🩺 Health Check

You can test the server with:

curl -k https://localhost:8443/health


Example:

OK


The -k flag is appropriate when using a self-signed development certificate.

For Windows curl, local certificate revocation lookup may need to be disabled:

curl.exe --ssl-no-revoke https://127.0.0.1:8443/health

📡 HTTP API

Example routes:

Method	Path	Description
GET	/	Server information
GET	/health	Health check
GET	/api/users	Retrieve users
POST	/api/users	Create a user
DELETE	/api/users/:id	Delete a user

The routing layer is designed so new handlers can be registered without modifying the underlying TCP or TLS implementation.

📦 Example Request
GET / HTTP/1.1
Host: localhost:8443
Connection: close


Example response:

HTTP/1.1 200 OK
Content-Type: text/plain
Content-Length: 12
Connection: close

Hello World!

⚙️ Configuration

Server configuration is separated from application code.

Example:

[server]
host = 0.0.0.0
port = 8443
workers = 4

[http]
max_request_size = 10485760
request_timeout = 30
keep_alive_timeout = 5

[tls]
certificate = certs/server.crt
private_key = certs/server.key


The exact available options depend on the current implementation.

🧪 Testing

Configure with tests enabled:

cmake -S . -B build \
    -DBUILD_TESTS=ON


Build:

cmake --build build -j


Run:

ctest --test-dir build --output-on-failure


The test suite is intended to cover both individual components and complete request/response flows.

⚡ Benchmarking

Performance benchmarking can be enabled with:

cmake -S . -B build \
    -DBUILD_BENCHMARKS=ON

cmake --build build -j


Current benchmarking areas include:

Requests per second
Connection establishment latency
HTTP parsing throughput
Concurrent connection handling
TLS handshake overhead

Performance work is ongoing.

🎯 Design Goals

The project is built around several principles.

Separation of Concerns

TCP, TLS, HTTP parsing, routing, threading, and application logic are separate components.

Resource Safety

C++ RAII is used to manage sockets, TLS resources, threads, and other system resources.

Explicit Failure Handling

Network and protocol errors are handled explicitly rather than allowing failures to unexpectedly terminate the server.

Configurability

Operational parameters should be configurable without recompiling the application.

Testability

Core components such as HTTP parsing and routing should be testable without requiring a live network connection.

Performance

The server is designed to support concurrent clients while avoiding unnecessary allocations and system calls where practical.

🔒 Security Considerations

This is primarily an educational and portfolio systems-programming project, not a production-ready Internet-facing web server.

Before deploying publicly, additional hardening is required.

Important considerations include:

Use a trusted CA-issued certificate
Protect the TLS private key
Disable obsolete TLS versions
Configure appropriate cipher suites
Enforce request-size limits
Enforce connection timeouts
Validate all client-controlled input
Handle malformed HTTP requests safely
Implement rate limiting
Avoid leaking internal errors
Run with minimum required OS privileges
Keep OpenSSL and dependencies updated

Do not expose the development configuration directly to the public Internet.

🗺️ Roadmap
✅ Completed
 TCP listener
 HTTP protocol design
 HTTP/1.1 parser
 HTTP response serializer
 TLS context
 HTTPS connections
 Request router
 Thread pool
 Keep-Alive connections
 Configuration system
 Structured logging
 Graceful shutdown
 Unit tests
 Integration tests
 Benchmark infrastructure
🚧 Planned
 Linux epoll event loop
 HTTP/2 support
 More extensive protocol compliance testing
 Production hardening
 More detailed performance profiling
 Improved observability
 Expanded configuration system
🧠 What I'm Exploring

This project is more than just building an HTTP server.

It is an experiment in understanding systems programming from the bottom up.

While developing it, the main areas of exploration are:

C++20
  │
  ├── RAII & Resource Management
  ├── Concurrency & Thread Pools
  ├── Socket Programming
  ├── TCP/IP
  ├── TLS
  ├── OpenSSL
  ├── HTTP/1.1
  ├── Protocol Parsing
  ├── Systems Architecture
  └── Performance Engineering


The goal is not to compete with mature production web servers.

The goal is to understand how they work.

📈 Future Architecture

One of the planned improvements is moving beyond the thread-oriented connection model toward an event-driven architecture.

ClientsepollEvent LoopTCPTLSHTTP ParserRouterApplication

This would allow the project to explore a different concurrency model and provide an interesting comparison between thread-per-connection / thread-pool architectures and event-driven networking.

🤝 Contributing

This project is primarily a personal systems-programming project, but ideas, bug reports, and improvements are welcome.

If you find something interesting or spot a problem, feel free to open an issue or pull request.

📜 License

This project is distributed under the MIT License.

See LICENSE for details.

👨‍💻 Author

Built as a systems-programming project to explore:

C++20 • Linux Networking • TCP/IP • TLS • OpenSSL • HTTP/1.1 • Concurrency • Systems Architecture • Performance Engineering

<p align="center"> <i>Built from sockets up. 🔌 → 🔐 → 🌐</i> </p>
