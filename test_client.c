/**
 * @file test_client.c
 * @brief Test client that connects to server, sends messages, and measures response time
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <unistd.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <arpa/inet.h>
 #include <pthread.h>
 #include <time.h>
 #include <sys/time.h>
 #include <errno.h>
 
 #define TCP_PORT 8888
 #define DISCOVERY_PORT 8889
 #define BUFFER_SIZE 1024
 #define DISCOVERY_MSG "CHAT_SERVER_HERE"
 #define MAX_CLIENTS 100
 
 typedef struct {
     int client_id;
     double connection_time;
     double message_send_time;
     double response_time;
     int messages_sent;
     int messages_received;
     int success;
 } client_stats_t;
 
 // Global statistics
 client_stats_t client_stats[MAX_CLIENTS];
 pthread_mutex_t stats_mutex = PTHREAD_MUTEX_INITIALIZER;
 
 // Get current time in milliseconds
 double get_time_ms() {
     struct timeval tv;
     gettimeofday(&tv, NULL);
     return (tv.tv_sec * 1000.0) + (tv.tv_usec / 1000.0);
 }
 
 // Discover server (simplified - directly connect to localhost)
 int discover_and_connect_server() {
     int sock;
     struct sockaddr_in serv_addr;
     
     if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
         return -1;
     }
     
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_port = htons(TCP_PORT);
     serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
     
     if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
         close(sock);
         return -1;
     }
     
     return sock;
 }
 
 // Client thread function
 void* client_thread(void* arg) {
     int client_id = *(int*)arg;
     client_stats_t* stats = &client_stats[client_id];
     
     double start_time = get_time_ms();
     
     // Connect to server
     int sock = discover_and_connect_server();
     if (sock < 0) {
         stats->success = 0;
         return NULL;
     }
     
     stats->connection_time = get_time_ms() - start_time;
     
     // Send test messages
     char message[BUFFER_SIZE];
     char buffer[BUFFER_SIZE];
     
     for (int i = 0; i < 5; i++) {
         snprintf(message, sizeof(message), "Test message %d from client %d", i, client_id);
         
         double send_start = get_time_ms();
         
         if (send(sock, message, strlen(message), 0) < 0) {
             break;
         }
         
         stats->message_send_time = get_time_ms() - send_start;
         stats->messages_sent++;
         
         // Try to receive response (with timeout)
         fd_set readfds;
         struct timeval timeout;
         FD_ZERO(&readfds);
         FD_SET(sock, &readfds);
         timeout.tv_sec = 2;
         timeout.tv_usec = 0;
         
         if (select(sock + 1, &readfds, NULL, NULL, &timeout) > 0) {
             if (FD_ISSET(sock, &readfds)) {
                 int bytes = recv(sock, buffer, BUFFER_SIZE - 1, 0);
                 if (bytes > 0) {
                     buffer[bytes] = '\0';
                     stats->messages_received++;
                     stats->response_time += get_time_ms() - send_start;
                 }
             }
         }
         
         usleep(100000); // 100ms delay between messages
     }
     
     if (stats->messages_received > 0) {
         stats->response_time /= stats->messages_received;
     }
     
     stats->success = 1;
     close(sock);
     return NULL;
 }
 
 int main(int argc, char *argv[]) {
     int num_clients = MAX_CLIENTS;
     
     if (argc > 1) {
         num_clients = atoi(argv[1]);
         if (num_clients > MAX_CLIENTS) {
             num_clients = MAX_CLIENTS;
         }
     }
     
     printf("Starting test with %d clients...\n", num_clients);
     printf("Make sure server is running on localhost:%d\n", TCP_PORT);
     sleep(2);
     
     pthread_t threads[MAX_CLIENTS];
     int client_ids[MAX_CLIENTS];
     
     double test_start = get_time_ms();
     
     // Create client threads
     for (int i = 0; i < num_clients; i++) {
         client_ids[i] = i;
         client_stats[i].client_id = i;
         
         if (pthread_create(&threads[i], NULL, client_thread, &client_ids[i]) != 0) {
             fprintf(stderr, "Failed to create thread for client %d\n", i);
             continue;
         }
         
         usleep(10000); // 10ms delay between client starts
     }
     
     // Wait for all threads to complete
     for (int i = 0; i < num_clients; i++) {
         pthread_join(threads[i], NULL);
     }
     
     double test_end = get_time_ms();
     
     // Calculate and print statistics
     int successful_clients = 0;
     double total_connection_time = 0;
     double total_response_time = 0;
     int total_messages_sent = 0;
     int total_messages_received = 0;
     
     printf("\n=== TEST RESULTS ===\n");
     printf("Total test time: %.2f ms\n", test_end - test_start);
     printf("\nClient Details:\n");
     printf("ID\tConn(ms)\tResp(ms)\tSent\tRecv\tStatus\n");
     printf("--\t--------\t--------\t----\t----\t------\n");
     
     for (int i = 0; i < num_clients; i++) {
         client_stats_t* stats = &client_stats[i];
         
         printf("%d\t%.2f\t\t%.2f\t\t%d\t%d\t%s\n",
                stats->client_id,
                stats->connection_time,
                stats->response_time,
                stats->messages_sent,
                stats->messages_received,
                stats->success ? "OK" : "FAIL");
         
         if (stats->success) {
             successful_clients++;
             total_connection_time += stats->connection_time;
             total_response_time += stats->response_time;
             total_messages_sent += stats->messages_sent;
             total_messages_received += stats->messages_received;
         }
     }
     
     printf("\n=== SUMMARY ===\n");
     printf("Successful clients: %d/%d (%.1f%%)\n", 
            successful_clients, num_clients, 
            (successful_clients * 100.0) / num_clients);
     
     if (successful_clients > 0) {
         printf("Average connection time: %.2f ms\n", total_connection_time / successful_clients);
         printf("Average response time: %.2f ms\n", total_response_time / successful_clients);
         printf("Total messages sent: %d\n", total_messages_sent);
         printf("Total messages received: %d\n", total_messages_received);
         printf("Message success rate: %.1f%%\n", 
                (total_messages_received * 100.0) / total_messages_sent);
     }
     
     return 0;
 }
 