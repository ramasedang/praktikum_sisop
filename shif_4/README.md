# sisop-praktikum-modul-4-2023-mhfg-it14

## Laporan Resmi Modul 4 Praktikum Sistem Operasi 2023
---
### Kelompok ITA14:
Aloysius Bataona Manullang - 5027211008  
Athaya Reyhan Nugroho - 5027211067    
Moh. Sulthan Arief Rahmatullah - 5027211045


---
## Soal 2
## Analisis soal:
Pada soal diminta untuk melakukan program FUSE yang dapat melakukan operasi make folder, rename file dan folder, serta delete file dan folder, dengan  membuat sistem manajemen folder dengan aturan tertentu, seperti tidak dapat menghapus atau mengubah nama file/folder yang memiliki kata "restricted". Lalu untuk mehilangkan aturan tersebut. diperlukan kata "bypass".  Setiap kegiatan yang dilakukan harus tercatat dalam sebuah file logmucatatsini.txt dengan format nama seusai soal.

## Cara pengerjaan soal 1:
Pertama-tama dibuatkan fungsi untuk mengecek folder/file yang mengandung kata restricted :
```c
int is_restricted(const char *path)
{
    // Check if a path contains the word "restricted"
    if (strstr(path, "restricted") != NULL) {
         return -1;
    }
    return 0; // Access granted
} 
```
Lalu, fungsi tersebut akan digunakan di setiap fungsi, seperti fungsi rename, deleta, atau mkdir. Lalu dibuatkan fungsi untuk membaca atribut, file, dan folder :
```c
static int xmp_getattr(const char *path, struct stat *stbuf)
{
    int res;
    char fpath[1000];

    sprintf(fpath,"%s%s",dirpath,path);

    res = lstat(fpath, stbuf);

    if (res == -1) 
        return -errno;

    return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    char fpath[1000];

    if(strcmp(path,"/") == 0)
    {
        path=dirpath;
        sprintf(fpath,"%s",path);
    } else sprintf(fpath, "%s%s",dirpath,path);

    int res = 0;

    DIR *dp;
    struct dirent *de;
    (void) offset;
    (void) fi;

    dp = opendir(fpath);

    if (dp == NULL) 
        return -errno;

    while ((de = readdir(dp)) != NULL) {
        struct stat st;

        memset(&st, 0, sizeof(st));

        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;

        res = (filler(buf, de->d_name, &st, 0));
        
        if(res!=0) 
            break;
    }

    closedir(dp);

    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    char fpath[1000];

    if(strcmp(path,"/") == 0)
    {
        path=dirpath;
        sprintf(fpath,"%s",path);
    } else sprintf(fpath, "%s%s",dirpath,path);

    int res = 0;
    int fd = 0 ;

    (void) fi;

    fd = open(fpath, O_RDONLY);

    if (fd == -1) 
        return -errno;

    res = pread(fd, buf, size, offset);

    if (res == -1) 
        res = -errno;

    close(fd);

    return res;
}

```
Selanjutnya adalah membuat fungsi rename, delete, mkdir, create file :
```c
static int xmp_rename(const char *from, const char *to)
{
    char fpath_from[1000], fpath_to[1000];
    sprintf(fpath_from, "%s%s", dirpath, from);
    sprintf(fpath_to, "%s%s", dirpath, to);

    // Check if the source name contains "restricted"
    if (is_restricted(fpath_from)) {
        // Check if the destination name does not contain "restricted" or "bypass"
        if (strstr(to, "restricted") != NULL || strstr(to, "bypass") == NULL) {
            log_write("FAILED", "RENAME::rxpkisoviet-Rename from %s to %s", fpath_from, fpath_to);
            return -EPERM; // Operation not permitted
        }
    }

    int res = rename(fpath_from, fpath_to);
    if (res == -1) {
        log_write("FAILED", "RENAME::rxpkisoviet-Rename from %s to %s", fpath_from, fpath_to);
        return -errno;
    }
    log_write("SUCCESS", "RENAME::rxpkisoviet-Rename ffrom %s to %s", fpath_from, fpath_to);
    return 0;
}
```
Dapat dilihat pada fungsi di atas, jika file/folder yang memiliki kata "restricted" maka tidak dapat di rename, tetapi dapat direname jika diubah menjadi memiliki kata "bypass" dengan menggunakan if-else ``` if (strstr(to, "restricted") != NULL || strstr(to, "bypass") == NULL)```
```c
static int xmp_unlink(const char *path)
{
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);

    if (is_restricted(fpath)){
        log_write("FAILED", "RMFILE::rxpkisoviet-Remove file %s.", fpath);
        return -EPERM; // Operation not permitted
    }
    int res = unlink(fpath);
    if (res == -1){
        log_write("FAILED", "RMFILE::rxpkisoviet-Remove file %s.", fpath);
        return -errno;
    }
    log_write("SUCCESS", "RMFILE::rxpkisoviet-Remove file %s.", fpath);
    return 0;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
    char fpath[1000];

    // Check if the path contains the word "restricted"
    if (is_restricted(path)) {
        log_write("FAILED", "MKDIR::rxpkisoviet-Create directory %s ", path, fpath);
        return -EPERM; // Operation not permitted
    }

    sprintf(fpath, "%s%s", dirpath, path);

    int res = mkdir(fpath, mode);
    if (res == -1 && is_restricted(path)) {
        log_write("FAILED", "MKDIR::rxpkisoviet-Failed to create directory %s in restricted folder.", fpath);
    }
    log_write("SUCCESS", "MKDIR::rxpkisoviet-Create directory %s.", fpath);
    return 0;
}

static int xmp_rmdir(const char *path)
{
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);
    if (is_restricted(fpath)) {
        log_write("FAILED", "RMDIR::rxpkisoviet-Remove directory %s.", fpath);
        return -EPERM; // Operation not permitted
    }
    int res = rmdir(fpath);
    if (res == -1) {
        log_write("FAILED", "RMDIR::rxpkisoviet-Remove directory %s.", fpath);
        return -errno;
    }
    log_write("SUCCESS", "RMDIR::rxpkisoviet-Remove directory %s.", fpath);
    return 0;
}
static int xmp_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);

    // Check if the path contains the word "restricted"
    if (is_restricted(fpath)) {
        log_write("FAILED", "CREATE::rxpkisoviet-Create file %s.", fpath);
        return -EPERM; // Operation not permitted
    }

    int res = open(fpath, fi->flags, mode);
    if (res == -1) {
        log_write("FAILED", "CREATE::rxpkisoviet-Create file %s.", fpath);
        return -errno;
    }
    fi->fh = res;

    log_write("SUCCESS", "CREATE::rxpkisoviet-Create file %s.", fpath);
    return 0;
}

```
Dapat dilihat juga pada fungsi lain-lainnya, sudah memanggil fungsi is_restricted agar tidak dapat menghapus atau mengganti nama pada file/folder yang mengandung kata "restricted". Selanjutnya adalah membuat file log untuk setiap kegiatan tersebut :
```c
void log_write(char *lvl, char *message, ...)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    FILE *f = fopen(logpath, "a");
    va_list args;
    va_start(args, message);
    fprintf(f, "%s::%02d/%02d/%04d-%02d:%02d:%02d::", lvl, tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
    vfprintf(f, message, args);
    fprintf(f, "\n");
    fclose(f);
    va_end(args);
}
```
Pada fungsi di atas, menggunakan library time agar mendapatkan localtime, lalu fungsi log_write tersebut akan dipanggil pada setiap fungsi rename, delete, atau mkdir. Lalu fungsi-fungsi di atas akan dimasukkan ke dalam fungsi fuse_operation :
```c
static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .rename = xmp_rename,
    .unlink = xmp_unlink,
    .read = xmp_read,
    .mkdir = xmp_mkdir,
    .rmdir = xmp_rmdir,
    .create = xmp_create,
};
```
Dan terakhir, fungsi tersebut akan dipanggil di int main() :
```c

int main(int argc, char *argv[])
{
    umask(0);

    return fuse_main(argc, argv, &xmp_oper, NULL);
}
```

