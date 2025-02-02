#include "https_server.hpp"
// Global variable to control server loop
volatile sig_atomic_t running = 1;

HTTPS_SERVER::HTTPS_SERVER(int port, int max_threads, std::string log_file_base) : port(port), max_threads(max_threads), log_file_base(log_file_base)
{
    log_file_index = 0;
    std::ostringstream log_file_name;
    log_file_name << log_file_base << "/server_log_" << std::setw(3) << std::setfill('0') << log_file_index << ".txt";

    if ((log_file = new std::ofstream(log_file_name.str(), std::ios_base::app)) == nullptr)
    { // Open log file in append mode
        perror((time_stamp() + "Log file fatal: not created").c_str());
    }

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

    // ThreadPool
    delete pool;

    if (log_file != nullptr && log_file->is_open())
    {
        log_file->close();
    }
    delete log_file;
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

// Handle requests
void HTTPS_SERVER::handle_client(SSL *ssl)
{
    char buffer[BUFFER_SIZE] = {0};
    int bytes_received = SSL_read(ssl, buffer, sizeof(buffer));

    if (bytes_received < 0)
    { // Check for errors
        perror((time_stamp() + " SSL read error").c_str());
        ERR_print_errors_fp(stderr);
        return;
    }
    else if (bytes_received == 0)
    { // Client disconnected
        std::cout << "Client disconnected." << std::endl;
        return;
    }

    // Basic request parsing (very simplified)
    std::string request(buffer, bytes_received); // Use bytes_received for correct string length
    std::string response;
    std::vector<std::string> tokens = split(request); // Assuming 'split' is defined elsewhere

    int response_status = 200;                                   // Default response status
    auto start_time = std::chrono::high_resolution_clock::now(); // Start time for response time calculation

    if (tokens.size() > 2)
    {
        const std::string &method = tokens[0];
        if (method == "get")
            response = GET_handler(tokens); // Assuming GET_handler is defined elsewhere
        else if (method == "post")
            response = POST_handler(tokens); // Assuming POST_handler is defined elsewhere
        else
        {
            response = NOT_IMPLEMENTED; // Assuming NOT_IMPLEMENTED is defined elsewhere
            response_status = 501;      // Not Implemented
        }

        if (response.empty())
        {
            // Log the request and response along with client details
            log_request_response(ssl, request, response_status, start_time);

            std::cerr << time_stamp() << " Remote command: stop. Shutting down server." << std::endl;
            SSL_shutdown(ssl);
            SSL_free(ssl);
            close(SSL_get_fd(ssl)); // Close the socket
            exit(13);               // Exit the application immediately
        }
    }
    else
    {
        response = NOT_IMPLEMENTED;
        response_status = 400; // Bad Request
    }

    SSL_write(ssl, response.c_str(), response.length());

    // Log the request and response along with client details
    log_request_response(ssl, request, response_status, start_time);

    // Bi-directional shutdown (more robust)
    int sd = SSL_get_fd(ssl);
    if (sd != -1)
    {                      // Check if the socket is valid
        SSL_shutdown(ssl); // Send and receive close_notify
        close(sd);
    }
    SSL_free(ssl);
}

// Function to log requests and responses with file size limit
void HTTPS_SERVER::log_request_response(const SSL *ssl, const std::string &request, int response_status,
                                        std::chrono::time_point<std::chrono::high_resolution_clock> &start_time)
{
    if (log_file == nullptr)
        return;

    auto end_time = std::chrono::high_resolution_clock::now(); // End time for response time calculation
    double response_time = std::chrono::duration<double, std::milli>(end_time - start_time).count();
    auto t_stamp = time_stamp(); // get once for speed

    // Lock the mutex before writing to the log file
    std::lock_guard<std::mutex> guard(log_mutex);

    // Check if the current log file exceeds the maximum size
    if (log_file->tellp() > LOG_MAX_SIZE)
    {
        log_file->close();
        delete log_file;

        log_file_index++;
        std::ostringstream log_file_name;
        log_file_name << log_file_base << "/server_log_" << std::setw(3) << std::setfill('0') << log_file_index << ".txt";
        if ((log_file = new std::ofstream(log_file_name.str(), std::ios_base::app)) == nullptr)
        { // Open log file in append mode
            perror((t_stamp + "Log file fatal: not created").c_str());
            return;
        }
    }

    // Get client IP address and port
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    getpeername(SSL_get_fd(ssl), (struct sockaddr *)&addr, &addr_len);
    std::string client_ip = inet_ntoa(addr.sin_addr);
    int client_port = ntohs(addr.sin_port);

    *log_file << t_stamp << " Client " << client_ip << ":" << client_port << " status: " << response_status << " duration: " << response_time << " ms" << std::endl;

    auto ss = std::stringstream{request};
    int i = 0;
    for (std::string line; std::getline(ss, line) && i < 6; i++)
    {
        // request an user agent
        *log_file << t_stamp << '\t' << line << std::endl;
    }
}