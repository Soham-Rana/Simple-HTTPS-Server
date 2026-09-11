#include "tls/tls_context.hpp"

#include <openssl/err.h>
#include <openssl/ssl.h>

#include <stdexcept>
#include <utility>

namespace tls {

TlsContext::TlsContext(const std::string& certificate_file, const std::string& private_key_file)
    : certificate_file_(certificate_file), private_key_file_(private_key_file) {
    const SSL_METHOD* method = TLS_server_method();
    context_ = SSL_CTX_new(method);
    if (context_ == nullptr) {
        throw std::runtime_error("Failed to create OpenSSL TLS context");
    }
    configure();
}

TlsContext::~TlsContext() {
    if (context_ != nullptr) {
        SSL_CTX_free(context_);
        context_ = nullptr;
    }
}

SSL_CTX* TlsContext::native_handle() const noexcept { return context_; }

const std::string& TlsContext::certificate_file() const noexcept { return certificate_file_; }

const std::string& TlsContext::private_key_file() const noexcept { return private_key_file_; }

void TlsContext::configure() {
    SSL_CTX_set_min_proto_version(context_, TLS1_2_VERSION);
    SSL_CTX_set_default_read_buffer_len(context_, 4096);
    load_certificate();
    load_private_key();
    if (SSL_CTX_check_private_key(context_) != 1) {
        throw std::runtime_error("TLS certificate and private key do not match");
    }
}

void TlsContext::load_certificate() {
    if (SSL_CTX_use_certificate_file(context_, certificate_file_.c_str(), SSL_FILETYPE_PEM) <= 0) {
        throw std::runtime_error("Failed to load TLS certificate: " + certificate_file_);
    }
}

void TlsContext::load_private_key() {
    if (SSL_CTX_use_PrivateKey_file(context_, private_key_file_.c_str(), SSL_FILETYPE_PEM) <= 0) {
        throw std::runtime_error("Failed to load TLS private key: " + private_key_file_);
    }
}

}  // namespace tls
