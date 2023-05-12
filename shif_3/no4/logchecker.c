#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

char line[1000];
int file_count = 0;
struct dirent *entry;

int main() {
    char *log_file_name = "logs.txt";
    char *dir_path = "/home/reyhanqb/Documents/Shift 3/soal4";

    // Ekstrak isi file logs.txt
    FILE *log_file = fopen(log_file_name, "r");
    if (log_file == NULL) {
        printf("Gagal membuka file %s\n", log_file_name);
        return 1;
    }

    int times_accessed = 0;
    int times_made = 0;
    int times_moved = 0;
    while (fgets(line, sizeof(line), log_file)) {
        if (strstr(line, "ACCESSED") != NULL) {
            times_accessed++;
        }
        else if(strstr(line, "MADE") != NULL){
            times_made++;
        }
        else if(strstr(line, "MOVED") != NULL){
            times_moved++;
        }
        printf("%s", line);
    }
    fclose(log_file);

    // Hitung jumlah file di direktori
    DIR *dir = opendir(dir_path);
    if (dir == NULL) {
        printf("Gagal membuka direktori %s\n", dir_path);
        return 1;
    }
    
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            file_count++;
        }
    }
    closedir(dir);

    printf("Jumlah akses pada direktori: %d\n", times_accessed);
    printf("Jumlah akses pada direktori: %d\n", times_moved);
    printf("Jumlah akses pada direktori: %d\n", times_made);
    printf("Jumlah file di direktori %s: %d\n", dir_path, file_count);

    return 0;
}
