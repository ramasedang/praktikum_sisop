#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_LEN 128

void print_error(char *msg) {
    printf("Error: %s\n", msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    pid_t pid, sid;
    int fd, sec, min, hour;
    char *file_path;

    if (argc != 5) {
        print_error("Invalid arguments!");
    }

    if ((strcmp(argv[1], "*") != 0) && (atoi(argv[1]) < 0 || atoi(argv[1]) > 23)) {
        print_error("Invalid hour argument!");
    }

    if (atoi(argv[2]) < 0 || atoi(argv[2]) > 59) {
        print_error("Invalid minute argument!");
    }

    if (atoi(argv[3]) < 0 || atoi(argv[3]) > 59) {
        print_error("Invalid second argument!");
    }

    file_path = argv[4];

    pid = fork();
    if (pid < 0) {
        print_error("Failed to fork!");
    }

    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    umask(0);

    sid = setsid();
    if (sid < 0) {
        print_error("Failed to create a new session!");
    }

    if ((chdir("/")) < 0) {
        print_error("Failed to change working directory!");
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    fd = open("/dev/null", O_WRONLY);

    while (1) {
        time_t now = time(NULL);
        struct tm *tm_info = localtime(&now);

        sec = tm_info->tm_sec;
        min = tm_info->tm_min;
        hour = tm_info->tm_hour;

        if ((strcmp(argv[1], "*") == 0 || atoi(argv[1]) == hour) && atoi(argv[2]) == min && atoi(argv[3]) == sec) {
            if (fork() == 0) {
                execl("/bin/bash", "bash", file_path, NULL);
                exit(0);
            }
        }

        sleep(1);
    }

    close(fd);
    return 0;
}
