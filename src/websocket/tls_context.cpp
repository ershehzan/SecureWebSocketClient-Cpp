#include "tls_context.h"
#include <iostream>

TLSContext::TLSContext() {
    // Initialize OpenSSL
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
    
    // Create Context (TLS Client)
    ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
        cerr << "Failed to create SSL Context" << endl;
    }
}
TLSContext::~TLSContext() {
    if (ctx) SSL_CTX_free(ctx);
}

SSL_CTX* TLSContext::get() const {
    return ctx;
}
