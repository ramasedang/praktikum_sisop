#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>
#include <signal.h>
#include <stdbool.h>

#define MODE_A 1
#define MODE_B 2

bool is_running = true;
int mode;


//Handler untuk sinyal SIGTERM
void sigterm_handler(int sig) {
    is_running = false;
}

//fungsi menghapus folder
void delete_folder(const char* nama_folder) {
    sleep(5);
    pid_t pid = fork();
    if (pid == 0) {
        execl("/bin/rm", "rm", "-rf", nama_folder, NULL);
        exit(EXIT_SUCCESS);
    }
}

//fungsi melakukan zip pada folder
void zip_folder(const char* nama_folder) {
    char nama_zip[100];
    sprintf(nama_zip, "%s.zip", nama_folder);

    pid_t pid = fork();
    if (pid == 0) {
        execl("/usr/bin/zip", "zip", "-rmq", nama_zip, nama_folder, NULL);
        exit(EXIT_SUCCESS);
    }
}

//fungsi mendownload gambar sesuai perintah pada soal
void download_gambar(const char* nama_folder, int thread_num) {
    time_t t = time(NULL);
    char nama_gambar[50];
    sprintf(nama_gambar, "%04d-%02d-%02d_%02d:%02d:%02d_%d.jpg",
        1900 + localtime(&t)->tm_year, localtime(&t)->tm_mon + 1, localtime(&t)->tm_mday,
        localtime(&t)->tm_hour, localtime(&t)->tm_min, localtime(&t)->tm_sec, thread_num);
    char url[50];
    sprintf(url, "https://picsum.photos/%d", (int) (t%1000)+50);
    char path[100];
    sprintf(path, "%s/%s", nama_folder, nama_gambar);

    pid_t pid = fork();
    if (pid == 0) {
        execl("/usr/bin/wget", "wget", "-q", "-O", path, url, NULL);
        exit(EXIT_SUCCESS);
    }
}

//fungsi membuat file killer sesuai dengan Modenya
void create_killer(char* nama_program) {
    FILE* fp;
    fp = fopen("killer", "w");
    fprintf(fp, "#!/bin/bash\n");

    //mengecek mode yang digunakan
    if (mode == MODE_A) {
        fprintf(fp, "killall -SIGKILL %s\n", nama_program);
    } else {
        fprintf(fp, "kill -SIGTERM %d\n", getpid());
    }

    //menghapus file killer setelah digunakan
    fprintf(fp, "rm killer\n");
    fclose(fp);
    chmod("killer", 0700);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s [-a | -b]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    //mengecek mode yang yang digunakan
    if (strcmp(argv[1], "-a") == 0) {
        mode = MODE_A;
    } else if (strcmp(argv[1], "-b") == 0) {
        mode = MODE_B;
    } else {
        printf("Invalid argument\n");
        exit(EXIT_FAILURE);
    }

    signal(SIGTERM, sigterm_handler);

    // Variabel untuk menyimpan PID dan SID
    pid_t pid, sid;

    // Menyimpan PID dari Child Process
    pid = fork();

    /* Keluar saat fork gagal
    * (nilai variabel pid < 0) */
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }

    /* Keluar saat fork berhasil
    * (nilai variabel pid adalah PID dari child process) */
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    // mengubah file mode mask
    umask(0);

    // create new session
    sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }

    // change working directory
    if (chdir(".") < 0) {
        exit(EXIT_FAILURE);
    }

    // close standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    create_killer(argv[0]);

    // start the main loop
    while (is_running) {
        time_t t = time(NULL);
        char nama_folder[50];
        sprintf(nama_folder, "%04d-%02d-%02d_%02d:%02d:%02d", 
            1900 + localtime(&t)->tm_year, localtime(&t)->tm_mon + 1, localtime(&t)->tm_mday,
            localtime(&t)->tm_hour, localtime(&t)->tm_min, localtime(&t)->tm_sec);

        pid_t child_pid = fork();

        if (child_pid == 0) {
            // membuat folder
            mkdir(nama_folder, 0777);

            // download gambar
            for (int i = 0; i < 15; i++) {
                download_gambar(nama_folder, i + 1);
                sleep(5);
            }

            // zip folder
            zip_folder(nama_folder);

            // menghapus folder
            delete_folder(nama_folder);

            exit(EXIT_SUCCESS);
        } else {
            sleep(30);
        }
    }
    exit(EXIT_SUCCESS);
}










































































































































































































