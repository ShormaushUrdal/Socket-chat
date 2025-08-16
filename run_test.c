/**
 * @file run_test.c 
 * @brief Test runner that starts server and runs client tests
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <sys/wait.h>
 #include <signal.h>
 
 pid_t server_pid = 0;
 
 void cleanup_server() {
     if (server_pid > 0) {
         printf("Stopping server (PID: %d)...\n", server_pid);
         kill(server_pid, SIGTERM);
         waitpid(server_pid, NULL, 0);
     }
 }
 
 void signal_handler(int sig) {
     cleanup_server();
     exit(0);
 }
 
 int main(int argc, char *argv[]) {
     int num_clients = 100;
     
     if (argc > 1) {
         num_clients = atoi(argv[1]);
     }
     
     signal(SIGINT, signal_handler);
     signal(SIGTERM, signal_handler);
     
     printf("Starting server...\n");
     
     server_pid = fork();
     if (server_pid == 0) {
         // Child process - run server
         execl("./server_discovery", "server_discovery", NULL);
         perror("Failed to start server");
         exit(1);
     } else if (server_pid < 0) {
         perror("Fork failed");
         return 1;
     }
     
     // Give server time to start
     sleep(3);
     
     printf("Starting client tests with %d clients...\n", num_clients);
     
     // Run test client - FIX: Create complete command string
     char command[256];
     snprintf(command, sizeof(command), "./test_client %d", num_clients);
     
     int status = system(command);
     if (status < 0) {
         printf("Test execution failed\n");
     }
     
     cleanup_server();
     
     return 0;
 }
 