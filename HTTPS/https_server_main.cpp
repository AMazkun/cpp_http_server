#define PORT 8443
#define MAX_THREADS 5 // Maximum number of worker threads

#include <iostream>
#include <csignal> // For signal handling
#include <filesystem>

#include "https_server.hpp"

// Signal handler for graceful shutdown (Ctrl+C)
void signal_handler(int signum)
{
    if (signum == SIGINT)
    {
        std::cout << time_stamp() << " Shutting down server..." << std::endl;
        running = false; // Set the flag to stop the server loop
    }
}

int signal_handler_setup()
{
    // Set up signal handler for Ctrl+C
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror((time_stamp() + " Set signal action failed.").c_str()); // Print error if setting up signal handler fails
        return 1;                                                      // Exit with error code
    }
    return 0;
}

int main(int argc, char *argv[])
{
    int state = 0;
    std::cout << time_stamp() << " UTC time mentioned further. Server initializing." << std::endl; // Server initialization message
    if ((state = signal_handler_setup()) != 0)
        return state;

    auto server = new HTTPS_SERVER(PORT, MAX_THREADS, std::filesystem::current_path()); // Create a new HTTPS server instance

    if ((state = server->open()) == 0)
        server->run(); // Start the server if it opens successfully

    return state; // Return the final state (0 if success, other if failure)
}
