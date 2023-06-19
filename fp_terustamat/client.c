#include <arpa/inet.h>
#include <getopt.h>
#include <libgen.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>



#define PORT 8080
#define BUFFER_SIZE 1024

char loggedInUsername[1000];



struct User {
  char username[100];
  char password[100];
};



int registerUser(const char *username, const char *password) {

  if (geteuid() != 0) {
    printf("Registration can only be done by the root user.\n");
    return 0;
  }

  FILE *file = fopen("users.txt", "a+");

  if (file == NULL) {
    printf("Failed to open users.txt file.\n");
    return 0;
  }

  struct User user;

  while (fscanf(file, "%[^;];%[^\n]\n", user.username, user.password) != EOF) {
    if (strcmp(username, user.username) == 0) {
      printf("Username already exists.\n");
      fclose(file);
      return 0;

    }
  }


  fprintf(file, "%s;%s\n", username, password);
  fclose(file);

  if (chmod("users.txt", 0644) != 0) {
    printf("Failed to change file permissions.\n");
    return 1;
  }
  printf("User registered successfully.\n");
  return 1;
}


int loginUser(const char *username, const char *password) {
  FILE *file = fopen("users.txt", "r");
  if (file == NULL) {
    printf("Failed to open users.txt file.\n");
    return 0;

  }
  rewind(file);
  struct User user;

  while (fscanf(file, "%[^;];%[^\n]\n", user.username, user.password) != EOF) {
    if (strcmp(username, user.username) == 0) {
      if (strcmp(password, user.password) == 0) {
        fclose(file);
        strcpy(loggedInUsername, username);
        return 1;
      }
      break;
    }
  }
  fclose(file);
  return 0;
}



void writeLog(const char *username, const char *command) {
  time_t now = time(NULL);
  struct tm *timeinfo = localtime(&now);
  char timestamp[20];
  strftime(timestamp, sizeof(timestamp), "%d:%m:%Y:%H:%M:%S", timeinfo);
  FILE *file = fopen("log.txt", "a+");



  if (file == NULL) {
    printf("Failed to open log.txt file.\n");
    return;
  }
  fprintf(file, "%s:%s:%s\n", timestamp, username, command);
  fclose(file);
}



int main(int argc, char *argv[]) {
  int client_socket;
  struct sockaddr_in server_address;
  char buffer[BUFFER_SIZE] = {0};
  bool isLoggedIn = false;
  char loggedInUsername[100];


  // Check if the program is running as root (sudo)
  if (geteuid() == 0) {
    // If the program is running as root, set isLoggedIn to true and
    // loggedInUsername to "sudo"
    isLoggedIn = true;
    strcpy(loggedInUsername, "sudo");
  }
  if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(PORT);
  server_address.sin_addr.s_addr = INADDR_ANY;
    
  if (connect(client_socket, (struct sockaddr *)&server_address,
              sizeof(server_address)) < 0) {
    perror("connection failed");
    exit(EXIT_FAILURE);

  }
  while (1) {
    if (!isLoggedIn) {
      printf("Type a random character to continue or 'exit' to exit.\n");
      char option[10];
      fgets(option, sizeof(option), stdin);
      if (strcmp(option, "exit\n") == 0) {
        break;
      } else {
        char username[100];
        char password[100];
        int option;
        while ((option = getopt(argc, argv, "u:p:")) != -1) {
          switch (option) {
            case 'u':
              strcpy(username, optarg);
              break;
            case 'p':
              strcpy(password, optarg);
              break;
            default:
              printf("Invalid option.\n");
              break;
          }
        }
        if (strlen(username) > 0 && strlen(password) > 0) {

          if (loginUser(username, password)) {
            printf("Login successful.\n");
            isLoggedIn = true;
            strcpy(loggedInUsername, username);
          } else {
            printf("Invalid credentials. Please try again.\n");
          }
        }
      }

    } else {

      while (isLoggedIn) {
        printf("LOGGED IN AS %s\n", loggedInUsername);
        char command[100];
        printf("Enter command: ");

        fgets(command, sizeof(command), stdin);

        if (command[strlen(command) - 1] == '\n') {
          strtok(command, "\n");
        }
        // If the user is root and the command starts with "CREATE USER",
        // register the user
        if (strcmp(loggedInUsername, "sudo") == 0 && geteuid() == 0 &&
        strncmp(command, "CREATE USER", 11) == 0) {
          char username[100];
          char password[100];

          if (sscanf(command, "CREATE USER %s IDENTIFIED BY %s", username,
                     password) == 2 ) {
            registerUser(username, password);

            printf("User %s registered successfully.\n", username);
          } else if(sscanf(command, "CREATE USER %s IDENTIFIED BY %s", username,
                     password) == 2 && geteuid() != 0){
                        printf("YOu are not the root user");        
                     } 
          else {
            printf(
                "Invalid format. Usage: CREATE USER [username] IDENTIFIED BY "
                "[password]\n");
          }
          continue;
        }

        writeLog(loggedInUsername, command);
        send(client_socket, command, strlen(command), 0);
        memset(buffer, 0, BUFFER_SIZE);

        if (recv(client_socket, buffer, BUFFER_SIZE, 0) > 0) {
          printf("%s\n", buffer);
        }
      }
    }
  }
  close(client_socket);
  return 0;
}
