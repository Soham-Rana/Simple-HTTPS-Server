#include "tls/tls_context.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace {

void expect(
bool condition,
std::string_view message
)
{
if (!condition) {
throw std::runtime_error(
std::string("Assertion failed: ") +
std::string(message)
);
}
}

std::filesystem::path project_root()
{
if (const char* root = std::getenv("PROJECT_SOURCE_DIR")) {
return root;
}

return ".";


}

std::filesystem::path certificate_path()
{
return project_root() / "certs" / "server.crt";
}

std::filesystem::path private_key_path()
{
return project_root() / "certs" / "server.key";
}

void test_certificate_files_exist()
{
expect(
std::filesystem::exists(certificate_path()),
"Test certificate should exist"
);

expect(
    std::filesystem::exists(private_key_path()),
    "Test private key should exist"
);


}

void test_context_creation()
{
const auto certificate = certificate_path().string();
const auto private_key = private_key_path().string();

tls::TlsContext context(
    certificate,
    private_key
);

expect(
    context.native_handle() != nullptr,
    "TLS context should contain an SSL_CTX"
);

expect(
    context.certificate_file() == certificate,
    "Certificate path should be preserved"
);

expect(
    context.private_key_file() == private_key,
    "Private key path should be preserved"
);


}

void test_invalid_certificate()
{
bool threw = false;

try {
    tls::TlsContext context(
        "certs/does-not-exist.crt",
        private_key_path().string()
    );
}
catch (const std::exception&) {
    threw = true;
}

expect(
    threw,
    "Invalid certificate should cause context creation to fail"
);


}

void test_invalid_private_key()
{
bool threw = false;

try {
    tls::TlsContext context(
        certificate_path().string(),
        "certs/does-not-exist.key"
    );
}
catch (const std::exception&) {
    threw = true;
}

expect(
    threw,
    "Invalid private key should cause context creation to fail"
);


}

} // namespace

int main()
{
try {
test_certificate_files_exist();
test_context_creation();
test_invalid_certificate();
test_invalid_private_key();

    std::cout << "tls_context_tests: PASS\n";
    return EXIT_SUCCESS;
}
catch (const std::exception& exception) {
    std::cerr
        << "tls_context_tests: FAIL\n"
        << exception.what()
        << '\n';

    return EXIT_FAILURE;
}


}