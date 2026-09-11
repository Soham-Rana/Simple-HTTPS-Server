TLS Certificates

This directory contains TLS certificates used by the development HTTPS server.

Expected Files

The server expects the following files for local development:

certs/
├── server.crt
└── server.key

server.crt — X.509 server certificate.
server.key — corresponding private key.
Generate Development Certificates

Use the certificate-generation script from the project root:

./scripts/generate_certs.sh


This should generate:

certs/server.crt
certs/server.key


For local testing, the certificate may be self-signed.

Test the Certificate

After starting the server:

curl -k https://localhost:8443/


The -k option tells curl to accept a self-signed certificate.

Do not use -k in production environments.

Security

Never commit private keys to Git.

The following files are intentionally ignored by .gitignore:

*.key
*.pem
*.p12
*.pfx


In particular:

certs/server.key


must remain private.

If a private key is accidentally committed, consider it compromised and generate a new key immediately.

Production Certificates

Self-signed certificates are intended only for development and testing.

For production deployment, use a certificate issued by a trusted Certificate Authority (CA).

The production certificate configuration should be provided through deployment configuration or a secure secret-management system rather than committed to the repository.

Certificate Renewal

Certificates have an expiration date.

Production deployments should have a process for:

Obtaining a renewed certificate.
Installing the new certificate.
Validating the certificate and private key.
Reloading or restarting the server safely.
Removing expired credentials.
Repository Policy

This directory intentionally contains no real credentials.

The repository tracks:

certs/.gitkeep
certs/README.md


but does not track:

certs/server.key
certs/*.pem
certs/*.p12
certs/*.pfx


Development credentials should be generated locally.