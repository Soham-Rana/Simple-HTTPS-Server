#pragma once

#include <memory>
#include <string>

#include <openssl/ssl.h>

namespace tls {

/**
 * Shared TLS configuration and OpenSSL context.
 *
 * A single TlsContext is created during server startup and shared by
 * individual TlsConnection instances.
 */
class TlsContext {
public:
    /**
     * Create a TLS server context using the supplied certificate and
     * private key.
     *
     * @throws std::runtime_error if OpenSSL initialization or TLS
     *     configuration fails.
     */
    TlsContext(
        const std::string& certificate_file,
        const std::string& private_key_file
    );

    ~TlsContext();

    TlsContext(const TlsContext&) = delete;
    TlsContext& operator=(const TlsContext&) = delete;

    TlsContext(TlsContext&&) = delete;
    TlsContext& operator=(TlsContext&&) = delete;

    /**
     * Returns the underlying OpenSSL SSL_CTX.
     * The returned pointer is owned by this object and must not be freed
     * by the caller.
     */
    [[nodiscard]]
    SSL_CTX* native_handle() const noexcept;

    /**
     * Returns the configured certificate path.
     */
    [[nodiscard]]
    const std::string& certificate_file() const noexcept;

    /**
     * Returns the configured private-key path.
     */
    [[nodiscard]]
    const std::string& private_key_file() const noexcept;

private:
    /**
     * Configure the OpenSSL context.
     */
    void configure();

    /**
     * Load and validate the certificate.
     */
    void load_certificate();

    /**
     * Load and validate the private key.
     */
    void load_private_key();

    std::string certificate_file_;
    std::string private_key_file_;
    SSL_CTX* context_{nullptr};
};

} // namespace tls