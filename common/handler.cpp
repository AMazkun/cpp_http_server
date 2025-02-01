#include <string>
#include <vector>
#include <iostream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "handlers.hpp"
#include "parsing.hpp"

// Handle requests
void handle_client(SSL *ssl)
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

    if (tokens.size() > 2)
    {
        const std::string &method = tokens[0];
        if (method == "get")
            response = GET_handler(tokens); // Assuming GET_handler is defined elsewhere
        else if (method == "post")
            response = POST_handler(tokens); // Assuming POST_handler is defined elsewhere
        else
            response = NOT_IMPLEMENTED; // Assuming NOT_IMPLEMENTED is defined elsewhere

        if (response.empty())
        {
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
    }

    SSL_write(ssl, response.c_str(), response.length());

    // Bi-directional shutdown (more robust)
    int sd = SSL_get_fd(ssl);
    if (sd != -1)
    {                      // Check if the socket is valid
        SSL_shutdown(ssl); // Send and receive close_notify
        close(sd);
    }
    SSL_free(ssl);
}