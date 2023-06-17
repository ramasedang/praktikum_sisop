#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <libgen.h>
#include <sys/stat.h>

struct User {
    char username[100];
    char password[100];
};

int registerUser(const char* username, const char* password) {
    if (geteuid() != 0) {
        printf("Registration can only be done by the root user.\n");
        return 0;
    }

    FILE* file = fopen("users.txt", "a+");
    if (file == NULL) {
        printf("Failed to open users.txt file.\n");
        return 0;
    }

    // Check username
    struct User user;
    while (fscanf(file, "CREATE USER %[^ ] IDENTIFIED BY %[^;];\n", user.username, user.password) == 2)  {
        if (strcmp(username, user.username) == 0) {
            printf("Username already exists.\n");
            fclose(file);
            return 0;
        }
    }

    fprintf(file, "CREATE USER %s IDENTIFIED BY %s;\n", username, password);
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

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printf("Invalid format. Usage:\n");
        printf("For registration: sudo ./program CREATE USER [username] IDENTIFIED BY [password]\n");
        printf("For login: ./program -login -u [username] -p [password]\n");
        return 1;
    }

    const char* mode = argv[1];
    const char* username = NULL;
    const char* password = NULL;

    if (strcmp(mode, "CREATE") == 0 && strcmp(argv[2], "USER") == 0 && argc >= 6) {
        username = argv[3];
        password = argv[5];
    }

    if (username == NULL || password == NULL) {
        printf("Invalid format. Usage:\n");
        printf("For registration: sudo ./program CREATE USER [username] IDENTIFIED BY [password]\n");
        printf("For login: ./program -login -u [username] -p [password]\n");
        return 1;
    }

    if (strcmp(mode, "CREATE") == 0 && strcmp(argv[2], "USER") == 0) {
        if (geteuid() != 0) {
            printf("Registration can only be done by the root user.\n");
            return 1;
        }
        int result = registerUser(username, password);
        if (result == 1) {
            printf("User registered successfully.\n");

            // Change file permissions
            if (chmod("users.txt", 0600) != 0) {
                printf("Failed to change file permissions.\n");
                return 1;
            }
            return 0;
        } else {
            printf("Failed to register user.\n");
            return 1;
        }
    } else if (strcmp(mode, "-login") == 0) {
        int result = loginUser(username, password);
        if (result == 1) {
            printf("Logged in as %s.\n", username);
            return 0;
        } else {
            printf("Invalid credentials.\n");
            return 1;
        }
    } else {
        printf("Invalid mode. Usage:\n");
        printf("For registration: sudo ./program CREATE USER [username] IDENTIFIED BY [password]\n");
        printf("For login: ./program -login -u [username] -p [password]\n");
        return 1;
    }
}
