#include "handlers.hpp"

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
        return RESPONSE_STUB + request[2] + " + " + request[3] + " = " + std::to_string(sum);
    }
    else if (cmd == "hello")
    {
        int beer = 0;
        try
        {
            beer = std::stoi(request[2]);
            if (0 < beer && beer < 24)
            {
                return RESPONSE_STUB + "Beer delivery of " + std::to_string(beer) + " bottle(s)";
            }
        }
        catch (const std::exception &e)
        {
            return RESPONSE_STUB + "Hello world!";
        }
        return RESPONSE_STUB + "Unsuccessful application.";
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
