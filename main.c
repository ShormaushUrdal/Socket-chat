/**
 * @file main.c
 * @brief Entry point for the modularized multi-client chat server with UDP discovery and authentication.
 */
#include "discovery.h"
#include "chat.h"
#include "network_utils.h"
#include "auth.h"
#include <stdio.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_CLIENTS 500
#define USERNAME_MAX_LEN 32

static int client_socket[MAX_CLIENTS] = {0};
static char client_usernames[MAX_CLIENTS][USERNAME_MAX_LEN] = {{0}};
static int num_clients = 0;
static int running = 1;

/**
 * @brief Signal handler for SIGINT to cleanly announce server shutdown.
 */
void handle_sigint(int sig) {
    printf("\nServer shutting down. Notifying clients...\n");
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_socket[i] > 0) {
            char msg[128];
            snprintf(msg, sizeof(msg), "Server is shutting down. Goodbye, %s!\n", client_usernames[i][0] ? client_usernames[i] : "client");
            send(client_socket[i], msg, strlen(msg), 0);
            close(client_socket[i]);
        }
    }
    running = 0;
}

/**
 * @brief Main server loop: handles new connections, authentication, chat, and discovery.
 */
void server_loop(int master_socket, int discovery_socket, struct sockaddr_in *address, struct sockaddr_in *broadcast_addr) {
    fd_set readfds;
    struct timeval timeout;
    puts("Waiting for connections ...");
    while (running) {
        FD_ZERO(&readfds);
        FD_SET(master_socket, &readfds);
        int max_sd = master_socket;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_socket[i] > 0) {
                FD_SET(client_socket[i], &readfds);
                if (client_socket[i] > max_sd) {
                    max_sd = client_socket[i];
                }
            }
        }
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        int activity = select(max_sd + 1, &readfds, NULL, NULL, &timeout);
        if ((activity < 0) && (errno != EINTR)) {
            printf("select error\n");
        }
        if (FD_ISSET(master_socket, &readfds)) {
            int addrlen = sizeof(*address);
            int new_socket = accept(master_socket, (struct sockaddr *)address, (socklen_t *)&addrlen);
            if (new_socket >= 0) {
                char username[USERNAME_MAX_LEN] = {0};
                if (num_clients < MAX_CLIENTS && authenticate_user(new_socket, username)) {
                    for (int i = 0; i < MAX_CLIENTS; i++) {
                        if (client_socket[i] == 0) {
                            client_socket[i] = new_socket;
                            strncpy(client_usernames[i], username, USERNAME_MAX_LEN);
                            num_clients++;
                            char join_msg[128];
                            snprintf(join_msg, sizeof(join_msg), "User '%s' has joined the chat.\n", username);
                            printf("%s", join_msg);
                            // Notify all other clients
                            for (int j = 0; j < MAX_CLIENTS; j++) {
                                if (client_socket[j] > 0 && j != i) {
                                    send(client_socket[j], join_msg, strlen(join_msg), 0);
                                }
                            }
                            break;
                        }
                    }
                } else {
                    const char *fail_msg = "Authentication failed or server full. Connection closed.\n";
                    send(new_socket, fail_msg, strlen(fail_msg), 0);
                    close(new_socket);
                }
            }
        }
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int sd = client_socket[i];
            if (sd > 0 && FD_ISSET(sd, &readfds)) {
                char buffer[BUFFER_SIZE + 1];
                int valread = recv(sd, buffer, BUFFER_SIZE, MSG_DONTWAIT);
                if (valread == 0) {
                    // Client disconnected
                    char leave_msg[128];
                    snprintf(leave_msg, sizeof(leave_msg), "User '%s' has left the chat.\n", client_usernames[i][0] ? client_usernames[i] : "client");
                    printf("%s", leave_msg);
                    // Notify all other clients
                    for (int j = 0; j < MAX_CLIENTS; j++) {
                        if (client_socket[j] > 0 && j != i) {
                            send(client_socket[j], leave_msg, strlen(leave_msg), 0);
                        }
                    }
                    close(sd);
                    client_socket[i] = 0;
                    client_usernames[i][0] = '\0';
                    num_clients--;
                } else if (valread > 0) {
                    buffer[valread] = '\0';
                    if (buffer[strlen(buffer) - 1] == '\n') buffer[strlen(buffer) - 1] = '\0';
                    char msg[BUFFER_SIZE + USERNAME_MAX_LEN + 16];
                    snprintf(msg, sizeof(msg), "%s: %s\n", client_usernames[i][0] ? client_usernames[i] : "client", buffer);
                    // Broadcast to all other clients
                    for (int j = 0; j < MAX_CLIENTS; j++) {
                        if (client_socket[j] > 0 && j != i) {
                            send(client_socket[j], msg, strlen(msg), 0);
                        }
                    }
                    printf("%s", msg);
                }
            }
        }
        if (activity == 0) {
            broadcast_discovery(discovery_socket, broadcast_addr);
        }
    }
}

int main(int argc, char *argv[]) {
    signal(SIGINT, handle_sigint);
    struct sockaddr_in address, broadcast_addr;
    int master_socket = setup_tcp_server(&address);
    int discovery_socket = setup_udp_discovery(&broadcast_addr);
    server_loop(master_socket, discovery_socket, &address, &broadcast_addr);
    printf("Server exited.\n");
    return 0;
} 