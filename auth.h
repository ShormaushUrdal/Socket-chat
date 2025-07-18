/**
 * @file auth.h
 * @brief User authentication module for chat server.
 *
 * Provides functions for authenticating users with username and password.
 */
#ifndef AUTH_H
#define AUTH_H

#define USERNAME_MAX_LEN 32
#define PASSWORD_MAX_LEN 32

/**
 * @brief Authenticate a user by username and password.
 * @param sockfd Client socket file descriptor.
 * @param username Buffer to store the authenticated username.
 * @return 1 if authentication succeeds, 0 otherwise.
 */
int authenticate_user(int sockfd, char *username);

#endif // AUTH_H 