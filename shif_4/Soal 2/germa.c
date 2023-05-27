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

//membuat file log
void fuse_log_write(char *lvl, char *message, ...);
//melakukan pengecekan dengan kata restricted
int fuse_is_restricted(const char *path);
//mendapatkan file atribut
static int fuse_xmp_getattr(const char *path, struct stat *stbuf);
//membaca direktori
static int fuse_xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
//mengubah nama file atau direktori
static int fuse_xmp_rename(const char *from, const char *to);
//menghapus file
static int fuse_xmp_unlink(const char *path);
//membaca file
static int fuse_xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
//membuat folder
static int fuse_xmp_mkdir(const char *path, mode_t mode);
//menghapus folder
static int fuse_xmp_rmdir(const char *path);
//membuat file
static int fuse_xmp_create(const char *path, mode_t mode, struct fuse_file_info *fi);

//operasi fuse
static struct fuse_operations fuse_xmp_oper = {
    .getattr = fuse_xmp_getattr,
    .readdir = fuse_xmp_readdir,
    .rename = fuse_xmp_rename,
    .unlink = fuse_xmp_unlink,
    .read = fuse_xmp_read,
    .mkdir = fuse_xmp_mkdir,
    .rmdir = fuse_xmp_rmdir,
    .create = fuse_xmp_create,
};



int main(int argc, char *argv[])
{
    umask(0);
    //memanggil fungsi fuse
    return fuse_main(argc, argv, &xmp_oper, NULL);
}

//membuat log 
void fuse_log_write(char *lvl, char *message, ...)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    FILE *f = fopen(logpath, "a");
    va_list args;
    va_start(args, message);
    //memprnt time
    fprintf(f, "%s::%02d/%02d/%04d-%02d:%02d:%02d::", lvl, tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
    vfprintf(f, message, args);
    fprintf(f, "\n");
    fclose(f);
    va_end(args);
}

//mengecek kata restricted
int fuse_is_restricted(const char *path)
{
    // Check if a path contains the word "restricted"
    if (strstr(path, "restricted") != NULL) {
         return -1;
    }
    return 0; // Access granted
}

//mengambil file atribut
static int fuse_xmp_getattr(const char *path, struct stat *stbuf)
{
    int res;
    char fpath[1000];

    sprintf(fpath,"%s%s",dirpath,path);

    res = lstat(fpath, stbuf);

    if (res == -1) 
        return -errno; //akses tidak diizinkan

    return 0;
}

//membaca direktori
static int fuse_xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
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

//mengubah nama file dan direktori
static int fuse_xmp_rename(const char *from, const char *to)
{
    char fpath_from[1000], fpath_to[1000];
    sprintf(fpath_from, "%s%s", dirpath, from);
    sprintf(fpath_to, "%s%s", dirpath, to);

    // Check if the source name contains "restricted"
    if (fuse_is_restricted(fpath_from)) {
        // Check if the destination name does not contain "restricted" or "bypass"
        if (strstr(to, "restricted") != NULL || strstr(to, "bypass") == NULL) {
            fuse_log_write("FAILED", "RENAME::rxpkisoviet-Rename from %s to %s", fpath_from, fpath_to);
            return -EPERM; // Operation not permitted
        }
    }

    int res = rename(fpath_from, fpath_to);
    if (res == -1) {
        fuse_log_write("FAILED", "RENAME::rxpkisoviet-Rename from %s to %s", fpath_from, fpath_to);
        return -errno;
    }
    fuse_log_write("SUCCESS", "RENAME::rxpkisoviet-Rename ffrom %s to %s", fpath_from, fpath_to);
    return 0;
}


//menghapus file
static int fuse_xmp_unlink(const char *path)
{
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);
    //mengecek kata restricted
    if (fuse_is_restricted(fpath)){
        log_write("FAILED", "RMFILE::rxpkisoviet-Remove file %s.", fpath);
        return -EPERM; // Operation not permitted
    }
    int res = unlink(fpath);
    if (res == -1){
        fuse_log_write("FAILED", "RMFILE::rxpkisoviet-Remove file %s.", fpath);
        return -errno;
    }
    fuse_log_write("SUCCESS", "RMFILE::rxpkisoviet-Remove file %s.", fpath);
    return 0;
}

//membaca file
static int fuse_xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
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

//membuat direktori
static int fuse_xmp_mkdir(const char *path, mode_t mode)
{
    char fpath[1000];

    // Check if the path contains the word "restricted"
    if (fuse_is_restricted(path)) {
        fuse_log_write("FAILED", "MKDIR::rxpkisoviet-Create %s.", fpath);
        return -EPERM; // Operation not permitted
    }

    sprintf(fpath, "%s%s", dirpath, path);

    int res = mkdir(fpath, mode);
    if (res == -1) {
        fuse_log_write("FAILED", "MKDIR::rxpkisoviet-Create directory %s.", fpath);
        return -errno;
    }
    fuse_log_write("SUCCESS", "MKDIR::rxpkisoviet-Create directory %s.", fpath);
    return 0;
}

//menghapus direktori
static int fuse_xmp_rmdir(const char *path)
{
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);
    //mengecek kata restricted
    if (fuse_is_restricted(fpath)) {
        fuse_log_write("FAILED", "RMDIR::rxpkisoviet-Remove directory %s.", fpath);
        return -EPERM; // Operation not permitted
    }
    int res = rmdir(fpath);
    if (res == -1) {
        fuse_log_write("FAILED", "RMDIR::rxpkisoviet-Remove directory %s.", fpath);
        return -errno;
    }
    fuse_log_write("SUCCESS", "RMDIR::rxpkisoviet-Remove directory %s.", fpath);
    return 0;
}

//membuat file
static int fuse_xmp_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);

    // Check if the path contains the word "restricted"
    if (fuse_is_restricted(fpath)) {
        fuse_log_write("FAILED", "CREATE::rxpkisoviet-Create file %s.", fpath);
        return -EPERM; // Operation not permitted
    }

    int res = open(fpath, fi->flags, mode);
    if (res == -1) {
        fuse_log_write("FAILED", "CREATE::rxpkisoviet-Create file %s.", fpath);
        return -errno;
    }
    fi->fh = res;

    fuse_log_write("SUCCESS", "CREATE::rxpkisoviet-Create file %s.", fpath);
    return 0;
}

