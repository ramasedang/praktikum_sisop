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

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Invalid format. Usage: sudo ./program [-login | -register] [username] [password]\n");
        return 1;
    }

    const char* mode = argv[1];
    const char* username = argv[2];
    const char* password = argv[3];

    if (strcmp(mode, "-login") == 0) {
        int result = loginUser(username, password);
        if (result == 1) {
            printf("Login successful.\n");
            return 0;
        } else {
            printf("Invalid credentials.\n");
            return 1;
        }
    } else if (strcmp(mode, "-register") == 0) {
        if (geteuid() != 0) {
            printf("Registration can only be done by sudo.\n");
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
    } else {
        printf("Invalid mode. Usage: sudo ./program [-login | -register] [username] [password]\n");
        return 1;
    }
}
