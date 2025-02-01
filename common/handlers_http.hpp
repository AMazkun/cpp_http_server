#pragma once

#include <string>
#include <vector>

#define BUFFER_SIZE 4096
const std::string NOT_IMPLEMENTED = "HTTP/1.1 501 Not Implemented\r\nContent-Type: text/html\r\n\r\n<html><body><h1>501 Not Implemented</h1></body></html>";
const std::string RESPONSE_STUB = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";

// Handle requests
std::string GET_handler(const std::vector<std::string> &request);
std::string POST_handler(const std::vector<std::string> &request);