## Source code
```c
#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <stdarg.h>

static const char *dirpath = "/home/rxpkisoviet/Documents";
static const char *logpath = "/home/rxpkisoviet/Documents/logmucatatsini.txt";

void log_write(char *lvl, char *message, ...);
int is_restricted(const char *path);
static int xmp_getattr(const char *path, struct stat *stbuf);
static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
static int xmp_rename(const char *from, const char *to);
static int xmp_unlink(const char *path);
static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
static int xmp_mkdir(const char *path, mode_t mode);
static int xmp_rmdir(const char *path);
static int xmp_create(const char *path, mode_t mode, struct fuse_file_info *fi);


static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .rename = xmp_rename,
    .unlink = xmp_unlink,
    .read = xmp_read,
    .mkdir = xmp_mkdir,
    .rmdir = xmp_rmdir,
    .create = xmp_create,
};



int main(int argc, char *argv[])
{
    umask(0);

    return fuse_main(argc, argv, &xmp_oper, NULL);
}

void log_write(char *lvl, char *message, ...)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    FILE *f = fopen(logpath, "a");
    va_list args;
    va_start(args, message);
    fprintf(f, "%s::%02d/%02d/%04d-%02d:%02d:%02d::", lvl, tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
    vfprintf(f, message, args);
    fprintf(f, "\n");
    fclose(f);
    va_end(args);
}

int is_restricted(const char *path)
{
    // Check if a path contains the word "restricted"
    if (strstr(path, "restricted") != NULL) {
         return -1;
    }
    return 0; // Access granted
}


static int xmp_getattr(const char *path, struct stat *stbuf)
{
    int res;
    char fpath[1000];

    sprintf(fpath,"%s%s",dirpath,path);

    res = lstat(fpath, stbuf);

    if (res == -1) 
        return -errno;

    return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    char fpath[1000];

    if(strcmp(path,"/") == 0)
    {
        path=dirpath;
        sprintf(fpath,"%s",path);
    } else sprintf(fpath, "%s%s",dirpath,path);

    int res = 0;

    DIR *dp;
    struct dirent *de;
    (void) offset;
    (void) fi;

    dp = opendir(fpath);

    if (dp == NULL) 
        return -errno;

    while ((de = readdir(dp)) != NULL) {
        struct stat st;

        memset(&st, 0, sizeof(st));

        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;

        res = (filler(buf, de->d_name, &st, 0));
        
        if(res!=0) 
            break;
    }

    closedir(dp);

    return 0;
}

static int xmp_rename(const char *from, const char *to)
{
    char fpath_from[1000], fpath_to[1000];
    sprintf(fpath_from, "%s%s", dirpath, from);
    sprintf(fpath_to, "%s%s", dirpath, to);

    // Check if the source name contains "restricted"
    if (is_restricted(fpath_from)) {
        // Check if the destination name does not contain "restricted" or "bypass"
        if (strstr(to, "restricted") != NULL || strstr(to, "bypass") == NULL) {
            log_write("FAILED", "RENAME::rxpkisoviet-Rename from %s to %s", fpath_from, fpath_to);
            return -EPERM; // Operation not permitted
        }
    }

    int res = rename(fpath_from, fpath_to);
    if (res == -1) {
        log_write("FAILED", "RENAME::rxpkisoviet-Rename from %s to %s", fpath_from, fpath_to);
        return -errno;
    }
    log_write("SUCCESS", "RENAME::rxpkisoviet-Rename ffrom %s to %s", fpath_from, fpath_to);
    return 0;
}



static int xmp_unlink(const char *path)
{
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);

    if (is_restricted(fpath)){
        log_write("FAILED", "RMFILE::rxpkisoviet-Remove file %s.", fpath);
        return -EPERM; // Operation not permitted
    }
    int res = unlink(fpath);
    if (res == -1){
        log_write("FAILED", "RMFILE::rxpkisoviet-Remove file %s.", fpath);
        return -errno;
    }
    log_write("SUCCESS", "RMFILE::rxpkisoviet-Remove file %s.", fpath);
    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    char fpath[1000];

    if(strcmp(path,"/") == 0)
    {
        path=dirpath;
        sprintf(fpath,"%s",path);
    } else sprintf(fpath, "%s%s",dirpath,path);

    int res = 0;
    int fd = 0 ;

    (void) fi;

    fd = open(fpath, O_RDONLY);

    if (fd == -1) 
        return -errno;

    res = pread(fd, buf, size, offset);

    if (res == -1) 
        res = -errno;

    close(fd);

    return res;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
    char fpath[1000];

    // Check if the path contains the word "restricted"
    if (is_restricted(path)) {
        log_write("FAILED", "MKDIR::rxpkisoviet-Create directory %s ", path, fpath);
        return -EPERM; // Operation not permitted
    }

    sprintf(fpath, "%s%s", dirpath, path);

    int res = mkdir(fpath, mode);
    if (res == -1 && is_restricted(path)) {
        log_write("FAILED", "MKDIR::rxpkisoviet-Failed to create directory %s in restricted folder.", fpath);
    }
    log_write("SUCCESS", "MKDIR::rxpkisoviet-Create directory %s.", fpath);
    return 0;
}

static int xmp_rmdir(const char *path)
{
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);
    if (is_restricted(fpath)) {
        log_write("FAILED", "RMDIR::rxpkisoviet-Remove directory %s.", fpath);
        return -EPERM; // Operation not permitted
    }
    int res = rmdir(fpath);
    if (res == -1) {
        log_write("FAILED", "RMDIR::rxpkisoviet-Remove directory %s.", fpath);
        return -errno;
    }
    log_write("SUCCESS", "RMDIR::rxpkisoviet-Remove directory %s.", fpath);
    return 0;
}
static int xmp_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);

    // Check if the path contains the word "restricted"
    if (is_restricted(fpath)) {
        log_write("FAILED", "CREATE::rxpkisoviet-Create file %s.", fpath);
        return -EPERM; // Operation not permitted
    }

    int res = open(fpath, fi->flags, mode);
    if (res == -1) {
        log_write("FAILED", "CREATE::rxpkisoviet-Create file %s.", fpath);
        return -errno;
    }
    fi->fh = res;

    log_write("SUCCESS", "CREATE::rxpkisoviet-Create file %s.", fpath);
    return 0;
}


```

