# 🔐 C++ HTTPS Server

<p align="center">
  <strong>Modular HTTPS server built from scratch in modern C++20.</strong>
</p>

<p align="center">
  TCP Sockets → TLS → HTTP/1.1 → Routing → Concurrency
</p>

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-20-00599C?style=for-the-badge&logo=cplusplus&logoColor=white" alt="C++20">
  <img src="https://img.shields.io/badge/OpenSSL-3.x-721412?style=for-the-badge&logo=openssl&logoColor=white" alt="OpenSSL">
  <img src="https://img.shields.io/badge/CMake-3.20%2B-064F8C?style=for-the-badge&logo=cmake&logoColor=white" alt="CMake">
  <img src="https://img.shields.io/badge/HTTP-1.1-F7931E?style=for-the-badge" alt="HTTP/1.1">
  <img src="https://img.shields.io/badge/Linux-Primary%20Platform-FCC624?style=for-the-badge&logo=linux&logoColor=black" alt="Linux">
  <img src="https://img.shields.io/badge/Status-Work%20in%20Progress-orange?style=for-the-badge" alt="Work in Progress">
</p>

<p align="center">
  <em>
    A systems-programming project exploring how an HTTPS server works from the
    TCP socket layer to application-level HTTP routing.
  </em>
</p>

---

## 📌 Overview

**C++ HTTPS Server** is a modular HTTP/1.1 server implemented in **C++20** using Linux/POSIX networking APIs and OpenSSL.

The project deliberately avoids high-level web frameworks and implements the core server pipeline as independent layers:

```text
┌─────────────────────────────────────────────────────┐
│                     HTTP Client                     │
└──────────────────────────┬──────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────┐
│                    TCP Sockets                      │
└──────────────────────────┬──────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────┐
│                 TLS / OpenSSL                       │
│              Encryption & Handshake                 │
└──────────────────────────┬──────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────┐
│                  HTTP/1.1 Parser                    │
│              Requests & Headers                     │
└──────────────────────────┬──────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────┐
│                      Router                         │
└──────────────────────────┬──────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────┐
│                  Request Handler                    │
└──────────────────────────┬──────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────┐
│                 HTTP Response                       │
└─────────────────────────────────────────────────────┘
```
The main goal is not to compete with production servers such as nginx or Apache.

The goal is to understand the engineering behind a network server:

- How TCP connections are established and managed
- How TLS secures a byte stream
- How HTTP requests are parsed
- How routes are matched to handlers
- How multiple clients are handled concurrently
- How resources are safely managed in C++
- How networking code can be tested and benchmarked

> 🚧 **Status:** Work in progress

## ✨ Features

### 🌐 Networking

- TCP socket abstraction
- Client connection management
- Configurable host and port
- Connection limits
- Connection timeouts
- Graceful shutdown

### 🔐 TLS / HTTPS

- OpenSSL integration
- TLS certificate configuration
- Private-key configuration
- TLS 1.2+
- Encrypted client/server communication
- Development certificate generation

### 📡 HTTP/1.1

- Request parsing
- Request headers
- Request bodies
- HTTP response serialization
- Status codes
- Persistent connections / keep-alive
- Request-size limits
- Request timeouts

### 🛣️ Routing

- HTTP method matching
- Path-based routing
- Parameterized routes
- Extensible request handlers

### 🧵 Concurrency

- Thread-pool based architecture
- Concurrent client handling
- Configurable worker count
- Connection management

### 🧪 Engineering

- Modern C++20
- CMake build system
- RAII-based resource management
- Structured logging
- Configuration files
- Unit tests
- Integration tests
- AddressSanitizer
- UndefinedBehaviorSanitizer
- Benchmarking infrastructure

## 🏛️ Architecture

The server follows a layered architecture where each component has a focused responsibility.

