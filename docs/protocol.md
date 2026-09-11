Protocol Specification
1. Overview

This document describes the network protocols implemented by the C++ HTTPS Server.

The server provides:

TCP-based client connections
TLS encryption using OpenSSL
HTTP/1.1 request/response handling
Persistent HTTP connections
Configurable request and response limits
HTTP routing

The protocol stack is:

┌─────────────────────────────┐
│        Application          │
│          HTTP/1.1           │
├─────────────────────────────┤
│            TLS              │
│          OpenSSL            │
├─────────────────────────────┤
│            TCP              │
├─────────────────────────────┤
│             IP              │
└─────────────────────────────┘

2. Transport Layer

The server uses TCP as its transport protocol.

Default ports
HTTP   : 8080
HTTPS  : 8443


The ports are configurable through the server configuration.

For a production deployment, HTTPS would normally be exposed on port 443.

Connection lifecycle
Client                         Server
  │                              │
  │────── TCP SYN ──────────────►│
  │◄───── TCP SYN/ACK ──────────│
  │────── TCP ACK ──────────────►│
  │                              │
  │       TLS handshake          │
  │◄────────────────────────────►│
  │                              │
  │       HTTP requests          │
  │─────────────────────────────►│
  │◄─────────────────────────────│
  │                              │
  │       Connection close       │
  │◄────────────────────────────►│


Each accepted TCP connection is represented internally by a Connection object.

3. TLS

HTTPS connections are protected using TLS.

The server uses OpenSSL for TLS implementation and cryptographic operations.

TLS responsibilities

TLS provides:

Encryption
Server authentication
Message integrity
Protection against network-level interception

The server must have:

server.crt
server.key


where:

server.crt is the server certificate.
server.key is the corresponding private key.

The private key must never be committed to the repository.

3.1 TLS Configuration

The TLS context is initialized once during server startup.

Conceptually:

Server startup
      │
      ▼
Load certificate
      │
      ▼
Load private key
      │
      ▼
Configure TLS
      │
      ▼
Create TLS context
      │
      ▼
Accept connections


Each client connection creates a TLS session using the shared server TLS context.

3.2 TLS Handshake

After TCP connection establishment:

Client                         Server
  │                              │
  │──── ClientHello ────────────►│
  │◄─── ServerHello  ────────────│
  │◄─── Certificate  ────────────│
  │◄─── TLS parameters  ─────────│
  │                              │
  │──── Key exchange ───────────►│
  │                              │
  │◄──── Finished ───────────────│
  │──── Finished ───────────────►│
  │                              │
  │       Encrypted HTTP         │
  │◄────────────────────────────►│


HTTP data is not processed until the TLS handshake has completed successfully.

If the handshake fails, the connection is terminated.

4. HTTP/1.1

The application protocol implemented by the server is HTTP/1.1.

An HTTP request has the following general structure:

METHOD request-target HTTP/1.1
Header-Name: value
Header-Name: value

optional body


Example:

