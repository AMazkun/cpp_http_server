#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream> // For logging to a file
#include <mutex>   // For std::mutex
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "../common/thread_pools.hpp"
#include "../common/handlers.hpp"
#include "../common/parsing.hpp"

#define LOG_MAX_SIZE 1024 * 1024
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

    // Mutex for protecting log access
    std::mutex log_mutex;
    int log_file_index = 0;
    std::string log_file_base = "";
    std::ofstream *log_file = nullptr;

    // Create SSL context
    SSL_CTX *create_context();
    void configure_context(SSL_CTX *ctx);

    void handle_client(SSL *ssl);
    void log_request_response(const SSL *ssl, const std::string &request, int response_status,
                              std::chrono::time_point<std::chrono::high_resolution_clock> &start_time);

public:
    HTTPS_SERVER(int port, int max_threads, std::string log_file_base);
    ~HTTPS_SERVER();
    int open();
    void run();
};
