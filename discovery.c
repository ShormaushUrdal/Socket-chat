/**
 * @file discovery.c
 * @brief UDP discovery module implementation for chat server.
 */
#include "discovery.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define DISCOVERY_PORT 8889
#define DISCOVERY_MSG "CHAT_SERVER_HERE"

int setup_udp_discovery(struct sockaddr_in *broadcast_addr) {
    int opt = 1;
    int discovery_socket;
    if ((discovery_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("UDP socket failed");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(discovery_socket, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt)) < 0) {
        perror("UDP setsockopt broadcast failed");
        close(discovery_socket);
        exit(EXIT_FAILURE);
    }
    memset(broadcast_addr, 0, sizeof(*broadcast_addr));
    broadcast_addr->sin_family = AF_INET;
    broadcast_addr->sin_port = htons(DISCOVERY_PORT);
    broadcast_addr->sin_addr.s_addr = inet_addr("255.255.255.255");
    printf("Broadcasting presence on UDP port %d\n", DISCOVERY_PORT);
    return discovery_socket;
}

void broadcast_discovery(int discovery_socket, struct sockaddr_in *broadcast_addr) {
    sendto(discovery_socket, DISCOVERY_MSG, strlen(DISCOVERY_MSG), 0, (struct sockaddr *)broadcast_addr, sizeof(*broadcast_addr));
} 