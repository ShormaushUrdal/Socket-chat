/**
 * @file load_test.c
 * @brief Advanced load testing with automatic server management
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <sys/wait.h>
 #include <signal.h>
 #include <time.h>
 
 pid_t server_pid = 0;
 
 void start_server() {
     printf("Starting server...\n");
     server_pid = fork();
     if (server_pid == 0) {
         // Child process - run server
         execl("./server_discovery", "server_discovery", NULL);
         perror("Failed to start server");
         exit(1);
     } else if (server_pid < 0) {
         perror("Fork failed");
         exit(1);
     }
     sleep(3); // Give server time to start
 }
 
 void stop_server() {
     if (server_pid > 0) {
         printf("Stopping server (PID: %d)...\n", server_pid);
         kill(server_pid, SIGTERM);
         waitpid(server_pid, NULL, 0);
         server_pid = 0;
         sleep(1); // Give time for cleanup
     }
 }
 
 void run_test_with_clients(int num_clients) {
     char cmd[256];
     snprintf(cmd, sizeof(cmd), "./test_client %d", num_clients);
     
     printf("\n=== Testing with %d clients ===\n", num_clients);
     
     // Start fresh server for each test
     start_server();
     
     // Run the test
     int status = system(cmd);
     if (status != 0) {
         printf("Test with %d clients FAILED (exit code: %d)\n", num_clients, status);
     } else {
         printf("Test with %d clients completed successfully\n", num_clients);
     }
     
     // Stop server after test
     stop_server();
     
     printf("Waiting between tests...\n");
     sleep(2);
 }
 
 int main() {
     printf("Starting comprehensive load test with server management...\n");
     
     // Test with increasing number of clients
     int client_counts[] = {1, 5, 10, 25, 50};  // Reduced for stability
     int num_tests = sizeof(client_counts) / sizeof(client_counts[0]);
     
     // Signal handler for cleanup
     signal(SIGINT, stop_server);
     signal(SIGTERM, stop_server);
     
     for (int i = 0; i < num_tests; i++) {
         printf("\n" "========================================\n");
         printf("Running test %d/%d\n", i + 1, num_tests);
         printf("========================================\n");
         
         run_test_with_clients(client_counts[i]);
     }
     
     printf("\nLoad test complete!\n");
     return 0;
 }
 