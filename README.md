# cpp

# WebSocket Applications

This repository contains two applications: a **Chat Server** and an **Echo Server**, both implemented using the `libwebsockets` library in C++. These applications showcase basic WebSocket server functionalities: one for chat broadcasting and the other for message echoing.

## 1. Chat Server Application

### Description
A simple WebSocket-based chat server using the `libwebsockets` library. The server allows multiple clients to connect and broadcast messages to all other connected clients in real-time, simulating a chat room.

### Features
- Real-time communication between multiple clients.
- Messages sent by one client are broadcast to all other clients.
- Simple management of client connections and graceful shutdown using signal handling (Ctrl+C).

### Requirements
- C++ compiler (e.g., GCC).
- `libwebsockets` library installed on your system.

### Installation and Setup

1. Clone the repository:
   ```bash
   git clone git@github.com:DevikaBoddu13/cpp.git
   cd websocket-chat-server
2. Install the libwebsockets library:
   ```bash
   sudo apt-get install libwebsockets-dev
3. Compilation & Execution
   ```bash
   g++ -std=c++11 -o chatServer chatServer.cpp -lwebsockets
   ./chatServer

