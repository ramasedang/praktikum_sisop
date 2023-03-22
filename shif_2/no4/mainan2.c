#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>

#define MAX_COMMAND_LENGTH 100

void print_error_message()
{
    printf("Error: Invalid arguments\n");
}

void signal_handler(int signum)
{
    // do nothing
}

void daemonize()
{
    pid_t pid = fork();
    if (pid < 0)
    {
        exit(EXIT_FAILURE);
    }
    if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }
    if (setsid() < 0)
    {
        exit(EXIT_FAILURE);
    }
    umask(0);
    chdir("/");
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

int parse_time_string(char *time_str, int min_value, int max_value)
{
    if (strcmp(time_str, "*") == 0)
    {
        return -1;
    }
    else if (strchr(time_str, '/') != NULL)
    {
        char *slash_pos = strchr(time_str, '/');
        int divisor = atoi(slash_pos + 1);
        if (divisor == 0)
        {
            print_error_message();
            exit(1);
        }
        int value = atoi(time_str);
        if (value < min_value || value > max_value)
        {
            print_error_message();
            exit(1);
        }
        return value;
    }
    else
    {
        int value = atoi(time_str);
        if (value < min_value || value > max_value)
        {
            print_error_message();
            exit(1);
        }
        return value;
    }
}

int should_execute(char *hour_str, char *minute_str, char *second_str)
{
    time_t now = time(NULL);
    struct tm *current_time = localtime(&now);
    int hour = parse_time_string(hour_str, 0, 23);
    int minute = parse_time_string(minute_str, 0, 59);
    int second = parse_time_string(second_str, 0, 59);
    // rest of the function remains the same
    int interval = 1;
    if (hour != -1)
    {
        interval *= (hour == current_time->tm_hour) ? 1 : 0;
    }
    if (minute != -1)
    {
        int minute_value = parse_time_string(minute, 0, 59);
        if (minute_value == -1)
        {
            interval *= 1;
        }
        else
        {
            interval *= ((current_time->tm_min - minute_value) % minute_value == 0) ? 1 : 0;
        }
    }
    if (second != -1)
    {
        int second_value = parse_time_string(second, 0, 59);
        if (second_value == -1)
        {
            interval *= 1;
        }
        else
        {
            interval *= ((current_time->tm_sec - second_value) % second_value == 0) ? 1 : 0;
        }
    }
    return interval;
}

void execute_command(char *command_path)
{
    char command[MAX_COMMAND_LENGTH];
    sprintf(command, "/bin/bash %s", command_path);
    system(command);
}

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        print_error_message();
        return 1;
    }

    int hour = parse_time_string(argv[1], 0, 23);
    int minute = parse_time_string(argv[2], 0, 59);
    int second = parse_time_string(argv[3], 0, 59);
    char *command_path = argv[4];

    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, signal_handler);

    pid_t pid = fork();
    if (pid < 0)
    {
        exit(EXIT_FAILURE);
    }
    if (pid > 0)
    {
        // exit parent process
        exit(EXIT_SUCCESS);
    }

    // child process 1
    setsid();

    pid = fork();
    if (pid < 0)
    {
        exit(EXIT_FAILURE);
    }
    if (pid > 0)
    {
        // exit child process 1
        exit(EXIT_SUCCESS);
    }

    // child process 2 (daemon)
    daemonize();

    while (1)
    {
        if (should_execute(argv[1], argv[2], argv[3]))
        {
            execute_command(command_path);
        }

        sleep(1);
    }

    return 0;
}
