# Client-Server File Transfer Protocol System

## Overview
This project implements a client-server file transfer system using sockets in C++. The system allows users to interact with the server to perform various file operations such as uploading files (`put`), downloading files (`get`), creating directories (`mkdir`), changing directories (`cd`), deleting files (`delete`), listing directory contents (`ls`), and checking the current directory (`pwd`).


## Prerequisites
- C++ compiler
- Linux environment (for socket programming)
- Basic understanding of client-server architecture and socket programming concepts

## Usage
1. Compile the server and client programs using a C++ compiler.
2. Run the server program on a host machine by giving port number.
3. Run the client program on another machine and connect it to the server using the server's IP address and port number.
4. Follow the prompts on the client-side to interact with the server and perform file operations.

## File Structure
- **Server.cpp:** Contains the implementation of the server-side logic.
- **Client.cpp:** Contains the implementation of the client-side logic.
- **README.md:** This file, providing an overview of the project, its features, prerequisites, usage instructions, and file structure.


## Contributors
- Yash Joshi
- Aditya Malode
