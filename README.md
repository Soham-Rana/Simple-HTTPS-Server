C++ HTTPS Server

A modular HTTPS server written in modern C++20.

The project implements a layered networking architecture with TCP, TLS, HTTP/1.1 parsing, request routing, concurrent connection handling, structured logging, configuration, and automated testing.

Status: Work in progress

Features
C++20 implementation
TCP socket abstraction
HTTPS using OpenSSL
TLS certificate and private-key configuration
HTTP/1.1 request parsing
HTTP request/response abstraction
HTTP routing
Persistent connections / keep-alive
Thread-pool based concurrency
Configurable request and connection limits
Connection and request timeouts
Structured server logging
Graceful shutdown
Unit and integration tests
CMake-based build system
AddressSanitizer / UndefinedBehaviorSanitizer support
Performance benchmarking infrastructure
Architecture

The server is organized into independent layers:

                         ┌───────────────────┐
                         │      Client       │
                         └─────────┬─────────┘
                                   │
                                   ▼
                         ┌───────────────────┐
                         │       TCP         │
                         └─────────┬─────────┘
                                   │
                                   ▼
                         ┌───────────────────┐
                         │       TLS         │
                         │     OpenSSL       │
                         └─────────┬─────────┘
                                   │
                                   ▼
                         ┌───────────────────┐
                         │   HTTP/1.1 Parser │
                         └─────────┬─────────┘
                                   │
                                   ▼
                         ┌───────────────────┐
                         │      Router       │
                         └─────────┬─────────┘
                                   │
                                   ▼
                         ┌───────────────────┐
                         │ Request Handler   │
                         └─────────┬─────────┘
                                   │
                                   ▼
                         ┌───────────────────┐
                         │ HTTP Response     │
                         └───────────────────┘

Project Structure
cpp-https-server/
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

Requirements
Linux

Recommended environment:

Linux
CMake 3.20+
GCC 11+ or Clang 14+
OpenSSL 3.x
pthreads
Git

Install dependencies on Debian/Ubuntu:

sudo apt update
sudo apt install \
    build-essential \
    cmake \
    git \
    libssl-dev

Building

Clone the repository:

git clone <repository-url>
cd cpp-https-server


Configure the project:

cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release


Build:

cmake --build build -j


The resulting executable will be:

build/cpp_https_server

Debug Build

For development:

cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Debug

cmake --build build -j

Sanitizer Build

AddressSanitizer and UndefinedBehaviorSanitizer can be enabled with:

cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Debug \
    -DENABLE_SANITIZERS=ON

cmake --build build -j


Run:

./build/cpp_https_server

TLS Certificates

The server expects a TLS certificate and corresponding private key.

Development certificates can be generated using:

./scripts/generate_certs.sh


The generated files should be stored under:

certs/
├── server.crt
└── server.key


Private keys must never be committed to version control.

For production deployments, use certificates issued by a trusted Certificate Authority.

Running

After building and configuring the server, start it from the project root.

Linux or macOS:

./build/cpp_https_server

Windows with the MinGW executable:

```powershell
$env:Path += ';C:\msys64\ucrt64\bin;C:\msys64\usr\bin'
.\cpp_https_server_test.exe
```

The server prints a startup message and keeps running in that terminal. Open
the following address in a browser to use the development form:

https://127.0.0.1:8443/

The form submits user text to `POST /print` and displays it in the response.
The health endpoint is available at:

https://127.0.0.1:8443/health

To stop the server, focus the terminal where it is running and press `Ctrl+C`.
On Windows, if that terminal is unavailable, stop the process from another
PowerShell window:

```powershell
Stop-Process -Name cpp_https_server_test -Force
```

Current development entry point:

- Host: `0.0.0.0`
- Port: `8443`
- Protocol: HTTPS with TLS 1.2 or newer
- Certificate: `certs/server.crt`
- Private key: `certs/server.key`

The local certificate is trusted on this Windows machine through `mkcert`.
On another machine, install that machine's local development CA or use a
certificate issued by a trusted public Certificate Authority. For Windows
`curl`, local revocation lookup may need to be disabled:

```powershell
curl.exe --ssl-no-revoke https://127.0.0.1:8443/health
```

The HTTPS endpoint can also be tested with:

curl -k https://localhost:8443/


The -k option is appropriate for testing with a self-signed development certificate.

Example Request
GET / HTTP/1.1
Host: localhost:8443
Connection: close


Example response:

HTTP/1.1 200 OK
Content-Type: text/plain
Content-Length: 12
Connection: close

Hello World!

HTTP API

Example routes:

Method	Path	Description
GET	/	Server information
GET	/health	Health check
GET	/api/users	Retrieve users
POST	/api/users	Create a user
DELETE	/api/users/:id	Delete a user

The routing layer is designed to allow additional handlers to be registered without modifying the networking or TLS layers.

Configuration

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


The exact configuration options depend on the current implementation.

Testing

Configure with tests enabled:

cmake -S . -B build \
    -DBUILD_TESTS=ON


Build:

cmake --build build -j


Run:

ctest --test-dir build --output-on-failure

Benchmarks

Performance benchmarks can be enabled with:

cmake -S . -B build \
    -DBUILD_BENCHMARKS=ON

cmake --build build -j


Benchmarking focuses on:

Requests per second
Connection establishment latency
HTTP parsing throughput
Concurrent connection handling
TLS handshake overhead
Design Goals

The project is designed around the following principles:

Separation of concerns

Networking, TLS, HTTP parsing, routing, and application logic are isolated into separate modules.

Resource safety

C++ RAII is used to manage sockets, TLS resources, threads, and other system resources.

Explicit failure handling

Network and protocol errors are handled explicitly instead of being allowed to terminate the server process.

Configurability

Operational parameters should be configurable without recompiling the server.

Testability

Core components such as HTTP parsing and routing should be independently testable without requiring a live network connection.

Performance

The server is designed to support concurrent clients while minimizing unnecessary allocations and system calls.

Security Considerations

This project is primarily intended as an educational and portfolio systems project.

Before deploying to the public Internet, additional hardening is required.

Important considerations include:

Use a trusted CA-issued certificate.
Protect the TLS private key.
Disable obsolete TLS versions.
Configure appropriate cipher suites.
Enforce request size limits.
Enforce connection timeouts.
Validate all client-controlled input.
Protect against malformed HTTP requests.
Implement appropriate rate limiting.
Avoid leaking internal errors.
Run with the minimum required operating-system privileges.
Keep OpenSSL and other dependencies updated.
Roadmap
 TCP listener
 HTTP protocol design
 HTTP/1.1 parser
 HTTP response serializer
 TLS context
 HTTPS connections
 Request router
 Thread pool
 Keep-alive connections
 Configuration system
 Structured logging
 Graceful shutdown
 Unit tests
 Integration tests
 Benchmarks
 Linux epoll event loop
 HTTP/2 support
 Production hardening
License

This project is distributed under the MIT License.

See LICENSE for details.

Author

Built as a systems-programming project to explore:

C++20
Linux networking
TCP/IP
TLS
OpenSSL
HTTP/1.1
Concurrent programming
Systems architecture
Performance engineering