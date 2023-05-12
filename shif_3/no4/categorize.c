#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>

#define MAX_EXT_COUNT 100
#define MAX_EXT_LEN 10

char *extensions[] = {"jpg", "txt", "js", "py", "png", "emc", "xyz"};
char *files_dir = "/home/reyhanqb/Documents/Shift 3/soal4/files";
char *dir_name;

int ext_size = sizeof(extensions) / sizeof(extensions[0]);
const int maximum = 10;

time_t current_time;
struct tm *timeline;

char buff[80], valid_dir[256];
char src[256], dest[256];
char subdir[256], destination[256], directory_path[256], dest_folder[256];

char ext_list[MAX_EXT_COUNT][MAX_EXT_LEN]; // list ekstensi file yang akan diproses

void createLog();

void *categorizeFiles(void *dir_name)
{
    DIR *dir;
    struct dirent *ent;
    char current_dir[256];
    strcpy(current_dir, dir_name);
    char logs[512];

    snprintf(logs, sizeof(logs), "ACCESSED %s", current_dir);
    createLog(logs);

    if ((dir = opendir(current_dir)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (ent->d_type == DT_REG)
            {
                char *validExtension = strrchr(ent->d_name, '.');
                int validFiles = 0;
                char valid_ext[256] = "other";

                if (validExtension)
                {
                    validExtension++;
                    strcpy(valid_ext, validExtension);

                    for (char *p = valid_ext; *p; ++p)
                    {
                        *p = tolower(*p);
                    }

                    for (int i = 0; i < ext_size; i++)
                    {
                        if (strcmp(valid_ext, extensions[i]) == 0)
                        {
                            validFiles = 1;
                            break;
                        }
                    }
                }

                if (!validFiles)
                {
                    snprintf(dest_folder, sizeof(dest_folder), "categorized/other");

                    if (dest_folder == NULL)
                    {
                        mkdir(dest_folder, 0700);
                    }

                    snprintf(src, sizeof(src), "%s/%s", current_dir, ent->d_name);
                    snprintf(dest, sizeof(dest), "%s/%s", dest_folder, ent->d_name);

                    rename(src, dest);
                    snprintf(logs, sizeof(logs), "MOVED %s file : %s > %s", valid_ext, src, dest);
                    createLog(logs);
                }
                else
                {
                    snprintf(destination, sizeof(destination), "categorized/%s", valid_ext);

                    struct stat st;
                    if (stat(destination, &st) == -1)
                    {
                        mkdir(destination, 0700);
                        snprintf(logs, sizeof(logs), "MADE %s", destination);
                        createLog(logs);
                    }

                    int dirs = 1;
                    strcpy(dest_folder, destination);

                    while (1)
                    {
                        DIR *dest_dir = opendir(dest_folder);
                        int count = 0;

                        if (dest_dir)
                        {
                            struct dirent *file_ent;
                            while ((file_ent = readdir(dest_dir)) != NULL)
                            {
                                if (file_ent->d_type == DT_REG)
                                {
                                    count++;
                                }
                            }
                            closedir(dest_dir);
                        }

                        if (count < maximum)
                        {
                            break;
                        }

                        dirs++;
                        snprintf(dest_folder, sizeof(dest_folder), "%s (%d)", destination, dirs);
                        if (stat(dest_folder, &st) == -1)
                        {
                            mkdir(dest_folder, 0700);
                            snprintf(logs, sizeof(logs), "MADE %s", dest_folder);
                            createLog(logs);
                        }
                    }

                    snprintf(src, sizeof(src), "%s/%s", current_dir, ent->d_name);
                    snprintf(dest, sizeof(dest), "%s/%s", dest_folder, ent->d_name);

                    rename(src, dest);
                    snprintf(logs, sizeof(logs), "MOVED %s file : %s > %s", valid_ext, src, dest);
                    createLog(logs);
                }
            }
            else if (ent->d_type == DT_DIR && ent->d_name[0] != '.')
            {
                
                snprintf(subdir, sizeof(subdir), "%s/%s", current_dir, ent->d_name);

                pthread_t thread;
                pthread_create(&thread, NULL, categorizeFiles, (void *)subdir);
                pthread_join(thread, NULL);
            }
        }
        closedir(dir);
    }

    return NULL;
}

