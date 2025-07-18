/**
 * @file chat.c
 * @brief Chat logic and client management implementation for chat server.
 */
#include "chat.h"
#include "network_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int setup_tcp_server(struct sockaddr_in *address) {
    int opt = 1;
    int master_socket;
    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("TCP socket failed");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) {
        perror("TCP setsockopt");
        exit(EXIT_FAILURE);
    }
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(8888);
    if (bind(master_socket, (struct sockaddr *)address, sizeof(*address)) < 0) {
        perror("TCP bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(master_socket, MAX_CLIENTS) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("Chat server listening on TCP port %d\n", 8888);
    return master_socket;
}

int accept_new_client(int master_socket, struct sockaddr_in *address, int *client_socket, int *num_clients) {
    int addrlen = sizeof(*address);
    int new_socket = accept(master_socket, (struct sockaddr *)address, (socklen_t *)&addrlen);
    if (new_socket < 0) {
        perror("accept");
        return -1;
    }
    char client_addr_str[30];
    get_client_address(new_socket, client_addr_str);
    printf("Server: Received a new connection from client %s\n", client_addr_str);
    if (*num_clients < MAX_CLIENTS) {
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_socket[i] == 0) {
                client_socket[i] = new_socket;
                (*num_clients)++;
                break;
            }
        }
    } else {
        printf("Max clients reached. Connection from %s rejected.\n", client_addr_str);
        send(new_socket, "Server is full. Try again later.\n", 33, 0);
        close(new_socket);
    }
    return new_socket;
}

void handle_client_messages(int *client_socket, int *num_clients) {
    char buffer[BUFFER_SIZE + 1];
    for (int i = 0; i < MAX_CLIENTS; i++) {
        int sd = client_socket[i];
        if (sd > 0) {
            int valread = recv(sd, buffer, BUFFER_SIZE, MSG_DONTWAIT);
            if (valread == 0) {
                char client_addr[30];
                get_client_address(sd, client_addr);
                printf("Client %s disconnected\n", client_addr);
                close(sd);
                client_socket[i] = 0;
                (*num_clients)--;
            } else if (valread > 0) {
                buffer[valread] = '\0';
                char sender_addr[30];
                get_client_address(sd, sender_addr);
                if (buffer[strlen(buffer) - 1] == '\n') buffer[strlen(buffer) - 1] = '\0';
                printf("Server: Received message \"%s\" from client %s\n", buffer, sender_addr);
                if (*num_clients < 2) {
                    printf("Server: Insufficient clients, \"%s\" from client %s dropped\n", buffer, sender_addr);
                } else {
                    char broadcast_msg[BUFFER_SIZE + 50];
                    snprintf(broadcast_msg, sizeof(broadcast_msg), "%s %s", sender_addr, buffer);
                    for (int j = 0; j < MAX_CLIENTS; j++) {
                        int dest_sd = client_socket[j];
                        if (dest_sd > 0 && dest_sd != sd) {
                            send(dest_sd, broadcast_msg, strlen(broadcast_msg), 0);
                            char recipient_addr[30];
                            get_client_address(dest_sd, recipient_addr);
                            printf("Server: Send message \"%s\" from client %s to %s\n", buffer, sender_addr, recipient_addr);
                        }
                    }
                }
            }
        }
    }
} 