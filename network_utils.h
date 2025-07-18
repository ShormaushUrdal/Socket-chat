/**
 * @file network_utils.h
 * @brief Network utility functions for chat server.
 *
 * Provides helper functions for network address formatting and related tasks.
 */
#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

/**
 * @brief Get the client address as a string.
 * @param sockfd Socket file descriptor.
 * @param addr_buf Buffer to store the address string.
 */
void get_client_address(int sockfd, char *addr_buf);

#endif // NETWORK_UTILS_H 