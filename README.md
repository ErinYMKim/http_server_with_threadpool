# Simple Multithreaded HTTP Server and Client

This repository contains a simple multithreaded HTTP server and client written in C.

## Overview

The server is set up with a specified number of threads that continually check a job queue for new jobs. These jobs are added to the queue when the server receives a new HTTP request from a client. Each thread in the server will handle one HTTP request in the order they were received. When the job (request) is handled, a file path is parsed from the GET request, a file at this path is read and its content is sent back to the client as an HTTP response.

The client sends an HTTP GET request asking for '/index.html' to the server upon execution and then print out the server's response.

Please note that the server code does not provide protections against directory traversal attacks or other similar vulnerabilities. It is meant for educational purposes and should not be used in a production environment.

## Structure

- `http_server.c`: This is where the main server functionality is implemented.
- `http_client.c`: This contains a simple client that is able to send HTTP GET requests and print server's response.
- `thread_pool.c`/`thread_pool.h`: These files contain the code relating to the thread pool used by the server.
- `index.html`: A simple test HTML page that the server can serve.
- `Makefile`: A simple Makefile to compile the server, client, and tests.

## Usage

First, compile all the required files by running `make`. This will create the executables "server", "client", and "tests".

You can then run the server by calling `./server`. This will start the server, which will be ready to accept incoming connections and serve the test index.html file.

You can run the client by calling `./client <ip> <port>`, where `<ip>` is the IP address where the server is running and `<port>` is the port number (server is set to run on port 8080 by default), to ask for '/index.html' from the server and print out server's response.

## Requirements

This code was written to be compatible with any system that has a GCC-like compiler and the pthread library installed. The tests require the check library to be installed.

Please note that this code does not fully respect HTTP protocol (e.g. it only understands simple GET requests and the server does not handle errors as it should according to the HTTP spec). It is meant to demonstrate basic socket programming and multithreading, not to be used as an actual production-ready HTTP server.