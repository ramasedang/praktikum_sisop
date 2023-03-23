#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>

#define MAX_PANJANG_PERINTAH 100

void cetak_pesan_error();
void tangani_sinyal(int signum);
void buat_proses_latar();
int parse_waktu_string(char *waktu_str, int min_value, int max_value);
int perlu_dieksekusi(char *jam_str, char *menit_str, char *detik_str);
void jalankan_perintah(char *path_perintah);

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        cetak_pesan_error();
        return 1;
    }

    int jam = parse_waktu_string(argv[1], 0, 23);
    int menit = parse_waktu_string(argv[2], 0, 59);
    int detik = parse_waktu_string(argv[3], 0, 59);
    char *path_perintah = argv[4];

    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, tangani_sinyal);

    pid_t pid = fork();
    if (pid < 0)
    {
        exit(EXIT_FAILURE);
    }
    if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }

    setsid();

    pid = fork();
    if (pid < 0)
    {
        exit(EXIT_FAILURE);
    }
    if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }

    buat_proses_latar();

    while (1)
    {
        if (perlu_dieksekusi(argv[1], argv[2], argv[3]))
        {
            jalankan_perintah(path_perintah);
        }

        sleep(1);
    }

    return 0;
}

void cetak_pesan_error()
{
    printf("Error: Argumen tidak valid\n");
}

void tangani_sinyal(int signum)
{
    // tidak melakukan apa-apa
}

void buat_proses_latar()
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

int parse_waktu_string(char *waktu_str, int min_value, int max_value)
{
    if (strcmp(waktu_str, "*") == 0)
    {
        return -1;
    }
    else if (strchr(waktu_str, '/') != NULL)
    {
        char *slash_pos = strchr(waktu_str, '/');
        int pembagi = atoi(slash_pos + 1);
        if (pembagi == 0)
        {
            cetak_pesan_error();
            exit(1);
        }
        int nilai = atoi(waktu_str);
        if (nilai < min_value || nilai > max_value)
        {
            cetak_pesan_error();
            exit(1);
        }
        return nilai;
    }
    else
    {
        int nilai = atoi(waktu_str);
        if (nilai < min_value || nilai > max_value)
        {
            cetak_pesan_error();
            exit(1);
        }
        return nilai;
    }
}

int perlu_dieksekusi(char *jam_str, char *menit_str, char *detik_str)
{
    time_t now = time(NULL);
    struct tm *waktu_sekarang = localtime(&now);
    int jam = parse_waktu_string(jam_str, 0, 23);
    int menit = parse_waktu_string(menit_str, 0, 59);
    int detik = parse_waktu_string(detik_str, 0, 59);
    int interval = 1;
    if (jam != -1)
    {
        interval *= (jam == waktu_sekarang->tm_hour) ? 1 : 0;
    }
    if (menit != -1)
    {
        int nilai_menit = parse_waktu_string(menit_str, 0, 59);
        if (nilai_menit == -1)
        {
            interval *= 1;
        }
        else
        {
            interval *= ((waktu_sekarang->tm_min - nilai_menit) % nilai_menit == 0) ? 1 : 0;
        }
    }
    if (detik != -1)
    {
        int nilai_detik = parse_waktu_string(detik_str, 0, 59);
        if (nilai_detik == -1)
        {
            interval *= 1;
        }
        else
        {
            interval *= ((waktu_sekarang->tm_sec - nilai_detik) % nilai_detik == 0) ? 1 : 0;
        }
    }
    return interval;
}

void jalankan_perintah(char *path_perintah)
{
    char perintah[MAX_PANJANG_PERINTAH];
    sprintf(perintah, "/bin/bash %s", path_perintah);
    system(perintah);
}