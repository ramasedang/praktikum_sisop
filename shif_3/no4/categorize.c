#include <stdlib.h>
#include <libgen.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <fnmatch.h>
#include <pthread.h>

#define extensions 100
#define extensionsLength 10


char *pathToCategorized = "/home/reyhanqb/Documents/Shift 3/soal4";
char *currentPath = "/home/reyhanqb/Documents/Shift 3/soal4/files"; 

struct dirent *entry;
char prevPath[1000], newPath[1000], filenames[1000], sub_currentPath[1000];
char extensionsLists[extensions][extensionsLength]; 
int ext_count = 0;

void categorizeFiles(char *currentPath, char extensionsLists[extensions][extensionsLength], int ext_count) {
    DIR *dir = opendir(currentPath);
    int iterations = 0;
    int i = 0;

    if((entry = readdir(dir)) == NULL){
        printf("Direktori kosong");
    }
    
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, "files") == 0 ) {
                continue;
            }
            sprintf(sub_currentPath, "%s/%s", currentPath, entry->d_name);
            categorizeFiles(sub_currentPath, extensionsLists, ext_count); 
        } else if (entry->d_type == DT_REG) {
            // Check kalau entry ke direktori benar
            char* filename = entry->d_name;

            // ubah nama file ke lowercase
            int j = 0;
            while (filename[j] != '\0') {
                filenames[j] = tolower(filename[j]);
                j++;
            }

            filenames[strlen(filename)] = '\0';
            while (i < ext_count) {
                int result = mkdir(extensionsLists[i], 0777);
                char ext[extensionsLength + 2];
                sprintf(ext, "*.%s", extensionsLists[i]);
                            
                if (fnmatch(ext, filenames, 0) == 0) {
                    sprintf(prevPath, "%s/%s", currentPath, filename); 
                    // Tentukan path tujuan file dari dir files 
                    sprintf(newPath, "%s/%s/%s/%s", pathToCategorized, "categorized", extensionsLists[i], filename); 
                    rename(prevPath, newPath); 
                    iterations = 1;
                    break;
                }
                
                i++;
            }

            if (!iterations) {  
                // Panggil path dari file yang tdk punya extension sesuai    
                sprintf(prevPath, "%s/%s", currentPath, filename);
                sprintf(newPath, "%s/%s/%s", pathToCategorized, "categorized/other", filename); 
                rename(prevPath, newPath);
            }
        }
    }
    closedir(dir);
}


int getExtensions(){
    // Buka file extensions.txt
    FILE* fp = fopen("extensions.txt", "r");
    while (fgets(extensionsLists[ext_count], extensionsLength, fp)) {
    //Buat iterasi dari isi extensions.txt untuk menentukan file yang harus dibuat 
      extensionsLists[ext_count][strcspn(extensionsLists[ext_count], "\r\n")] = 0; 
      ext_count++;
      if (ext_count >= extensions) {
        return 1;
      }
    }
    fclose(fp);
}


int countCategorize(char *currentPath) {
DIR *dir = opendir(currentPath);
int count = 0;
struct dirent *entry;

// Check apakah direktori saat ini merupakan direktori categorized
switch (strstr(currentPath, "categorized") == NULL) {
    case 1:
        return 0;
    default:
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_DIR) {
                // Abaikan direktori berikut
                switch (strcmp(entry->d_name, ".")) {
                    case 0:
                        continue;
                    default:
                        break;
                }
                switch (strcmp(entry->d_name, "..")) {
                    case 0:
                        continue;
                    default:
                        break;
                }
                switch (strcmp(entry->d_name, "/files/")) {
                    case 0:
                        continue;
                    default:
                        break;
                }
                sprintf(sub_currentPath, "%s/%s", currentPath, entry->d_name);
                int sub_count = countCategorize(sub_currentPath); // memanggil diri sendiri secara rekursif
                printf("%s: %d\n", sub_currentPath, sub_count);
                count += sub_count;
            } else {
                count++;
            }
        }
        closedir(dir);
        printf("\nJumlah isi tiap direktori: %d", &count);
        return count;
}
}

int main() {
    // list ekstensi file yang akan diproses

    getExtensions();
    
    // Masuk ke direktori soal 4
    chdir("/home/reyhanqb/Documents/Shift 3/soal4");
    mkdir("categorized", 0777);
    
    // Masuk kedalam direktori categorized
    chdir("/home/reyhanqb/Documents/Shift 3/soal4/categorized/");
    
    // Buat direktori other
    mkdir("other", 0777); 

    categorizeFiles(currentPath, extensionsLists, ext_count);

    countCategorize(pathToCategorized);

    return 0;
}