## Test output
Di bawah ini adalah test output dari logmucatatsini.txt :
![image](https://github.com/ramasedang/praktikum_sisop/assets/100351038/3ea67305-efca-42b2-8b63-a34ba12dacb2)


## Kendala
Kendala dalam mengerjakan soal adalah kurang teliti sehingga sering terjadi kesalapahaman dengan soal.



# Soal 5

## Analisis Soal

Pada soal diminta untuk melakukan download dan unzip folder rahasia, lalu dimount menggunakan FUSE. Folder yang dimount hanya bisa diakses oleh user yang sudah login, dan user dapat melakukan rename ke file dan folder yang terdapat didalamnya dengan format nama <Nama_Folder>_<Kode_Kelompok> dan <Kode_Kelompok>_<Nama_File>.<ext>. Setelahnya, diminta untuk membuat file result.txt yang berisi list seluruh folder, subfolder, dan file menggunakan tree kemudian hitung file tersebut berdasarkan extension dan output-kan menjadi extension.txt.

## Pengerjaan

Pertama-tama buat fungsi xmp yang diperlukan. Untuk pengerjaan soal ini, digunakan 4 fungsi FUSE yaitu getattr, rename, read, dan readdir. Berikut ini merupakan fungsi getattr 
```c
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
```
Pada tiap fungsi FUSE, diberikan flag is_logged_in untuk melakukan pengecekan apakah user sudah login atau belum. Selain itu dilakukan pengecekan apakah path saat ini merupakan base path atau bukan lewat kondisional ```if (strcmp(path, "/") == 0)``` agar kode dapat mengakses direktori rahasia


Kemudian dibuat fungsi read dan readdir
```c
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
```
Untuk membaca file dan subdirektori yang ada didalam direktori rahasia menggunakan operasi FUSE. Terakhir, operasi FUSE yang digunakan adalah rename

```c
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
```
Fungsi ini membutuhkan dua parameter yaitu to dan from. Parameter ini digunakan untuk merename file dan subdirektori pada FUSE. Pada fungsi ini juga dilakukan pengecekan apakah operasi rename akan dilakukan pada subdirektori apa file lewat kondisional ```int is_dir = S_ISDIR(st.st_mode);```. Apabila operasi rename dilakukan kepada subdirektori, maka penamaan akan dilakukan secara rekursif. 

Selanjutnya juga dibuat fungsi untuk register dan login. Sesuai dengan ketentuan pada soal, password akan dihash dengan metode MD5. Berikut merupakan fungsi untuk hash password dengan metode MD5

```c
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
```

hashedStr yang dihasilkan akan disimpan dalam file .txt sesuai dengan format yang diberikan pada soal. Kemudian dibuat fungsi untuk register dan login.

```c
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

```
Pada fungsi register dibuat kondisional ```if (strcmp(username, user.username) == 0)``` untuk cek apakah username sudah terdaftar atau belum. 
```c
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
```
Kemudian pada fungsi login, dilakukan pengecekan apakah password yang diinput user sama atau tidak dengan hashed password pada kondisional ```if (strcmp(hashedPassword, user.password) == 0)```.


Selanjutnya dibuat fungsi untuk download dan unzip file serta fungsi untuk cek apakah file rahasia sudah didownload.

```c
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

int isFileDownloaded(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0; 
}
```
Pada program juga dibuat fungsi untuk melakukan checking termination, agar fungsi untuk membuat tree pada result.txt dan file extension.txt hanya dijalankan apabila program sudah diterminate oleh user. Berikut ini merupakan fungsi untuk membuat tree dan file extension.txt serta termination handler.\

```c
// Membuat tree
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
```
```c
//  Menghitung file berdasarkan extension
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
```
```c
// Termination handler
void termination_handler(int signum) {
    is_program_running = 0;

    createTree();
    getAmountOfFiles();
    
    char umount_cmd[256];
    snprintf(umount_cmd, sizeof(umount_cmd), "umount -u %s", mount_path);
    system(umount_cmd);

    exit(0);
}
```


Terakhir, berikut adalah fungsi main() dari program. 
```c
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

            char umount_cmd[256];
            snprintf(umount_cmd, sizeof(umount_cmd), "umount %s", mount_path);
            system(umount_cmd);



            return fuse_res;
        } else {
            printf("Invalid username or password.\n");
            return 1;
        }
    } else {
        printf("???.\n");
    }
}
```

Pada fungsi main(), user harus menginput opsi yang dipilih yaitu -reg untuk register dan -login untuk login. Dapat dilihat pada kode bahwa FUSE hanya akan dimount apabila login berhasil. Program akan terus berjalan hingga diterminate oleh user, lalu akan dibuat file result.txt dan extensions.txt setelah program diterminate.


# Source code

```c
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
// how to run : sudo ./rahasia <-login | -reg > <mount_point>

static const char* rahasia_path = "/home/reyhanqb/Documents/shift4Bismillah/rahasia";
static const char* user_path = "/home/reyhanqb/Documents/shift4Bismillah/users.txt";
static const char* mount_path = "./mount_path";

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
    
    char umount_cmd[256];
    snprintf(umount_cmd, sizeof(umount_cmd), "umount -u %s", mount_path);
    system(umount_cmd);

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

            char umount_cmd[256];
            snprintf(umount_cmd, sizeof(umount_cmd), "umount %s", mount_path);
            system(umount_cmd);



            return fuse_res;
        } else {
            printf("Invalid username or password.\n");
            return 1;
        }
    } else {
        printf("???.\n");
    }
}

```

# Test Output

Output saat user register
![Register](https://github.com/ramasedang/praktikum_sisop/assets/107137535/4aacaa68-0364-4e76-8510-b384832dd6fe)

Run saat user login
![Login](https://github.com/ramasedang/praktikum_sisop/assets/107137535/284c76e7-c4b5-41e2-bde3-5fa8107e0e33)

Isi dari result.txt
![Output result.txt](https://github.com/ramasedang/praktikum_sisop/assets/107137535/a9bb6fa7-e7e3-4281-93e7-b90661659f44)

Isi dari extensions.txt
![Output extensions.txt](https://github.com/ramasedang/praktikum_sisop/assets/107137535/1bf78e62-6edf-4290-9e4a-356bf479c5d8)
 
# Kendala

Terdapat kendala pada saat melakukan unmount setelah user terminate program dan beberapa kali mountpoint dari FUSE isinya kosong.
