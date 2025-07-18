/**
 * @file auth.c
 * @brief User authentication module implementation for chat server.
 */
#include "auth.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int authenticate_user(int sockfd, char *username) {
    // TODO: Implement real authentication logic
    char buffer[USERNAME_MAX_LEN + PASSWORD_MAX_LEN + 32];
    // Prompt for username
    const char *ask_user = "Enter username: ";
    send(sockfd, ask_user, strlen(ask_user), 0);
    int len = recv(sockfd, buffer, USERNAME_MAX_LEN, 0);
    if (len <= 0) return 0;
    buffer[len] = '\0';
    strncpy(username, buffer, USERNAME_MAX_LEN);
    username[USERNAME_MAX_LEN - 1] = '\0';
    // Prompt for password
    const char *ask_pass = "Enter password: ";
    send(sockfd, ask_pass, strlen(ask_pass), 0);
    len = recv(sockfd, buffer, PASSWORD_MAX_LEN, 0);
    if (len <= 0) return 0;
    buffer[len] = '\0';
    // For now, always succeed
    return 1;
} 