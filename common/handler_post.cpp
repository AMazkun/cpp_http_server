#include "handlers.hpp"

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