```text
                    ┌──────────────┐
                    │    Client    │
                    └──────┬───────┘
                           │
                           ▼
                 ┌──────────────────┐
                 │   TCP Listener   │
                 └────────┬─────────┘
                          │
                          ▼
                 ┌──────────────────┐
                 │   Connection     │
                 │    Manager       │
                 └────────┬─────────┘
                          │
                          ▼
                 ┌──────────────────┐
                 │   TLS / OpenSSL  │
                 └────────┬─────────┘
                          │
                          ▼
                 ┌──────────────────┐
                 │   HTTP Parser    │
                 └────────┬─────────┘
                          │
                          ▼
                 ┌──────────────────┐
                 │      Router      │
                 └────────┬─────────┘
                          │
                          ▼
                 ┌──────────────────┐
                 │     Handler      │
                 └────────┬─────────┘
                          │
                          ▼
                 ┌──────────────────┐
                 │  HTTP Response   │
                 └──────────────────┘
```
C++ HTTPS Server
Request Lifecycle

A request such as:
```text
GET /health HTTP/1.1
Host: localhost:8443
Connection: keep-alive
```

moves through the system as:
```text
TCP connection
      ↓
TLS handshake
      ↓
Encrypted bytes
      ↓
TLS decryption
      ↓
HTTP parsing
      ↓
Route matching
      ↓
Request handler
      ↓
Response serialization
      ↓
TLS encryption
      ↓
TCP transmission
      ↓
Client
```

This separation allows individual components to be developed and tested independently.

<h3>📁 Project Structure</h3>
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

Module Responsibilities
Module	Responsibility
net/	TCP sockets and network connections
tls/	OpenSSL context, TLS handshake, encrypted I/O
http/	HTTP/1.1 parsing and response serialization
server/	Server lifecycle and connection orchestration
thread/	Thread pool and worker management
utils/	Configuration, logging, and shared utilities
tests/	Unit and integration tests
benchmarks/	Performance benchmarks
scripts/	Development utilities
🛠️ Requirements
Supported Environment

The primary development environment is Linux.

Dependency	Version
C++	C++20
GCC	11+
Clang	14+
CMake	3.20+
OpenSSL	3.x
Threads	POSIX/pthreads
Git	Latest recommended
Install Dependencies

On Debian/Ubuntu:

sudo apt update

sudo apt install \
    build-essential \
    cmake \
    git \
    libssl-dev


Verify:

g++ --version
cmake --version
openssl version

<h3>🚀 Quick Start</h3>
1. Clone
git clone <repository-url>
cd cpp-https-server

2. Generate Development Certificates
./scripts/generate_certs.sh


This should create:

certs/
├── server.crt
└── server.key


<h2>⚠️ Development certificates are for local testing only.</h2>

<h3>3. Configure</h3>

```text
  cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release
```
<h3>4. Build</h3>

```text
cmake --build build -j
```
<h3>5. Run</h3>

```text
./build/cpp_https_server
```

The default development endpoint is:
```text
https://127.0.0.1:8443/
```

<h3>🔐 TLS Configuration</h3>

The server expects a certificate and private key.

Default configuration:

certs/server.crt
certs/server.key


For local development, certificates can be generated with:

./scripts/generate_certs.sh

Production Warning

Do not use development/self-signed certificates for production.

Production deployments should use:

A certificate issued by a trusted Certificate Authority
Proper private-key protection
Secure TLS configuration
Current OpenSSL releases

Never commit private keys to Git.

<h3>🌍 Running the Server</h3>

Start the server:
```text
./build/cpp_https_server
```

Default configuration:
```text
Setting	Value
Host	0.0.0.0
Port	8443
Protocol	HTTPS
TLS	1.2+
Certificate	certs/server.crt
Private key	certs/server.key
```
Open in a browser:
```text
https://127.0.0.1:8443/
```

The development interface includes a simple form that submits data to:

POST /print


Health check:

GET /health


Stop the server with:
```text
Ctrl+C
```
<h3>🪟 Windows / MinGW</h3>

A Windows development environment using MinGW/MSYS2 can run the test executable with:
```text
$env:Path += ';C:\msys64\ucrt64\bin;C:\msys64\usr\bin'
.\cpp_https_server_test.exe
```

If the terminal running the server is unavailable:
```text
Stop-Process -Name cpp_https_server_test -Force
```

For Windows curl, local certificate revocation lookup may need to be disabled:
```text
curl.exe --ssl-no-revoke https://127.0.0.1:8443/health
```

