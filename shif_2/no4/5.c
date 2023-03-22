#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <curl/curl.h>
#include <pthread.h>
#include <dirent.h>
#include <zip.h>

#define MODE_A 1
#define MODE_B 2

volatile sig_atomic_t running = 1;

typedef struct
{
    char folder_name[20];
    int image_size;
} ImageParams;

void timestamp(char *buffer)
{
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, 20, "%Y-%m-%d_%H:%M:%S", timeinfo);
}

void create_folder(const char *folder_name)
{
    mkdir(folder_name, 0777);
}

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

void download_image(const char *folder_name, const char *file_name, int image_size)
{
    char url[50];
    snprintf(url, 50, "https://picsum.photos/%d", image_size);

    char file_path[40];
    snprintf(file_path, 40, "%s/%s.jpg", folder_name, file_name);

    CURL *curl = curl_easy_init();
    if (curl)
    {
        FILE *fp = fopen(file_path, "wb");
        if (fp)
        {
            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
            curl_easy_perform(curl);
            fclose(fp);
        }
        curl_easy_cleanup(curl);
    }
}

void *download_images_thread(void *arg)
{
    ImageParams *params = (ImageParams *)arg;
    for (int i = 0; i < 15; i++)
    {
        sleep(5);
        char file_name[20];
        timestamp(file_name);
        download_image(params->folder_name, file_name, params->image_size);
    }
    free(params);
    return NULL;
}

void start_downloading_images(const char *folder_name, int image_size)
{
    pthread_t thread_id;
    ImageParams *params = malloc(sizeof(ImageParams));
    strncpy(params->folder_name, folder_name, 20);
    params->image_size = image_size;

    pthread_create(&thread_id, NULL, download_images_thread, params);
    pthread_detach(thread_id);
}

void zip_and_remove_folder(const char *folder_name)
{
    char zip_file_name[25];
    snprintf(zip_file_name, 25, "%s.zip", folder_name);

    zip_t *zip = zip_open(zip_file_name, ZIP_CREATE | ZIP_EXCL, NULL);
    if (zip)
    {
        DIR *dir = opendir(folder_name);
        if (dir)
        {
            struct dirent *entry;
            while ((entry = readdir(dir)))
            {
                char file_path[40];
               snprintf(file_path, 255, "%s/%s", folder_name, entry->d_name);

                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                {
                    zip_source_t *source = zip_source_file(zip, file_path, 0, 0);
                    if (source)
                    {
                        zip_file_add(zip, entry->d_name, source, ZIP_FL_ENC_UTF_8);
                    }
                }
            }
            closedir(dir);
        }
        zip_close(zip);

        char remove_command[30];
        snprintf(remove_command, 30, "rm -rf %s", folder_name);
        system(remove_command);
    }
}

void create_killer_program(int mode, pid_t pid)
{
    FILE *fp = fopen("killer.sh", "w");
    if (fp)
    {
        fprintf(fp, "#!/bin/bash\n");
        if (mode == MODE_A)
        {
            fprintf(fp, "kill %d\n", pid);
        }
        else if (mode == MODE_B)
        {
            fprintf(fp, "kill -SIGUSR1 %d\n", pid);
        }

        fprintf(fp, "rm $0\n");
        fclose(fp);

        system("chmod +x killer.sh");
    }
}

void handle_sigusr1(int signum)
{
    running = 0;
}

void start_daemon(int mode)
{
    pid_t pid = fork();
    if (pid == -1)
    {
        exit(EXIT_FAILURE);
    }
    if (pid == 0)
    {
        // Child process
        umask(0);

        pid_t sid = setsid();
        if (sid == -1)
        {
            exit(EXIT_FAILURE);
        }

        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);

        while (running)
        {
            char folder_name[20];
            timestamp(folder_name);
            create_folder(folder_name);

            for (int i = 0; i < 15; i++)
            {
                int t = time(NULL);
                int image_size = (t % 1000) + 50;

                start_downloading_images(folder_name, image_size);
            }

            zip_and_remove_folder(folder_name);

            sleep(30);
        }

        exit(EXIT_SUCCESS);
    }
    else
    {
        // Parent process
        create_killer_program(mode, pid);

        if (mode == MODE_B)
        {
            signal(SIGUSR1, handle_sigusr1);
        }

        while (running)
        {
            sleep(1);
        }

        kill(pid, SIGTERM);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s [-a|-b]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int mode = 0;
    if (strcmp(argv[1], "-a") == 0)
    {
        mode = MODE_A;
    }
    else if (strcmp(argv[1], "-b") == 0)
    {
        mode = MODE_B;
    }
    else
    {
        printf("Invalid argument: %s\n", argv[1]);
        printf("Usage: %s [-a|-b]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    start_daemon(mode);

    return 0;
}