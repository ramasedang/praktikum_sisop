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

static const char *directorypath = "/home/rxpkisoviet/Documents";
static const char *logpath = "/home/rxpkisoviet/Documents/logmucatatsini.txt";

//membuat file log
void fuse_log_write(char *lvl, char *message, ...);
//melakukan pengecekan dengan kata restricted
int fuse_is_restricted(const char *path);
//mendapatkan file atribut
static int fuse_getattr(const char *path, struct stat *stbuf);
//membaca direktori
static int fuse_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
//mengubah nama file atau direktori
static int fuse_rename(const char *from, const char *to);
//menghapus file
static int fuse_unlink(const char *path);
//membaca file
static int fuse_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
//membuat folder
static int fuse_mkdir(const char *path, mode_t mode);
//menghapus folder
static int fuse_rmdir(const char *path);
//membuat file
static int fuse_create(const char *path, mode_t mode, struct fuse_file_info *fi);

//operasi fuse
static struct fuse_operations fuse_oper = {
    .getattr = fuse_getattr,
    .readdir = fuse_readdir,
    .rename = fuse_rename,
    .unlink = fuse_unlink,
    .read = fuse_read,
    .mkdir = fuse_mkdir,
    .rmdir = fuse_rmdir,
    .create = fuse_create,
};



int main(int argc, char *argv[])
{
    umask(0);
    //memanggil fungsi fuse
    return fuse_main(argc, argv, &oper, NULL);
}

//membuat log 
void fuse_log_write(char *lvl, char *message, ...)
{
    //   va_list args;
    // va_start(args, message);
    time_t t = time(NULL);
    //mengambil localtime
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
        //tidak memberikan akses
         return -1;
    }
    return 0; // Access granted
}

//mengambil file atribut
static int fuse_getattr(const char *path, struct stat *stbuf)
{
    int res;
    
    char folderpath[1000];

    sprintf(folderpath,"%s%s",directorypath,path);

    res = lstat(folderpath, stbuf);

    if (res == -1) 
        return -errno; //akses tidak diizinkan

    return 0;
}

//membaca direktori
static int fuse_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    char folderpath[1000];

//    if(strcmp(path,"/") == 0)
//     {
//         path=directorypath;
//         printf(folderpath,"%s",path);
//     }  sprintf(folderpath, "%s%s",directorypath,path);


    if(strcmp(path,"/") == 0)
    {
        path=directorypath;
        sprintf(folderpath,"%s",path);
    } else sprintf(folderpath, "%s%s",directorypath,path);

    int res = 0;

    DIR *dp;
    struct dirent *de;
    (void) offset;
    (void) fi;

    dp = opendir(folderpath);

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
static int fuse_rename(const char *from, const char *to)
{
    char folderpath_from[1000], folderpath_to[1000];
    sprintf(folderpath_from, "%s%s", directorypath, from);
    sprintf(folderpath_to, "%s%s", directorypath, to);

    // Check if the source name contains "restricted"
    if (fuse_is_restricted(folderpath_from)) {
        // Check if the destination name does not contain "restricted" or "bypass"
        if (strstr(to, "restricted") != NULL || strstr(to, "bypass") == NULL) {
            fuse_log_write("FAILED", "RENAME::rxpkisoviet-Rename from %s to %s", folderpath_from, folderpath_to);
            return -EPERM; // Operation not permitted
        }
    }

    int res = rename(folderpath_from, folderpath_to);
    if (res == -1) {
        //gagal melakukan rename
        fuse_log_write("FAILED", "RENAME::rxpkisoviet-Rename from %s to %s", folderpath_from, folderpath_to);
        return -errno;
    }
    //sukses melakukan rename
    fuse_log_write("SUCCESS", "RENAME::rxpkisoviet-Rename ffrom %s to %s", folderpath_from, folderpath_to);
    return 0;
}