int countOtherFiles(const char *current_dir_path)
{
    int count = 0;
    int dirs = 1;
    
    strcpy(directory_path, current_dir_path);

    while (true)
    {
        int found = 0;
        DIR *dir;
        struct dirent *ent;

        if ((dir = opendir(directory_path)) != NULL)
        {
            while ((ent = readdir(dir)) != NULL)
            {
                if (ent->d_type == DT_REG)
                {
                    count++;
                    found = 1;
                }
            }
            closedir(dir);
        }

        if (!found)
        {
            break;
        }

        dirs++;
        snprintf(directory_path, sizeof(directory_path), "%s (%d)", current_dir_path, dirs);
    }

    return count;
}

void getAmountOfFiles(){
       // Membuat file temp.txt untuk menyimpan output 'ls'
    system("ls -R categorized > temp.txt");

    // Membuka file sementara
    FILE *temp_file = fopen("temp.txt", "r");
    char line[256];

    int png_count = 0;
    int jpg_count = 0;
    int emc_count = 0;
    int xyz_count = 0;
    int js_count = 0;
    int py_count = 0;
    int txt_count = 0;
    int other_count = 0;

    // Menghitung jumlah file setiap ekstensi dalam urutan ascending
    while (fgets(line, sizeof(line), temp_file))
    {
        
        line[strcspn(line, "\n")] = 0;

        // Memeriksa apakah baris merupakan path direktori
        if (line[strlen(line) - 1] == ':')
        {
            continue;
        }
        else
        {
            // Memeriksa apakah baris merupakan nama file atau direktori
            char *file_name = strrchr(line, '/');
            if (file_name != NULL)
            {
                file_name++;
            }
            else
            {
                file_name = line;
            }

            // Memeriksa apakah file memiliki ekstensi yang valid
            int is_valid = 0;
            int exist = 0;
            for (int i = 0; i < ext_size; i++)
            {
                if (strstr(file_name, extensions[i]) != NULL)
                {
                    exist++;
                    is_valid = 1;
                    break;
                }
            }

            // Menghitung jumlah file dengan jenis ekstensi yg valid
            if (is_valid)
            {
                if (strstr(file_name, ".png") != NULL)
                {
                    png_count++;
                }
                else if (strstr(file_name, ".jpg") != NULL)
                {
                    jpg_count++;
                }
                else if (strstr(file_name, ".emc") != NULL)
                {
                    emc_count++;
                }
                else if (strstr(file_name, ".xyz") != NULL)
                {
                    xyz_count++;
                }
                else if (strstr(file_name, ".js") != NULL)
                {
                    js_count++;
                }
                else if (strstr(file_name, ".py") != NULL)
                {
                    py_count++;
                }
                else if (strstr(file_name, ".txt") != NULL)
                {
                    txt_count++;
                }
            }      
        }
    }

    // Menampilkan hasil penghitungan
    printf("Jumlah file txt: %d\n", txt_count);
    printf("Jumlah file emc: %d\n", emc_count);
    printf("Jumlah file jpg: %d\n", jpg_count);
    printf("Jumlah file png: %d\n", png_count);
    printf("Jumlah file js: %d\n", js_count);
    printf("Jumlah file xyz: %d\n", xyz_count);
    printf("Jumlah file py: %d\n", py_count);

    // Menutup file sementara
    fclose(temp_file);

    // Menghapus file sementara
    // remove("temp.txt");
}

void createLog(const char *message)
{
    FILE *logchecker = fopen("log.txt", "a");

    if (logchecker != NULL)
    {
        time(&current_time);
        timeline = localtime(&current_time);

        strftime(buff, sizeof(buff), "%d-%m-%Y %H:%M:%S", timeline);
        fprintf(logchecker, "%s %s\n", buff, message);
        fclose(logchecker);
    }
}


int main()
{

    pthread_t tid;

    mkdir("categorized", 0700);
    createLog("MADE categorized");

    mkdir("categorized/other", 0700);
    createLog("MADE categorized/other");

    pthread_create(&tid, NULL, categorizeFiles, files_dir);
    pthread_join(tid, NULL);

    printf("Jumlah file : ");
    getAmountOfFiles();    

    int others = countOtherFiles("categorized/other");
    printf("Jumlah isi subdirektori other: %d\n", others);

    return 0;
}
