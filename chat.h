/**
 * @file chat.h
 * @brief Chat logic and client management for multi-client chat server.
 *
 * Provides functions for handling TCP chat connections and messages.
 */
#ifndef CHAT_H
#define CHAT_H

#include <netinet/in.h>

#define MAX_CLIENTS 500
#define BUFFER_SIZE 1024

/**
 * @brief Set up the TCP server socket.
 * @param address Pointer to sockaddr_in struct to be filled with server address info.
 * @return TCP server socket file descriptor.
 */
int setup_tcp_server(struct sockaddr_in *address);

/**
 * @brief Accept a new TCP client connection.
 * @param master_socket TCP server socket file descriptor.
 * @param address Pointer to sockaddr_in struct for client address.
 * @param client_socket Array of client socket file descriptors.
 * @param num_clients Pointer to number of connected clients.
 * @return New client socket file descriptor.
 */
int accept_new_client(int master_socket, struct sockaddr_in *address, int *client_socket, int *num_clients);

/**
 * @brief Handle TCP messages from clients.
 * @param client_socket Array of client socket file descriptors.
 * @param num_clients Pointer to number of connected clients.
 */
void handle_client_messages(int *client_socket, int *num_clients);

#endif // CHAT_H 