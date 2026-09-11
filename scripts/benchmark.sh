#!/usr/bin/env bash

set -euo pipefail

------------------------------------------------------------------------------
HTTPS server benchmark helper
------------------------------------------------------------------------------
Usage:
./scripts/benchmark.sh
Optional environment variables:
SERVER_BINARY
SERVER_URL
REQUESTS
CONCURRENCY
Example:
REQUESTS=10000 CONCURRENCY=100 ./scripts/benchmark.sh
------------------------------------------------------------------------------

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

SERVER_BINARY="${SERVER_BINARY:-${PROJECT_ROOT}/build/cpp_https_server}"
SERVER_URL="${SERVER_URL:-https://127.0.0.1:8443/}"

REQUESTS="${REQUESTS:-1000}"
CONCURRENCY="${CONCURRENCY:-10}"

------------------------------------------------------------------------------
Requirements
------------------------------------------------------------------------------

if ! command -v curl >/dev/null 2>&1; then
echo "Error: curl is required."
exit 1
fi

if [[ ! -x "${SERVER_BINARY}" ]]; then
echo "Error: server executable not found:"
echo " ${SERVER_BINARY}"
echo
echo "Build the project first:"
echo
echo " cmake -S . -B build -DCMAKE_BUILD_TYPE=Release"
echo " cmake --build build -j"
exit 1
fi

------------------------------------------------------------------------------
Check server availability
------------------------------------------------------------------------------

echo "Checking server..."
echo "URL: ${SERVER_URL}"
echo "Requests: ${REQUESTS}"
echo "Concurrency: ${CONCURRENCY}"
echo

if ! curl
--silent
--show-error
--insecure
--output /dev/null
--max-time 5
"${SERVER_URL}"; then

echo
echo "Error: HTTPS server is not reachable."
echo "Start the server before running the benchmark."
exit 1


fi

------------------------------------------------------------------------------
Run benchmark
------------------------------------------------------------------------------

echo "Running benchmark..."
echo

START_TIME="$(date +%s%N)"

seq "${REQUESTS}" |
xargs
-n 1
-P "${CONCURRENCY}"
-I {}
curl
--silent
--show-error
--insecure
--output /dev/null
--write-out "%{http_code}\n"
--max-time 10
"${SERVER_URL}" |
tee /tmp/cpp_https_server_status_codes.txt >/dev/null

END_TIME="$(date +%s%N)"

------------------------------------------------------------------------------
Calculate statistics
------------------------------------------------------------------------------

ELAPSED_NS=$((END_TIME - START_TIME))
ELAPSED_SECONDS="$(awk "BEGIN { printf "%.3f", ${ELAPSED_NS} / 1000000000 }")"

if [[ "${ELAPSED_NS}" -gt 0 ]]; then
REQUESTS_PER_SECOND="$(
awk "BEGIN { printf "%.2f", ${REQUESTS} / (${ELAPSED_NS} / 1000000000) }"
)"
else
REQUESTS_PER_SECOND="N/A"
fi

SUCCESSFUL="$(
grep -Ec '^2[0-9][0-9]$'
/tmp/cpp_https_server_status_codes.txt || true
)"

FAILED=$((REQUESTS - SUCCESSFUL))

------------------------------------------------------------------------------
Results
------------------------------------------------------------------------------

echo
echo "=========================================="
echo "Benchmark Results"
echo "=========================================="
echo "Total requests: ${REQUESTS}"
echo "Successful requests: ${SUCCESSFUL}"
echo "Failed requests: ${FAILED}"
echo "Elapsed time: ${ELAPSED_SECONDS}s"
echo "Requests/sec: ${REQUESTS_PER_SECOND}"
echo "=========================================="

rm -f /tmp/cpp_https_server_status_codes.txt