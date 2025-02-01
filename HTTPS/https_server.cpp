#include "https_server.hpp"
// Global variable to control server loop
volatile sig_atomic_t running = 1;

HTTPS_SERVER::HTTPS_SERVER(int port, int max_threads) : port(port), max_threads(max_threads)
{
    // Initialize OpenSSL
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
    this->ctx = create_context();
    configure_context(this->ctx);
}

HTTPS_SERVER::~HTTPS_SERVER()
{
    close(server_socket);
    // Cleanup OpenSSL
    EVP_cleanup();
    SSL_CTX_free(ctx);
}

// Create SSL context
SSL_CTX *HTTPS_SERVER::create_context()
{
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    method = TLS_server_method();
    ctx = SSL_CTX_new(method);
    if (!ctx)
    {
        perror((time_stamp() + " Unable to create SSL context").c_str());
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    return ctx;
}

// Configure SSL context
void HTTPS_SERVER::configure_context(SSL_CTX *ctx)
{
    SSL_CTX_set_ecdh_auto(ctx, 1);

    // Set the certificate and key
    if (SSL_CTX_use_certificate_file(ctx, "server.crt", SSL_FILETYPE_PEM) <= 0)
    {
        perror((time_stamp() + " Error loading certificate").c_str());
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, "server.key", SSL_FILETYPE_PEM) <= 0)
    {
        perror((time_stamp() + " Error loading private key").c_str());
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}

int HTTPS_SERVER::open()
{
    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        perror((time_stamp() + " Socket creation failed").c_str());
        return 1;
    }

    int reuse = 1; // Allow address reuse
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        perror((time_stamp() + " setsockopt failed").c_str());
        return 1;
    }

    // Prepare server address
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);

    // Bind socket to address
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror((time_stamp() + " Binding failed").c_str());
        return 2;
    }

    // Listen for connections
    if (listen(server_socket, 5) < 0)
    {
        perror((time_stamp() + " Listening failed").c_str());
        return 3;
    }

    std::cout << time_stamp() << " Server listening on port " << port << std::endl;

    this->pool = new ThreadPool(max_threads);
    std::cout << time_stamp() << " ThreadPool " << max_threads << " threads running." << std::endl;

    return 0;
}

void HTTPS_SERVER::run()
{
    while (running)
    {
        struct sockaddr_in client_address;
        socklen_t client_address_len = sizeof(client_address);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);

        if (client_socket < 0)
        {
            if (errno == EINTR)
            { // Handle interrupted system call
                std::cout << time_stamp() << " Accept interrupted by signal. Continuing..." << std::endl;
                continue; // Go back to the beginning of the loop
            }
            else
            {
                perror((time_stamp() + " Accepting connection failed").c_str());
                break; // Or handle the error as appropriate
            }
        }

        SSL *ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client_socket);

        if (SSL_accept(ssl) <= 0)
        {
            perror((time_stamp() + " SSL accept error").c_str());
            ERR_print_errors_fp(stderr);
            close(client_socket);
            SSL_free(ssl);
            continue;
        }

        pool->enqueue(
            [ssl, this]()
            {
                handle_client(ssl);
            });
    }
}
