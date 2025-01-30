# Simplest Multithread REST-like API (over TCP)
This repository contains a simple example of a multithreaded REST-like API server implemented in C++. It uses TCP sockets for communication and provides basic GET and POST request handling.  This is a simplified example for educational purposes and demonstrates core concepts.
|                                        |                                        |                                        |
|:--------------------------------------:|:--------------------------------------:|:--------------------------------------:|
|![unnamed](https://github.com/user-attachments/assets/e3fc0b85-0817-49c3-af4e-c4c1b11cfb75)|![unnamed](https://github.com/user-attachments/assets/e3fc0b85-0817-49c3-af4e-c4c1b11cfb75)|![unnamed](https://github.com/user-attachments/assets/e3fc0b85-0817-49c3-af4e-c4c1b11cfb75)|

## Features

* **Multithreading:** Handles multiple client connections concurrently using a thread pool (`thread_pools.hpp`).  This allows the server to remain responsive even under load.
* **Basic REST-like API:** Supports GET and POST requests with simple routing based on URL paths.
* **Simple Request Parsing:**  Parses incoming requests to identify the HTTP method and requested resource.
* **Example Handlers:** Includes example handlers for GET requests (e.g., `/hello`, `/add`, `/json`, `/help`) and POST requests (e.g., `/data`).  These examples demonstrate how to extract data from the request and construct responses.
* **Error Handling:** Basic error handling for invalid requests and data conversion issues.
* **Clear and Concise Code:** The code is designed to be easy to understand and modify.

## Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Usage](#usage)
- [Endpoints](#endpoints)
- [License](#license)

## Overview

This is a simple C++ project that implements a multithreaded REST-like API over TCP. 
The server can handle GET and POST requests, demonstrating basic request handling and response generation.
Handels multithreading for responces
C++

## Features

- Multithreaded server using a thread pool
- Simple GET and POST request handling
- Basic string and data processing
- JSON response example

## Prerequisites
- C++ compiler
- CMake (optional)

## Installation
1. Clone the repository
2. Build

## Usage
To start the server, run the following command in the build directory:

bash
./http_server
The server will start listening on port 8080.

## Endpoints

### GET /add/a/b
Adds two numbers a+b. Example:

http://localhost:8080/add/5/10

### GET /hello/number
Returns a message based on the number of beers. Example:
http://localhost:8080/hello/5

### GET /json
Returns a JSON response. Example:

http://localhost:8080/json

### GET / or /help
Lists available endpoints. Example:

http://localhost:8080/

### GET /stop
Stops server remotely. Example:

### http://localhost:8080/

### POST /data
Receives data in the body of the request. Example:
```
curl -d '{"name":"Bilya","age":24}' -H "Content-Type: application/json" -X POST http://localhost:8080/data
```
## License
This project is licensed under the MIT License - see the LICENSE file for details.

## Author
(C) Anatoly Mazkun, 2025