**The exact executable name may differ depending on the current build configuration.**

<h3>🧪 Testing with cURL</h3>

For a self-signed development certificate:
```text
curl -k https://localhost:8443/
```

Health check:
```text
curl -k https://localhost:8443/health
```

Verbose TLS debugging:
```text
curl -kv https://localhost:8443/
```

The -k flag disables certificate verification and should only be used for local development.

<h3>📡 HTTP API</h3>

Example Routes: 
|Method|Endpoint|Description|
|---|---|---|
| GET | / | Server information |
| GET	| /health | Health check |
| POST | /print | Process submitted development data |
| GET	| /api/users |	Retrieve users |
| POST | /api/users	| Create a user
| DELETE |/api/users/:id	| Delete a user |

**The routing layer is independent of the TCP and TLS implementations.**

This allows application handlers to be added without modifying the underlying networking stack.

<h3>📨 HTTP Example</h3>

### Request
GET / HTTP/1.1 <br>
Host: localhost:8443 <br>
Connection: close <br><br>

### Response
HTTP/1.1 200 OK <br>
Content-Type: text/plain <br>
Content-Length: 12 <br>
Connection: close <br><br>

Hello World! 

<h3>⚙️ Configuration</h3>

Configuration is separated from application code.

Example: <br><br>

[server]<br>
host = 0.0.0.0<br>
port = 8443<br>
workers = 4<br><br>

[http]<br>
max_request_size = 10485760<br>
request_timeout = 30<br>
keep_alive_timeout = 5<br><br>

[tls]<br>
certificate = certs/server.crt<br>
private_key = certs/server.key<br><br>


Configuration allows operational parameters to be changed without recompiling the server.

<h3>🧵 Concurrency Model</h3>

The server uses a thread pool to handle clients concurrently.

                       Listening Socket
                              │
                           accept()
                              │
          ┌───────────────────┼───────────────────┐
          │                   │                   │
          ▼                   ▼                   ▼
       Client A            Client B            Client C
          │                   │                   │
          └───────────────────┼───────────────────┘
                              │
                              ▼
                        ┌───────────┐
                        │ Thread    │
                        │   Pool    │
                        └─────┬─────┘
                              │
                ┌─────────────┼─────────────┐
                ▼             ▼             ▼
             Worker 1      Worker 2      Worker 3


The design keeps concurrency concerns separate from HTTP parsing and request routing.

This makes it possible to evolve the concurrency layer independently toward event-driven I/O in the future.

<h3>🧪 Testing</h3>

Configure with tests enabled:
```text
cmake -S . -B build \
    -DBUILD_TESTS=ON
```

Build:
```text
cmake --build build -j
```

Run:
```text
ctest --test-dir build --output-on-failure
```

Testing focuses on both isolated components and complete request/response flows.

Areas include:
```text
HTTP parsing
HTTP serialization
Routing
Configuration
Connection handling
HTTPS integration
```
<h3>🧰 Sanitizers</h3>

The project supports:
```text
AddressSanitizer
UndefinedBehaviorSanitizer
```
Configure:
```text
cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Debug \
    -DENABLE_SANITIZERS=ON
```

Build:
```text
cmake --build build -j
```

Run tests:
```text
ctest --test-dir build --output-on-failure
```

Sanitizer builds help identify memory-safety and undefined-behavior issues during development.

<h3>📊 Benchmarks</h3>

Benchmarking can be enabled with:
```text
cmake -S . -B build \
    -DBUILD_BENCHMARKS=ON
```

Build:
```text
cmake --build build -j
```

The benchmark infrastructure is intended to measure:
```text
Requests per second
Connection establishment latency
HTTP parsing throughput
Concurrent connection handling
TLS handshake overhead
```
Performance optimization follows a measure-first approach:
```text
Implement
   ↓
Benchmark
   ↓
Profile
   ↓
Identify bottleneck
   ↓
Optimize
   ↓
Benchmark again
```
<h3>🧠 Design Principles </h3>
Separation of Concerns

TCP, TLS, HTTP, routing, concurrency, configuration, and application logic are implemented as independent components.

