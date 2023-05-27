#define FUSE_USE_VERSION 31

#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>
#include <openssl/evp.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/time.h>
#include <libgen.h>
#include <signal.h>


// how to compile : gcc -D_FILE_OFFSET_BITS=64 rahasia.c -o rahasiat -lcrypto -lssl -lzip -lcurl -lfuse

static const char* rahasia_path = "/home/reyhanqb/Documents/shift4Bismillah/rahasia";
static const char* user_path = "/home/reyhanqb/Documents/shift4Bismillah/users.txt";
static const char* mount_path = "/home/reyhanqb/Documents/shift4Bismillah/mount_path";

char *extensions[] = {"png", "jpg", "pdf", "txt", "gif", "mp3"};
int extension_size = sizeof(extensions) / sizeof(extensions[0]);

int is_logged_in = 0;
int is_program_running = 1;

struct User {
    char username[100];
    char password[100];
};

void createTree()
{
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "tree %s > result.txt", rahasia_path);

    // bikin tree
    int status = system(cmd);

    // Memeriksa status setelah tree jadi
    if (WIFEXITED(status)) {
        int exit_status = WEXITSTATUS(status);
        if (exit_status != 0) {
            printf("Error. Exit status: %d\n", exit_status);
        } else {
            printf("Sukses membuat file.\n");
        }
    } else {
        printf("Error.\n");
    }
}

static int xmp_getattr(const char *path, struct stat *stbuf)
{
    if(!is_logged_in){
        printf("Log in dulu bang");
    }

    int res;
    char full_path[1000];

    if (strcmp(path, "/") == 0) {
        sprintf(full_path, "%s", rahasia_path);
    } else {
        sprintf(full_path, "%s%s", rahasia_path, path);
    }

    snprintf(full_path, sizeof(full_path), "%s%s", rahasia_path, path);

    res = lstat(full_path, stbuf);
    if (res == -1)
        return -errno;

    return 0;
}

void getAmountOfFiles() {
    // Membuka file baru untuk menulis hasil perhitungan
    FILE* result_file = fopen("extensions.txt", "w");
    if (result_file == NULL) {
        printf("Failed to open extensions.txt file.\n");
        return;
    }

    // Membuat file temp.txt untuk menyimpan output 'ls'
    system("ls -R rahasia > temp.txt");

    // Membuka file sementara
    FILE* temp_file = fopen("temp.txt", "r");
    char line[256];

    int png_count = 0;
    int jpg_count = 0;
    int txt_count = 0;
    int mp3_count = 0;
    int pdf_count = 0;
    int gif_count = 0;
    int subdirectory_count = 0; 

    // Menghitung jumlah file setiap ekstensi dalam urutan ascending
    while (fgets(line, sizeof(line), temp_file)) {
        line[strcspn(line, "\n")] = 0;

        // Memeriksa apakah baris merupakan path direktori
        if (line[strlen(line) - 1] == ':') {
            subdirectory_count++;  // Increment subdirectory count
            continue;
        } else {
            // Memeriksa apakah baris merupakan nama file atau direktori
            char* file_name = strrchr(line, '/');
            if (file_name != NULL) {
                file_name++;
            } else {
                file_name = line;
            }

            // Memeriksa apakah file memiliki ekstensi yang ada
            int is_valid = 0;
            int exist = 0;
            for (int i = 0; i < extension_size; i++) {
                if (strstr(file_name, extensions[i]) != NULL) {
                    exist++;
                    is_valid = 1;
                    break;
                }
            }

            // Menghitung jumlah file dengan jenis ekstensi yang ada
            if (is_valid) {
                if (strstr(file_name, ".png") != NULL) {
                    png_count++;
                } else if (strstr(file_name, ".jpg") != NULL) {
                    jpg_count++;
                } else if (strstr(file_name, ".gif") != NULL) {
                    gif_count++;
                } else if (strstr(file_name, ".pdf") != NULL) {
                    pdf_count++;
                } else if (strstr(file_name, ".txt") != NULL) {
                    txt_count++;
                } else if (strstr(file_name, ".mp3") != NULL) {
                    mp3_count++;
                }
            }
        }
    }

    
    fprintf(result_file, "Jumlah file txt: %d\n", txt_count);
    fprintf(result_file, "Jumlah file gif: %d\n", gif_count);
    fprintf(result_file, "Jumlah file jpg: %d\n", jpg_count);
    fprintf(result_file, "Jumlah file png: %d\n", png_count);
    fprintf(result_file, "Jumlah file pdf: %d\n", pdf_count);
    fprintf(result_file, "Jumlah file mp3: %d\n", mp3_count);
    fprintf(result_file, "Jumlah subdirectories: %d\n", subdirectory_count);  // jumlah subdir

    fclose(temp_file);
    fclose(result_file);

    // Menghapus file temp
    remove("temp.txt");
}