//menghapus file
static int fuse_unlink(const char *path)
{
    char folderpath[1000];
    sprintf(folderpath, "%s%s", directorypath, path);
    //mengecek kata restricted
    if (fuse_is_restricted(folderpath)){
        log_write("FAILED", "RMFILE::rxpkisoviet-Remove file %s.", folderpath);
        return -EPERM; // Operation not permitted
    }
    int res = unlink(folderpath);
    if (res == -1){
        //gagal melakukan remove file
        fuse_log_write("FAILED", "RMFILE::rxpkisoviet-Remove file %s.", folderpath);
        return -errno;
    }
    //berhasil melakukan fungsi remove file
    fuse_log_write("SUCCESS", "RMFILE::rxpkisoviet-Remove file %s.", folderpath);
    return 0;
}

//membaca file
static int fuse_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    char folderpath[1000];

    if(strcmp(path,"/") == 0)
    {
        path=directorypath;
        sprintf(folderpath,"%s",path);
    } else sprintf(folderpath, "%s%s",directorypath,path);

    int res = 0;
    int fd = 0 ;

    (void) fi;

    fd = open(folderpath, O_RDONLY);

    if (fd == -1) 
        return -errno;

    res = pread(fd, buf, size, offset);

    if (res == -1) 
        res = -errno;

    close(fd);

    //  int res = 0;
    // int fd = 0 ;

    // (void) fi;

    // fd = open(folderpath, O_RDONLY);

    return res;
}

//membuat direktori
static int fuse_mkdir(const char *path, mode_t mode)
{
    char folderpath[1000];

    // Check if the path contains the word "restricted"
    if (fuse_is_restricted(path)) {
        fuse_log_write("FAILED", "MKDIR::rxpkisoviet-Create %s.", folderpath);
        return -EPERM; // Operation not permitted
    }

    sprintf(folderpath, "%s%s", directorypath, path);

    int res = mkdir(folderpath, mode);
    if (res == -1) {
        //gagal membuat direktori
        fuse_log_write("FAILED", "MKDIR::rxpkisoviet-Create directory %s.", folderpath);
        return -errno;
    }
    //sukses membuat folder
    fuse_log_write("SUCCESS", "MKDIR::rxpkisoviet-Create directory %s.", folderpath);
    return 0;
}

//menghapus direktori
static int fuse_rmdir(const char *path)
{
    char folderpath[1000];
    sprintf(folderpath, "%s%s", directorypath, path);
    //mengecek kata restricted
    if (fuse_is_restricted(folderpath)) {
        fuse_log_write("FAILED", "RMDIR::rxpkisoviet-Remove directory %s.", folderpath);
        return -EPERM; // Operation not permitted
    }
    int res = rmdir(folderpath);
    if (res == -1) {
        //gagal melakukan remove folder
        fuse_log_write("FAILED", "RMDIR::rxpkisoviet-Remove directory %s.", folderpath);
        return -errno;
    }

    //     if (fuse_is_restricted(folderpath)) {
    //     fuse_log_write("FAILED", "RMDIR::rxpkisoviet-Remove directory %s.", folderpath);
    //     return -EPERM; // Operation not permitted
    // }
    //sukses menghapus folder
    fuse_log_write("SUCCESS", "RMDIR::rxpkisoviet-Remove directory %s.", folderpath);
    return 0;
}

//membuat file
static int fuse_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
    char folderpath[1000];
    sprintf(folderpath, "%s%s", directorypath, path);

    // Check if the path contains the word "restricted"
    if (fuse_is_restricted(folderpath)) {
        fuse_log_write("FAILED", "CREATE::rxpkisoviet-Create file %s.", folderpath);
        return -EPERM; // Operation not permitted
    }

    int res = open(folderpath, fi->flags, mode);
    if (res == -1) {
        //gagal membuat file
        fuse_log_write("FAILED", "CREATE::rxpkisoviet-Create file %s.", folderpath);
        return -errno;
    }
    fi->fh = res;
    // int res = open(folderpath, fi->flags, mode);
    // if (res == -1) {
    //sukses membuat file
    fuse_log_write("SUCCESS", "CREATE::rxpkisoviet-Create file %s.", folderpath);
    return 0;
}