#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <pthread.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fnmatch.h>
#include <errno.h>

#define extensions 100
#define extensionsLength 10


char *pathToCategorized = "/home/reyhanqb/Documents/Shift 3/soal4";
char *currentPath = "/home/reyhanqb/Documents/Shift 3/soal4/files"; 

struct dirent *entry;
char prevPath[1000], newPath[1000], filenames[1000];
char ext_list[extensions][extensionsLength]; 
int ext_count = 0;
char prevPath[1000], newPath[1000], sub_currentPath[1000];
char filenames[1000];

int getExtensions(){
    
    FILE* fp = fopen("extensions.txt", "r");

    while (fgets(ext_list[ext_count], extensionsLength, fp)) {
      ext_list[ext_count][strcspn(ext_list[ext_count], "\r\n")] = 0; // hapus karakter '\r' dan '\n'
      ext_count++;
      if (ext_count >= extensions) {
        return 1;
      }
    }

    fclose(fp);

}

void categorizeFiles(char *currentPath, char ext_list[extensions][extensionsLength], int ext_count) {
    DIR *dir = opendir(currentPath);

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            char sub_currentPath[1000];
            sprintf(sub_currentPath, "%s/%s", currentPath, entry->d_name);
            categorizeFiles(sub_currentPath, ext_list, ext_count); 
        } else if (entry->d_type == DT_REG) {
            char* filename = entry->d_name;
            char filenames[1000];

            // konversi nama file ke lowercase
            for (int j = 0; filename[j] != '\0'; j++) {
                filenames[j] = tolower(filename[j]);
            }
            filenames[strlen(filename)] = '\0';

            int iterations = 0;
            for (int i = 0; i < ext_count; i++) {
                int result = mkdir(ext_list[i], 0777);
            
                char ext[extensionsLength + 2]; // tambahkan 2 karakter untuk "*.ext" dan terminasi string
                sprintf(ext, "*.%s", ext_list[i]);
                
                if (fnmatch(ext, filenames, 0) == 0) {
                
                    sprintf(prevPath, "%s/%s", currentPath, filename); // membuat path lengkap file asli
                    sprintf(newPath, "%s/%s/%s/%s", pathToCategorized, "categorized", ext_list[i], filename); // membuat path lengkap file tujuan


                    rename(prevPath, newPath); 
                    iterations = 1;
                    break;
                }
            }

            if (!iterations) {                 
                sprintf(prevPath, "%s/%s", currentPath, filename); 
                sprintf(newPath, "%s/%s/%s", pathToCategorized, "categorized/other", filename); 

                rename(prevPath, newPath);
            }
        }
    }

    closedir(dir);
}



int count_files(char *currentPath) {
    DIR *dir = opendir(currentPath);
    int count = 0;

     if (strstr(currentPath, "categorized") == NULL) { // tambahkan kondisi untuk hanya menghitung isi dari direktori categorized
        return 0;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, "/files/") == 0 ) {
                continue;
            }
            char sub_currentPath[1000];
            sprintf(sub_currentPath, "%s/%s", currentPath, entry->d_name);
            int sub_count = count_files(sub_currentPath); // memanggil diri sendiri secara rekursif
            printf("%s: %d\n", sub_currentPath, sub_count);
            count += sub_count;
        } else {
            count++;
        }
    }

    closedir(dir);

    return count;
}

int main() {
    // list ekstensi file yang akan diproses

    getExtensions();
    
    
    chdir("/home/reyhanqb/Documents/Shift 3/soal4");
    int f = mkdir("categorized", 0777);
    
    chdir("/home/reyhanqb/Documents/Shift 3/soal4/categorized/");
    
    int other_result = mkdir("other", 0777); // tambahkan untuk membuat folder 'other'

    categorizeFiles(currentPath, ext_list, ext_count);

    printf("\nJumlah isi tiap direktori:\n");
    count_files(pathToCategorized);

    return 0;
}
