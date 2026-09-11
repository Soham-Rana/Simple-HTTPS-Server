#!/usr/bin/env bash

set -euo pipefail

------------------------------------------------------------------------------
Development TLS certificate generator
------------------------------------------------------------------------------
Generates a self-signed certificate for local HTTPS development.
Output:
certs/server.crt
certs/server.key
Usage:
./scripts/generate_certs.sh
These certificates are NOT intended for production use.
------------------------------------------------------------------------------

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
CERT_DIR="${PROJECT_ROOT}/certs"

CERT_FILE="${CERT_DIR}/server.crt"
KEY_FILE="${CERT_DIR}/server.key"

DAYS="${CERT_DAYS:-365}"
COMMON_NAME="${CERT_COMMON_NAME:-localhost}"

------------------------------------------------------------------------------
Requirements
------------------------------------------------------------------------------

if ! command -v openssl >/dev/null 2>&1; then
echo "Error: OpenSSL is not installed."
echo "Install it using your system package manager."
exit 1
fi

------------------------------------------------------------------------------
Prepare output directory
------------------------------------------------------------------------------

mkdir -p "${CERT_DIR}"

------------------------------------------------------------------------------
Generate private key
------------------------------------------------------------------------------

echo "Generating private key..."

openssl genrsa
-out "${KEY_FILE}"
4096

chmod 600 "${KEY_FILE}"

------------------------------------------------------------------------------
Generate self-signed certificate
------------------------------------------------------------------------------

echo "Generating self-signed certificate..."

openssl req
-x509
-new
-nodes
-key "${KEY_FILE}"
-sha256
-days "${DAYS}"
-out "${CERT_FILE}"
-subj "/C=XX/ST=Development/L=Local/O=CPP HTTPS Server/OU=Development/CN=${COMMON_NAME}"
-addext "subjectAltName=DNS:${COMMON_NAME},DNS:localhost,IP:127.0.0.1"

------------------------------------------------------------------------------
Verify generated certificate
------------------------------------------------------------------------------

echo
echo "Certificate generated successfully."
echo
echo "Certificate:"
echo " ${CERT_FILE}"
echo
echo "Private key:"
echo " ${KEY_FILE}"
echo
echo "Certificate details:"
echo

openssl x509
-in "${CERT_FILE}"
-noout
-subject
-issuer
-dates
-ext subjectAltName

echo
echo "WARNING: This is a self-signed development certificate."
echo "Do not use it for production deployments."