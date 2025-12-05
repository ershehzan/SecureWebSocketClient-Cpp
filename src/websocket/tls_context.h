#pragma once
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <memory>

using namespace std;
class TLSContext {
public:
        TLSContext();
       ~TLSContext();
        SSL_CTX* get() const;

private:
    SSL_CTX* ctx;
};
