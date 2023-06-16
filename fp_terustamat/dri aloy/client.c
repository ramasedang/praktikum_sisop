#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <libgen.h>
#include <sys/stat.h>
#include <stdbool.h>

#define PORT 8080
#define BUFFER_SIZE 1024

struct User {
    char username[100];
    char password[100];
};

int registerUser(const char* username, const char* password) {
    if (geteuid() != 0) {
        printf("Registration can only be done by sudo.\n");
        return 0;
    }

    FILE* file = fopen("users.txt", "a+");
    if (file == NULL) {
        printf("Failed to open users.txt file.\n");
        return 0;
    }

    // Check username
    struct User user;
    while (fscanf(file, "%[^;];%[^\n]\n", user.username, user.password) != EOF) {
        if (strcmp(username, user.username) == 0) {
            printf("Username already exists.\n");
            fclose(file);
            return 0;
        }
    }

    // Store username and hashed password in the file
    fprintf(file, "%s;%s\n", username, password);
    fclose(file);

    printf("User registered successfully.\n");
    return 1;
}

int loginUser(const char* username, const char* password) {
    FILE* file = fopen("users.txt", "r");
    if (file == NULL) {
        printf("Failed to open users.txt file.\n");
        return 0;
    }

    struct User user;
    while (fscanf(file, "%[^;];%[^\n]\n", user.username, user.password) != EOF) {
        if (strcmp(username, user.username) == 0) {
            if (strcmp(password, user.password) == 0) {
                fclose(file);
                return 1;  // Login berhasil
            }
            break;  // Username ditemukan, tapi password salah
        }
    }

    fclose(file);
    return 0;  // Username tidak ditemukan / password salah
}


int main()
{
    int client_socket;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE] = {0};
    bool isLoggedIn = false;

    // Create socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("connection failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        if (!isLoggedIn) {
            printf("Please login or register.\n");
            printf("Enter 'login' to login or 'register' to register.\n");
            char option[10];
            fgets(option, sizeof(option), stdin);

            if (strcmp(option, "login\n") == 0) {
                // Login process
                char username[100];
                char password[100];
                printf("Enter username: ");
                fgets(username, sizeof(username), stdin);
                printf("Enter password: ");
                fgets(password, sizeof(password), stdin);

                // Remove trailing newline characters
                username[strcspn(username, "\n")] = '\0';
                password[strcspn(password, "\n")] = '\0';

                if (loginUser(username, password)) {
                    printf("Login successful.\n");
                    isLoggedIn = true;
                } else {
                    printf("Invalid credentials. Please try again.\n");
                }
            } else if (strcmp(option, "register\n") == 0) {
                // Registration process
                char username[100];
                char password[100];
                printf("Enter username: ");
                fgets(username, sizeof(username), stdin);
                printf("Enter password: ");
                fgets(password, sizeof(password), stdin);

                // Remove trailing newline characters
                username[strcspn(username, "\n")] = '\0';
                password[strcspn(password, "\n")] = '\0';

                if (registerUser(username, password)) {
                    printf("User registered successfully.\n");

                    // Change file permissions
                    if (chmod("users.txt", 0600) != 0) {
                        printf("Failed to change file permissions.\n");
                        return 1;
                    }
                    isLoggedIn = true;
                } else {
                    printf("Failed to register user.\n");
                }
            } else {
                printf("Invalid option. Please try again.\n");
            }
        } else {
            // User is logged in
            // Send command to the server
            char command[100];
            printf("Enter command: ");
            fgets(command, sizeof(command), stdin);

            // Send the command to the server
            send(client_socket, command, strlen(command), 0);

            // Receive response from the server
            memset(buffer, 0, BUFFER_SIZE);
            if (recv(client_socket, buffer, BUFFER_SIZE, 0) > 0)
            {
                printf("%s\n", buffer);
            }
        }
    }

    // Close the socket
    close(client_socket);

    return 0;
}
