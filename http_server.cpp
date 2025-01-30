// Simplest Multithread REST-like API (over TCP) example
// (C) Anatoly Mazkun, buy me a beer, 2025

#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <regex>

#include "thread_pools.hpp"

#define MAX_THREADS 5 // Maximum number of worker threads
#define PORT 8080
#define BUFFER_SIZE 4096

// Convert a string to lowercase
std::string str_tolower(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

// Check if a string is not blank
bool not_blank(const std::string &s)
{
    return s.find_first_not_of(' ') != std::string::npos;
}

// Split string by delimiters
std::vector<std::string> split(const std::string &s)
{
    std::regex re("(GET|POST|/| |add|HTTP)");
    std::sregex_token_iterator iter(s.begin(), s.end(), re, {-1, 0});
    std::sregex_token_iterator end;

    std::vector<std::string> tokens;
    while (iter != end)
    {
        if (not_blank(*iter) && *iter != "/")
            tokens.push_back(str_tolower(*iter));
        iter++;
    }
    return tokens;
}

const std::string NOT_IMPLEMENTED = "HTTP/1.1 501 Not Implemented\r\nContent-Type: text/html\r\n\r\n<html><body><h1>501 Not Implemented</h1></body></html>";
const std::string RESPONSE_STUB = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";

// Handle GET requests
std::string GET_handler(const std::vector<std::string> &request)
{
    const std::string &cmd = request[1];
    if (cmd == "add")
    {
        int sum = 0;
        try
        {
            sum += std::stoi(request[2]);
            sum += std::stoi(request[3]);
        }
        catch (const std::exception &e)
        {
            return RESPONSE_STUB + e.what();
        }
        return request[2] + " + " + request[3] + " = " + std::to_string(sum);
    }
    else if (cmd == "hello")
    {
        int beer = 0;
        try
        {
            beer = std::stoi(request[2]);
            if (0 < beer && beer < 24)
            {
                return "Beer delivery of " + std::to_string(beer) + " bottle(s)";
            }
        }
        catch (const std::exception &e)
        {
            return RESPONSE_STUB + "Hello world!";
        }
        return "Unsuccessful application.";
    }
    else if (cmd == "stop")
    {
        return "";
    }
    else if (cmd == "json")
    {
        // Example data handling (replace with your logic)
        return "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"name\": \"Example Data\", \"value\": 42}";
    }
    else if (cmd == "http" || cmd == "help")
    {
        // Example data handling (replace with your logic)
        return RESPONSE_STUB + "Endpoints:\n\t/hello\n\t/hello/<number>\n\t/data - POST {\"name\":\"Bilya\",\"age\":24}\n\t/lucky\n\t/json\n\t/add/<a>/<b>\n\t/stop";
    }
    return NOT_IMPLEMENTED;
}

// Handle POST requests
std::string POST_handler(const std::vector<std::string> &request)
{
    const std::string &cmd = request[1];
    if (cmd == "data")
    {
        // Example POST handling (extract data from request body - more complex in real app)
        // curl -d '{"name":"Bilya","age":24}' {ip}:8080/data
        std::string response = RESPONSE_STUB;
        for (const auto &item : request)
        {
            response += item + "\n";
        }
        return RESPONSE_STUB + "Data Received (Simplified)\n" + response;
    }
    return NOT_IMPLEMENTED;
}

// Function to handle a single client connection
void handle_client(int client_socket, int server_socket)
{
    char buffer[BUFFER_SIZE];
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
    buffer[bytes_received] = '\0'; // Null-terminate the received data

    // Basic request parsing (very simplified)
    std::string request(buffer);
    std::string response;
    std::vector<std::string> tokens = split(request);

    if (tokens.size() > 2)
    {
        const std::string &method = tokens[0];
        if (method == "get")
            response = GET_handler(tokens);
        else if (method == "post")
            response = POST_handler(tokens);
        else
            response = NOT_IMPLEMENTED;

        if (response.empty())
        {
            close(client_socket);
            close(server_socket);
        }
    }
    else
    {
        response = NOT_IMPLEMENTED;
    }

    send(client_socket, response.c_str(), response.length(), 0);
}

// Function to start the server
int start_server()
{
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_len;

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        perror("Socket creation failed");
        return 1;
    }

    // Prepare server address
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY; // Listen on all available interfaces
    server_address.sin_port = htons(PORT);       // Port number

    // Bind socket to address
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("Binding failed");
        return 2;
    }

    // Listen for connections
    if (listen(server_socket, 5) < 0)
    { // 5 is the backlog (max queued connections)
        perror("Listening failed");
        return 3;
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    ThreadPool pool(MAX_THREADS);

    while (true)
    {
        client_address_len = sizeof(client_address);
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
        if (client_socket < 0)
        {
            perror("Accepting connection failed");
            return 13; // Or handle the error as needed
        }

        std::cout << "Client connected: " << client_socket << std::endl;

        // Handle client in a separate function (or thread/process for concurrency)
        pool.enqueue([client_socket, server_socket]()
                     {
            handle_client(client_socket, server_socket);
            close(client_socket); // Close the socket after handling
            std::cout << "Client disconnected: " << client_socket << std::endl; });
    }

    close(server_socket); // Server socket is usually kept open, but close it if needed
    return 0;
}

int main()
{
    return start_server();
}