static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    if(!is_logged_in){
        printf("Log in dulu bang");
    }

    (void) offset;
    (void) fi;


    char new_path[1000];

    if (strcmp(path, "/") == 0) {
        sprintf(new_path, "%s", rahasia_path);
    } else {
        sprintf(new_path, "%s%s", rahasia_path, path);
    }

    DIR *dp = opendir(new_path);
    if (dp == NULL)
        return -errno;

    struct dirent *de;
    while ((de = readdir(dp)) != NULL) {
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) {
            continue;
        }

        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;

        if (filler(buf, de->d_name, &st, 0))
            break;
    }

    closedir(dp);
    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    if(!is_logged_in){
        printf("Log in dulu bang");
    }

    (void) fi;

    char new_path[1000];

    if (strcmp(path, "/") == 0) {
        sprintf(new_path, "%s", rahasia_path);
    } else {
        sprintf(new_path, "%s%s", rahasia_path, path);
    }

    int fd = open(new_path, O_RDONLY);
    if (fd == -1)
        return -errno;

    int res = pread(fd, buf, size, offset);
    if (res == -1)
        res = -errno;

    close(fd);
    return res;
}

static int xmp_rename(const char *from, const char *to)
{
    if(!is_logged_in){
        printf("Log in dulu bang");
    }

    // Ambil nama folder dari path awal
    char name[256];
    sscanf(from, "/%[^/]", name);

    char group_code[256];
    sscanf(to, "/%[^_]", group_code);

    // buat nama baru
    char new_name[256];
    sprintf(new_name, "/%s_%s", group_code, name);

    char old_path[256];
    sprintf(old_path, "%s%s", rahasia_path, from);

    char new_path[256];
    sprintf(new_path, "%s%s", rahasia_path, to); 

    struct stat st;
    stat(old_path, &st);

    int is_dir = S_ISDIR(st.st_mode);
    int res;
    if (is_dir) {
        res = rename(old_path, new_path);
        if (res == -1)
            return -errno;

        // Rename folder recursively
        DIR *dp = opendir(new_path); 
        if (dp == NULL)
            return -errno;

        struct dirent *de;
        while ((de = readdir(dp)) != NULL) {
            if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
                continue;

            char old_sub_path[256];
            sprintf(old_sub_path, "%s/%s", old_path, de->d_name);

            char new_sub_path[256];
            sprintf(new_sub_path, "%s/%s", new_path, de->d_name);

            res = xmp_rename(old_sub_path, new_sub_path);
            if (res == -1)
                return -errno;
        }

        closedir(dp);
    } else {
        // Create parent directory 
        char parent_dir[256];
        strncpy(parent_dir, new_path, sizeof(parent_dir));
        parent_dir[sizeof(parent_dir) - 1] = '\0';
        char *parent = dirname(parent_dir);
        mkdir(parent, 0755);

        res = rename(old_path, new_path);
        if (res == -1)
            return -errno;
    }

    return 0;
}


static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .read = xmp_read,
    .rename = xmp_rename,
};

