#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <wait.h>
#include <signal.h>
#include <stdbool.h>


#define NUM_IMAGES 15
#define INTERVAL 30
#define MODE_A 1
#define MODE_B 2

bool is_running = true;
int mode;

void download_image(const char* folder_name, int thread_num) {
    time_t t = time(NULL);
    char img_name[50];
    sprintf(img_name, "%04d-%02d-%02d_%02d:%02d:%02d_%d.jpg",
        1900 + localtime(&t)->tm_year, localtime(&t)->tm_mon + 1, localtime(&t)->tm_mday,
        localtime(&t)->tm_hour, localtime(&t)->tm_min, localtime(&t)->tm_sec, thread_num);
    char url[50];
    sprintf(url, "https://picsum.photos/%d", (int) (t%1000)+50);
    char path[100];
    sprintf(path, "%s/%s", folder_name, img_name);

    pid_t pid = fork();
    if (pid == 0) {
        execl("/usr/bin/wget", "wget", "-q", "-O", path, url, NULL);
        exit(EXIT_SUCCESS);
    }
}

void zip_folder(const char* folder_name) {
    char zip_name[100];
    sprintf(zip_name, "%s.zip", folder_name);
    pid_t pid = fork();
    if (pid == 0) {
        execl("/usr/bin/zip", "zip", "-rmq", zip_name, folder_name, NULL);
        exit(EXIT_SUCCESS);
    }
}

void delete_folder(const char* folder_name) {
    sleep (8);
    pid_t pid = fork();
    if (pid == 0) {
        execl("/bin/rm", "rm", "-rf", folder_name, NULL);
        exit(EXIT_SUCCESS);
    }
}

void create_killer(char* program_name) {
    FILE* fp;
    fp = fopen("killer.sh", "w");
    fprintf(fp, "#!/bin/bash\n");

    if (mode == MODE_A) {
        fprintf(fp, "killall -SIGKILL %s\n", program_name);
    } else {
        fprintf(fp, "kill -SIGTERM %d\n", getpid());
    }

    fprintf(fp, "rm killer\n");
    fclose(fp);
    chmod("killer", 0700);
}

void sigterm_handler(int sig) {
    is_running = false;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s [-a | -b]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "-a") == 0) {
        mode = MODE_A;
    } else if (strcmp(argv[1], "-b") == 0) {
        mode = MODE_B;
    } else {
        printf("Invalid argument\n");
        exit(EXIT_FAILURE);
    }

    signal(SIGTERM, sigterm_handler);

    pid_t pid, sid;

    // fork parent process
    pid = fork();

       // error handling
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }

    // exit parent process
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    // change file mode mask
    umask(0);

    // create new session
    sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }

    // change working directory
    if (chdir("/") < 0) {
        exit(EXIT_FAILURE);
    }

    // close standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // start the main loop
    while (is_running) {
        time_t t = time(NULL);
        char folder_name[50];
        sprintf(folder_name, "%04d-%02d-%02d_%02d:%02d:%02d", 
            1900 + localtime(&t)->tm_year, localtime(&t)->tm_mon + 1, localtime(&t)->tm_mday,
            localtime(&t)->tm_hour, localtime(&t)->tm_min, localtime(&t)->tm_sec);

        pid_t child_pid = fork();

        if (child_pid == 0) {
            // create folder
            mkdir(folder_name, 0777);

            // download images
            for (int i = 0; i < NUM_IMAGES; i++) {
                download_image(folder_name, i + 1);
                sleep(1);
            }

            // zip folder
            zip_folder(folder_name);

            // delete folder
            delete_folder(folder_name);

            exit(EXIT_SUCCESS);
        } else {
            sleep(INTERVAL);
        }
    }

    // create the killer script
    create_killer(argv[0]);

    exit(EXIT_SUCCESS);
}
