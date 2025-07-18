/**
 * @file network_utils.c
 * @brief Network utility functions implementation for chat server.
 */
#include "network_utils.h"
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void get_client_address(int sockfd, char *addr_buf) {
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    if (getpeername(sockfd, (struct sockaddr *)&addr, &addr_len) < 0) {
        perror("getpeername");
        strcpy(addr_buf, "unknown");
        return;
    }
    sprintf(addr_buf, "%s:%d", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
} 