char* md5Make(const char* str) {
    unsigned char digest[MD5_DIGEST_LENGTH];
    EVP_MD_CTX* mdctx;
    const EVP_MD* md = EVP_md5();
    mdctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, str, strlen(str));
    EVP_DigestFinal_ex(mdctx, digest, NULL);
    EVP_MD_CTX_free(mdctx);

    char* hashedStr = (char*)malloc(MD5_DIGEST_LENGTH * 2 + 1);
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
        sprintf(&hashedStr[i * 2], "%02x", (unsigned int)digest[i]);

    return hashedStr;
}

int registerUser(const char* username, const char* password) {
    FILE* file = fopen("users.txt", "a+");
    if (file == NULL) {
        printf("Failed to open users.txt file.\n");
        return 0;
    }

    // Cek username
    struct User user;
    while (fscanf(file, "%[^;];%[^\n]\n", user.username, user.password) != EOF) {
        if (strcmp(username, user.username) == 0) {
            printf("Username already exists.\n");
            fclose(file);
            return 0;
        }
    }

    // Hashing password menggunakan MD5
    char* hashedPassword = md5Make(password);

    // Menyimpan username dan hashed password ke file
    fprintf(file, "%s;%s\n", username, hashedPassword);
    fclose(file);

    printf("User registered successfully.\n");
    return 1;
}

int loginUser(const char* username, const char* password) {
    FILE* file = fopen("users.txt", "r");
    if (file == NULL) {
        printf("Failed to open users.txt file.\n");
        return 0;
    }

    struct User user;
    while (fscanf(file, "%[^;];%[^\n]\n", user.username, user.password) != EOF) {
        if (strcmp(username, user.username) == 0) {
            // Hashing password yang dibandingkan dengan hashed password dalam users.txt
            char* hashedPassword = md5Make(password);
            if (strcmp(hashedPassword, user.password) == 0) {
                fclose(file);
                return 1;  // Login berhasil
            }
            break;  // Username ditemukan, tapi password salah
        }
    }

    fclose(file);
    return 0;  // Username tidak ditemukan / password salah
}

int isFileDownloaded(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0; 
}


void termination_handler(int signum) {
    is_program_running = 0;

    createTree();
    getAmountOfFiles();

    exit(0);
}


char downloadAndUnzip(){
    if (isFileDownloaded("rahasia.zip")) {
        printf("rahasia.zip already exists.\n");
    } else {
        char url[] = "https://drive.google.com/u/0/uc?id=18YCFdG658SALaboVJUHQIqeamcfNY39a&export=download&confirm=yes";
        char cmd[100] = "wget -O rahasia.zip '";
        strcat(cmd, url);
        strcat(cmd, "'");

        system(cmd);  // Download zip file

        system("unzip rahasia.zip");  // Extract zip
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s <-reg|-login> <mount_point>\n", argv[0]);
        return 0;
    }

    if (strcmp(argv[1], "-reg") == 0) {
        downloadAndUnzip();
        char username[100];
        char password[100];
        printf("Enter username: ");
        scanf("%s", username);
        printf("Enter password: ");
        scanf("%s", password);
        registerUser(username, password);
    } else if (strcmp(argv[1], "-login") == 0) {
        downloadAndUnzip();
        char username[100];
        char password[100];
        printf("Enter username: ");
        scanf("%s", username);
        printf("Enter password: ");
        scanf("%s", password);
        int loginStatus = loginUser(username, password);
        if (loginStatus == 1) {
            is_logged_in = 1;
            printf("Login successful.\n");

            // Set up the handler buat bikin tree dan extensions.txt
            signal(SIGINT, termination_handler);

            // Adjust the arguments for fuse_main()
            argv[1] = argv[2]; 
            argv++; 
            argc--;

            // Mount FUSE, sampai distop user (sementara masi ctrl c)
            int fuse_res = fuse_main(argc, argv, &xmp_oper, NULL);

            return fuse_res;
        } else {
            printf("Invalid username or password.\n");
            return 1;
        }
    } else {
        printf("???.\n");
    }
}