GET / HTTP/1.1
Host: localhost:8443
Connection: keep-alive
Accept: */*

5. HTTP Request Parsing

The HTTP parser is responsible for converting raw bytes received from the TLS layer into a structured HttpRequest.

Conceptually:

TLS connection
      │
      ▼
Raw bytes
      │
      ▼
HTTP parser
      │
      ├── Method
      ├── Target
      ├── HTTP version
      ├── Headers
      └── Body
      │
      ▼
HttpRequest


A parsed request contains at minimum:

struct HttpRequest {
    HttpMethod method;
    std::string target;
    HttpVersion version;
    Headers headers;
    std::string body;
};

6. Supported HTTP Methods

The initial implementation supports:

GET
POST
PUT
DELETE
PATCH
HEAD
OPTIONS


Unsupported methods result in:

405 Method Not Allowed


The exact supported method set may be configured or extended as the server evolves.

7. HTTP Versions

The server currently targets:

HTTP/1.1


Requests using unsupported HTTP versions should result in:

505 HTTP Version Not Supported

8. Request Target

The request target is parsed into its path and optional query string.

Example:

GET /users?id=42 HTTP/1.1


is represented as:

Path:
    /users

Query:
    id=42


The router operates primarily on the path component.

9. Headers

HTTP headers are treated as case-insensitive.

Example:

Content-Type: application/json
Content-Length: 42
Connection: keep-alive


The parser normalizes header names internally to simplify lookup.

Example:

request.headers.get("Content-Length");


Header values remain strings until interpreted by the relevant HTTP component.

10. Request Body

Requests containing a body must provide an appropriate framing mechanism.

The initial implementation supports:

Content-Length


Example:

POST /api/users HTTP/1.1
Host: localhost
Content-Type: application/json
Content-Length: 27

{"name":"example","id":42}


The server reads exactly the declared number of bytes.

Requests exceeding the configured maximum body size are rejected.

Example response:

HTTP/1.1 413 Payload Too Large
Content-Length: 0
Connection: close

11. HTTP Response

The server generates responses using the following structure:

HTTP/1.1 STATUS_CODE REASON
Header-Name: value
Header-Name: value

optional body


Example:

HTTP/1.1 200 OK
Content-Type: text/plain
Content-Length: 12
Connection: keep-alive

Hello World!


The response abstraction is represented internally by HttpResponse.

12. Status Codes

The server uses standard HTTP status codes.

Successful responses
200 OK
201 Created
204 No Content

Client errors
400 Bad Request
401 Unauthorized
403 Forbidden
404 Not Found
405 Method Not Allowed
408 Request Timeout
413 Payload Too Large
414 URI Too Long

Server errors
500 Internal Server Error
501 Not Implemented
505 HTTP Version Not Supported

13. Routing

HTTP requests are passed from the parser to the router.

HttpRequest
     │
     ▼
  Router
     │
     ├── GET /
     ├── GET /health
     ├── GET /api/users
     ├── POST /api/users
     └── ...
     │
     ▼
Handler


A route is identified by:

HTTP method + request path


Example:

GET  /
GET  /health
GET  /api/users
POST /api/users
DELETE /api/users/:id


An unmatched route produces:

HTTP/1.1 404 Not Found

14. Keep-Alive

HTTP/1.1 persistent connections are supported.

A client may send multiple requests over the same TCP/TLS connection:

Client                         Server
  │                              │
  │──── GET / ──────────────────►│
  │◄─── 200 OK ─────────────────│
  │                              │
  │──── GET /health ────────────►│
  │◄─── 200 OK ─────────────────│
  │                              │
  │──── GET /api/users ─────────►│
  │◄─── 200 OK ─────────────────│
  │                              │


The connection remains open until:

The client requests closure.
The server requests closure.
A timeout occurs.
A protocol error occurs.
A server shutdown occurs.
15. Connection Header

The server recognizes the HTTP Connection header.

Example:

Connection: close


The server responds and closes the connection after completing the response.

For persistent connections:

Connection: keep-alive


the server may continue reading requests from the same connection.

16. Message Framing

The server must determine where an HTTP request ends before processing it.

For requests with a body, Content-Length is used to determine the body size.

Example:

Headers
   │
   ▼
Content-Length: 100
   │
   ▼
Read exactly 100 body bytes
   │
   ▼
Request complete


Malformed or ambiguous message framing must result in a protocol error rather than attempting to guess the request boundary.

17. Maximum Limits

To prevent resource exhaustion, the server enforces configurable limits.

Example defaults:

Maximum request line:       8 KB
Maximum header size:       16 KB
Maximum request body:      10 MB
Maximum connections:       1024
Request timeout:           30 seconds
Keep-alive timeout:         5 seconds


These values are configuration-dependent and may change.

When a request exceeds a configured limit, the server rejects it with an appropriate HTTP error.

18. Error Handling

Protocol errors should not crash the server.

The processing pipeline is:

Incoming data
     │
     ▼
TLS
     │
     ├── failure ──► Close connection
     │
     ▼
HTTP parser
     │
     ├── malformed ──► 400 Bad Request
     │
     ▼
Router
     │
     ├── no route ──► 404 Not Found
     │
     ▼
Handler
     │
     ├── failure ──► 500 Internal Server Error
     │
     ▼
HTTP response


Errors are logged through the server logging subsystem.

19. Timeouts

Timeouts protect the server from clients that establish connections but fail to make progress.

The server may enforce:

Connection timeout
TLS handshake timeout
Request header timeout
Request body timeout
Keep-alive timeout


A connection exceeding its timeout is closed.

20. Graceful Shutdown

During shutdown, the server stops accepting new connections.

Existing connections are allowed to finish when possible.

Shutdown signal
      │
      ▼
Stop accepting
      │
      ▼
Wait for active requests
      │
      ▼
Close connections
      │
      ▼
Release TLS resources
      │
      ▼
Exit


The exact shutdown grace period is configurable.

21. Security Requirements

The server follows these basic security requirements:

Private TLS keys must not be exposed.
TLS should use modern protocol versions.
Certificate validation must be performed by clients.
Request sizes must be bounded.
Header sizes must be bounded.
Connections must have timeouts.
User-controlled input must be validated.
Malformed HTTP requests must not cause undefined behavior.
Errors must not expose sensitive internal information.
TLS and HTTP parsing errors must not terminate the entire server.
22. Example HTTPS Request

A complete HTTPS interaction can be represented as:

                    Client
                      │
                      │ TCP connection
                      ▼
                 TCP Listener
                      │
                      ▼
                TLS Handshake
                      │
                      ▼
                 TLS Session
                      │
          encrypted HTTP request
                      │
                      ▼
                HTTP Parser
                      │
                      ▼
                 HttpRequest
                      │
                      ▼
                   Router
                      │
                      ▼
                  Handler
                      │
                      ▼
                HttpResponse
                      │
                      ▼
                HTTP Serializer
                      │
                      ▼
                 TLS Session
                      │
            encrypted response
                      │
                      ▼
                    Client

23. Protocol Processing Pipeline

The complete server pipeline is:

accept()
   │
   ▼
TCP connection
   │
   ▼
TLS handshake
   │
   ▼
TLS encrypted stream
   │
   ▼
HTTP/1.1 parser
   │
   ▼
HttpRequest
   │
   ▼
Router
   │
   ▼
Request handler
   │
   ▼
HttpResponse
   │
   ▼
HTTP serializer
   │
   ▼
TLS encryption
   │
   ▼
TCP
   │
   ▼
Client


This separation allows the networking, TLS, HTTP, and application layers to evolve independently.