RAII and Resource Safety

C++ RAII is used wherever practical to provide deterministic cleanup of:

1. File descriptors
2. Sockets
3. TLS resources
4. Threads
5. Other system resources
6. Explicit Error Handling

Network and protocol failures should be handled explicitly rather than causing unexpected process termination.

**Configurability**

Operational parameters should be configurable without recompiling the application.

**Testability**

Core components should be testable without requiring a live network connection whenever possible.

**Performance**

The implementation aims to minimize unnecessary allocations, system calls, and synchronization overhead while supporting concurrent clients.

<h3>🔒 Security</h3>

This project is primarily an educational and portfolio systems-programming project.

It is not currently intended for production Internet-facing deployments.

Before production use, additional hardening is required.

## Important areas include:

- Trusted CA-issued certificates
- Private-key protection
- TLS configuration hardening
- Request-size limits
- Connection and request timeouts
- Strict HTTP parsing
- Input validation
- Rate limiting
- Error handling
- Privilege reduction
- Dependency updates
- Security testing
- Protocol fuzzing

Never expose an untested development server directly to the public Internet.

<h3>🗺️ Roadmap</h3>

### Core Networking

- - [x] TCP listener
- - [x] Socket abstraction
- - [x] Connection handling
- - [x] Graceful shutdown

### TLS

- - [x] OpenSSL integration
- - [x] TLS context
- - [x] Certificate loading
- - [x] Private-key loading
- - [x] HTTPS connections

### HTTP

- - [x] HTTP/1.1 parser
- - [x] HTTP response serializer
- - [x] Request/response abstraction
- - [x] Keep-alive connections
- - [ ] Chunked transfer encoding
- - [ ] Expanded HTTP/1.1 compliance


### Routing
- - [x] Request router
- - [x] HTTP method matching
- - [x] Path matching
- - [x] Parameterized routes
  
### Concurrency
- - [x] Thread pool
- - [x] Concurrent connections
- - [x] Configurable workers
- - [ ] Linux epoll
- - [ ] Event-driven I/O

### Quality & Performance
- - [x] Configuration system
- - [x] Structured logging
- - [x] Unit tests
- - [x] Integration tests
- - [x] Sanitizer support
- - [x] Benchmarking
- - [ ] Fuzz testing
- - [ ] Production hardening

### Future Protocol Support
- - [ ] HTTP/2
- - [ ] Advanced TLS configuration
- - [ ] Improved connection management

<h3>📚 Learning Outcomes</h3>

This project provides practical experience with:
| Area | Concepts |
|---|---|
| C++20 | RAII, ownership, concurrency, modular design |
| Networking | TCP, sockets, connections, I/O |
| TLS | Handshakes, certificates, encrypted transport |
| OpenSSL | TLS context and secure I/O |
| HTTP | Parsing, headers, requests, responses |
| Concurrency | Thread pools, workers, synchronization |
| Systems Design | Layered architecture and resource ownership |
| Testing | Unit tests, integration tests, sanitizers |
| Performance | Throughput, latency, benchmarking |

<h3>🤝 Contributing</h3>

This is primarily a learning and portfolio project, but contributions and suggestions are welcome.

Before submitting a pull request:
```text
Build the project successfully.
Run the test suite.
Run sanitizer builds for relevant changes.
Keep changes focused.
Document significant architectural changes.
Do not commit private keys, certificates containing secrets, or build artifacts.
```
For larger architectural changes, opening an issue before implementation is recommended.

<h3>📄 License</h3>

This project is distributed under the MIT License.

See LICENSE for details.

<h3>👨‍💻 Author</h3>

Built as a systems-programming project to explore modern C++ networking, secure communication, concurrency, and server architecture.

## Technologies
```text
C++20
Linux
POSIX Sockets
TCP/IP
OpenSSL
TLS
HTTP/1.1
CMake
Multithreading
RAII
Sanitizers
Benchmarking
Systems Architecture
```
<p align="center"> <strong>Built from the socket layer up.</strong> </p> <p align="center"> TCP → TLS → HTTP → Routing → Concurrency </p>
