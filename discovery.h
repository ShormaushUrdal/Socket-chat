/**
 * @file discovery.h
 * @brief UDP discovery module for chat server.
 *
 * Provides functions for broadcasting server presence via UDP.
 */
#ifndef DISCOVERY_H
#define DISCOVERY_H

#include <netinet/in.h>

/**
 * @brief Set up the UDP discovery socket.
 * @param broadcast_addr Pointer to sockaddr_in struct to be filled with broadcast address info.
 * @return Discovery socket file descriptor.
 */
int setup_udp_discovery(struct sockaddr_in *broadcast_addr);

/**
 * @brief Broadcast the UDP discovery message.
 * @param discovery_socket UDP socket file descriptor.
 * @param broadcast_addr Pointer to broadcast address struct.
 */
void broadcast_discovery(int discovery_socket, struct sockaddr_in *broadcast_addr);

#endif // DISCOVERY_H 