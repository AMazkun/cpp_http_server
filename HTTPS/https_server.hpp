#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "../common/thread_pools.hpp"
#include "../common/handlers.hpp"
#include "../common/parsing.hpp"

// Global variable to control server loop
extern volatile sig_atomic_t running;

class HTTPS_SERVER
{
private:
    int server_socket;
    struct sockaddr_in server_address;
    int port = 0;
    int max_threads = 0;
    ThreadPool *pool = nullptr;
    // OpenSSL
    SSL_CTX *ctx = nullptr;

    // Create SSL context
    SSL_CTX *create_context();
    void configure_context(SSL_CTX *ctx);

public:
    HTTPS_SERVER(int port, int max_threads);
    ~HTTPS_SERVER();
    int open();
    void run();
};
