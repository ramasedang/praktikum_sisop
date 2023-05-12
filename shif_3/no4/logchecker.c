#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <stddef.h>

char line[1000], directory_path[1000];
int file_count = 0;
struct dirent *entry;

char *extensions[] = {"jpg", "txt", "js", "py", "png", "emc", "xyz"};
int ext_size = sizeof(extensions) / sizeof(extensions[0]);

int countTotalFiles(const char *current_dir_path)
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

    int others = countTotalFiles("categorized/other");

    int total = others + txt_count + emc_count + jpg_count + png_count + js_count + xyz_count + py_count;


    printf("Jumlah file dalam direktori categorize : %d\n", total);

    // Menutup file sementara
    fclose(temp_file);

    // Menghapus file sementara
    remove("temp.txt");
}


int main() {
    char *log_file_name = "log.txt";
    char *dir_path = "./categorized";

    // Ekstrak isi file log.txt
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

    printf("Jumlah ACCESSED pada direktori: %d\n", times_accessed);
    printf("Jumlah MOVED pada direktori: %d\n", times_moved);
    printf("Jumlah CREATED pada direktori: %d\n", times_made);
    printf("Jumlah file untuk tiap extension : \n");
    getAmountOfFiles();



    return 0;
}
