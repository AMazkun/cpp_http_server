#include <regex>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>
#include "parsing.hpp"

std::string time_stamp()
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    struct tm tm;
    gmtime_r(&in_time_t, &tm); // Use gmtime_r for UTC

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;

    std::stringstream ss;
    ss << std::put_time(&tm, "[%Y-%m-%d %H:%M:%S") << "." << std::setfill('0') << std::setw(3) << ms << "]";
    return ss.str();
}

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
