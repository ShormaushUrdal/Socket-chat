# Centralized Multi-Client Chat Server

## Introduction
This project is a modular, multi-client chat server written in C, designed for local area networks (LAN). It features:
- **TCP chat server** supporting multiple clients
- **UDP server discovery** (automatic LAN broadcast)
- **Modular codebase** (discovery, chat, network utilities, authentication)
- **Doxygen documentation** for easy code understanding

---

## File & Module Overview
- `main.c` — Entry point; initializes modules, handles main server loop
- `discovery.c/.h` — UDP discovery logic (server broadcasts its presence)
- `chat.c/.h` — Chat logic and client management (TCP server, message routing)
- `network_utils.c/.h` — Network utility functions (address formatting, helpers)
- `auth.c/.h` — User authentication (currently not integrated)

---

## Getting Started
### Prerequisites
- GCC (or any C99-compatible compiler)
- Linux or WSL recommended (uses POSIX sockets)
- [Optional] Doxygen for documentation

### Compilation
```sh
gcc main.c discovery.c chat.c network_utils.c auth.c -o chat_server
```

### Running the Server
```sh
./chat_server
```
- The server listens on **TCP port 8888** for chat clients.
- It broadcasts its presence on **UDP port 8889** for discovery.

---

## How It Works
### 1. Server Discovery (UDP)
- The server periodically broadcasts a message (`CHAT_SERVER_HERE`) on UDP port 8889 to `255.255.255.255`.
- Clients can listen for this broadcast to auto-detect the server's IP/port on the LAN.

### 2. Client Connection
- All connected clients can send and receive chat messages.
- They can view the messages sent by others.

### 3. Chat Flow (TCP)
- Connected clients can send messages to the server.
- The server broadcasts each message to all other connected clients.
- Connect/disconnect events are announced to all users.

---

## Example Usage
### Starting the Server
```sh
./chat_server
```

### Connecting as a Client (Example with netcat)
```sh
nc <server-ip> 8888
```
- Enter any username and password when prompted (these are not checked).
- Type messages and see them broadcast to all connected users.

### Discovering the Server (Example with netcat)
```sh
nc -ul 8889
```
- Listen for `CHAT_SERVER_HERE` broadcasts to find the server's IP.

---

## Extending the Project
- **Authentication:** Implement real credential checks in `auth.c` if needed.
- **Client Application:** Write a custom client for better UX.
- **Increase MAX_CLIENTS:** Edit `MAX_CLIENTS` in `chat.h` and `main.c`.
- **Message History, Private Messaging, etc.:** Add features in `chat.c`.

---

## Doxygen Documentation
To generate HTML docs:
```sh
doxygen -g   # (first time only, creates Doxyfile)
doxygen      # Generates docs in ./html/
```
Open `html/index.html` in your browser.

---

## Troubleshooting & Common Issues
- **Port already in use:** Make sure no other process is using TCP 8888 or UDP 8889.
- **Firewall issues:** Allow inbound connections on these ports.
- **Authentication always succeeds:** Edit `auth.c` to implement real checks if desired.
- **Windows support:** Use WSL or adapt socket code for Windows.

---