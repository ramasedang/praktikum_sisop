#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAX_COMMAND_LENGTH 100

void print_error_message() {
    printf("Error: Invalid arguments\n");
}

void signal_handler(int signum) {
    // do nothing
}

void daemonize() {
    pid_t pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }
    if (setsid() < 0) {
        exit(EXIT_FAILURE);
    }
    umask(0);
    chdir("/");
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        print_error_message();
        //prin all arguments
        for (int i = 0; i < argc; i++) {
            printf("argv[%d] = %s ", i, argv[i]);
        }
        return 1;
    }

    char* hour_str = argv[1];
    char* minute_str = argv[2];
    char* second_str = argv[3];
    char* command_path = argv[4];

    int hour, minute, second;
    if (strcmp(hour_str, "*") == 0) {
        hour = -1;
    } else {
        hour = atoi(hour_str);
        if (hour < 0 || hour > 23) {
            print_error_message();
            return 1;
        }
    }

    if (strcmp(minute_str, "*") == 0) {
        minute = -1;
    } else {
        minute = atoi(minute_str);
        if (minute < 0 || minute > 59) {
            print_error_message();
            return 1;
        }
    }

    if (strcmp(second_str, "*") == 0) {
        second = -1;
    } else {
        second = atoi(second_str);
        if (second < 0 || second > 59) {
            print_error_message();
            return 1;
        }
    }

    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, signal_handler);

    pid_t pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        // exit parent process
        exit(EXIT_SUCCESS);
    }

    // child process 1
    setsid();

    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        // exit child process 1
        exit(EXIT_SUCCESS);
    }

    // child process 2 (daemon)
    daemonize();

    while (1) {
        time_t now = time(NULL);
        struct tm* current_time = localtime(&now);
        if ((hour == -1 || hour == current_time->tm_hour) &&
            (minute == -1 || minute == current_time->tm_min) &&
            (second == -1 || second == current_time->tm_sec)) {
            char command[MAX_COMMAND_LENGTH];
            sprintf(command, "/bin/bash %s", command_path);
            system(command);
        }
        sleep(1);
    }

    return 0;
}
