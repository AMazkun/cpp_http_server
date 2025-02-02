# Simplest Multithread REST-like API (over TCP)

This repository provides a simple, educational example of a multithreaded REST-like API server implemented in C++. It uses TCP sockets for communication and demonstrates core concepts of handling GET and POST requests concurrently.  The repository includes both HTTP and HTTPS server implementations.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
- [Usage](#usage)
- [Endpoints](#endpoints)
- [HTTP Server](#http-server)
- [HTTPS Server](#https-server)
- [Installation](#installation)
- [Usage](#usage)
- [License](#license)
- [Author](#author)

## Overview

This project implements a basic multithreaded REST-like API server in C++ over TCP. It provides examples of handling GET and POST requests, basic routing, and concurrent connection handling using a thread pool.  The project is structured with separate directories for HTTP and HTTPS server implementations.

## Features

* **Multithreading:** Handles multiple client connections concurrently using a thread pool (`thread_pools.hpp`). This allows the server to remain responsive under load.
* **REST-like API:** Supports GET and POST requests with simple routing based on URL paths.
* **Request Parsing:** Parses incoming requests to identify the HTTP method and requested resource.
* **Example Handlers:** Includes example handlers for various GET and POST requests, demonstrating data extraction and response construction.
* **Error Handling:** Basic error handling for invalid requests and data conversion issues.
* **Clear and Concise Code:** The code is designed for educational purposes and emphasizes clarity and ease of understanding.
* **HTTP and HTTPS:**  Implements both HTTP (unencrypted) and HTTPS (encrypted) servers.
* **HTTPS:**  Log requests into files, files spleed if exided max size.

## Getting Started

### Prerequisites
* C++ compiler (e.g., g++, clang++)
* CMake (optional)
* OpenSSL library (required for the HTTPS server)

## HTTP Server
The HTTP server provides unencrypted communication.  It's simpler to set up and is suitable for development or situations where security is not a primary concern.

### Features
1. Multithreaded server using a thread pool
2. Simple GET and POST request handling
3. Basic string and data processing
4. JSON response example


## HTTPS Server
The HTTPS server example in the directory HTTPS builds upon the HTTP server by adding support for SSL/TLS encryption using OpenSSL. This ensures secure communication between clients and the server.

The HTTPS server uses OpenSSL to provide SSL/TLS encryption, ensuring secure communication. It's essential for production environments or when dealing with sensitive data. You'll need to have OpenSSL installed and configured correctly to build and run the HTTPS server.

### Generating SSL Certificates (for Testing)
For testing purposes, you can generate self-signed SSL certificates.  **Do not use self-signed certificates in production.**  They are not trusted by browsers and other clients.  Use a certificate authority (CA) to obtain proper certificates for production deployment.
Here's how to generate a self-signed certificate using OpenSSL:
```bash
openssl req -x509 -newkey rsa:4096 -nodes -keyout server.key -out server.crt -days 365
```
Manually Generate a Certificate Signing Request (CSR) Using OpenSSL - SSL.com
- x509: This option tells OpenSSL to create a self-signed certificate (instead of a certificate signing request).
- newkey rsa:4096: This generates a new RSA key with a 4096-bit key size (you can adjust this). The key is stored in server.key.
- nodes: This option tells OpenSSL not to encrypt the private key (for simplicity in this example). In production, you should always encrypt your private key.
- keyout server.key: Specifies the filename for the private key.
- out server.crt: Specifies the filename for the certificate.
- days 365: Specifies the validity period of the certificate (365 days in this example).
You'll be prompted to enter information about the certificate (e.g., country, organization, common name).  The "Common Name" is important; it should match the hostname or IP address of your server.

Important:  These server.key and server.crt files will be used by your HTTPS server.  Make sure the server code is configured to load these files.  The example code in this repository assumes the key and certificate files are named server.key and server.crt and are located in the same directory as the executable.  You may need to adjust the paths in the code if your files are in a different location.

Obtaining Certificates from a CA (for Production)
For production use, you must obtain certificates from a trusted Certificate Authority (CA) like Let's Encrypt, Comodo, or DigiCert.  The process varies depending on the CA, but generally involves:

### Features
1. SSL/TLS Encryption: Utilizes OpenSSL to provide secure communication.
2. Multithreading: Handles multiple client connections concurrently using a thread pool.
3. Basic REST-like API: Supports GET and POST requests with simple routing based on URL paths.
4. Simple Request Parsing: Parses incoming requests to identify the HTTP method and requested resource.
5. Example Handlers: Includes example handlers for GET requests and POST requests.
6. Log requests into files, files spleed if exided max size.

## Prerequisites
- C++ compiler
- OpenSSL library
- CMake (optional)

# Installation
1. Clone the repository
2. Build servers in folders HTTP and HTTPS

# Usage
To start the servers, run the following command in the build directory:
```
bash
./http_server
./https_server_main
```
The server will start listening on port 8080 or 8443.

# Endpoints

### GET /add/a/b
Adds two numbers. Example:

http://localhost:8080/add/5/10

### GET /hello/number
Returns a message based on the number of beers. Example:
http://localhost:8080/hello/5

### GET /json
Returns a JSON response. Example:

http(s)://localhost:8080/json

### GET / or /help
Lists available endpoints. Example:

http(s)://localhost:8080/

### GET /stop
Stops server remotely. Example:

### http://localhost:8080/

### POST /data
Receives data in the body of the request. Example:

curl -d '{"name":"Bilya","age":24}' -H "Content-Type: application/json" -X POST http://localhost:8080/data

## License
This project is licensed under the MIT License - see the LICENSE file for details.

## Author
(C) Anatoly Mazkun, 2025
