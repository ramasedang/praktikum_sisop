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

