#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <archive.h>
#include <archive_entry.h>

#define Max_Line 1024

void downloadDataset();
void extractZipFile(const char *zipfile);
void processFile();

int main()
{
    downloadDataset();
    extractZipFile("fifa-player-stats-database.zip");
    processFile();

    return 0;
}

void downloadDataset()
{
    char *args[] = {"/usr/local/bin/kaggle", "datasets", "download", "-d", "bryanb/fifa-player-stats-database", NULL};
    pid_t pid = fork();
    if (pid == 0)
    {
        execv(args[0], args);
        _exit(EXIT_FAILURE);
    }
    else if (pid < 0)
    {
        perror("fork failed");
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
    }
}


void extractZipFile(const char *zipfile)
{
    struct archive *a;
    struct archive *ext;
    struct archive_entry *entry;
    int flags;
    int r;

    flags = ARCHIVE_EXTRACT_TIME;

    a = archive_read_new();
    archive_read_support_format_zip(a);
    ext = archive_write_disk_new();
    archive_write_disk_set_options(ext, flags);
    archive_write_disk_set_standard_lookup(ext);
    if ((r = archive_read_open_filename(a, zipfile, 10240)))
        exit(1);
    for (;;)
    {
        r = archive_read_next_header(a, &entry);
        if (r == ARCHIVE_EOF)
            break;
        if (r != ARCHIVE_OK)
            exit(1);
        r = archive_write_header(ext, entry);
        if (r != ARCHIVE_OK)
            printf("error: %s\n", archive_error_string(ext));
        else
        {
            copy_data(a, ext);
            r = archive_write_finish_entry(ext);
            if (r != ARCHIVE_OK)
                exit(1);
        }
    }
    archive_read_close(a);
    archive_read_free(a);
    archive_write_close(ext);
    archive_write_free(ext);
}

void copy_data(struct archive *ar, struct archive *aw)
{
    int r;
    const void *buff;
    size_t size;
    int64_t offset;

    for (;;)
    {
        r = archive_read_data_block(ar, &buff, &size, &offset);
        if (r == ARCHIVE_EOF)
            return;
        if (r != ARCHIVE_OK)
            exit(1);
        r = archive_write_data_block(aw, buff, size, offset);
        if (r != ARCHIVE_OK)
        {
            printf("error: %s\n", archive_error_string(aw));
            exit(1);
        }
    }
}

void processFile()
{
    FILE *file = fopen("FIFA23_official_data.csv", "r");

    if (!file)
    {
        printf("Could not open file\n");
        return;
    }

    char line[Max_Line];
    fgets(line, sizeof(line), file); // Skip header line

    while (fgets(line, sizeof(line), file))
    {
        char *token;
        char *id, *name, *age, *photo_url, *nationality, *potential, *club;

        if (line[strlen(line) - 1] == '\n')
            line[strlen(line) - 1] = '\0'; // Remove newline character

        token = strtok(line, ",");
        id = token;
        token = strtok(NULL, ",");
        name = token;
        token = strtok(NULL, ",");
        age = token;
        token = strtok(NULL, ",");
        photo_url = token;
        token = strtok(NULL, ",");
        nationality = token;
        for (int i = 0; i < 3; i++)
            token = strtok(NULL, ",");
        potential = token;
        token = strtok(NULL, ",");
        club = token;

        if (atoi(age) < 25 && atoi(potential) > 85 && strcmp(club, "Manchester City") != 0)
        {
            printf("LANJUT\n");
            printf("---------Nama: %s\n---------Club: %s\n-Age: %s\nPotential: %s\nPhoto URL: %s\nNationality: %s\n\n", 
                   name, club, age, potential, photo_url, nationality);
        }
    }

    fclose(file);
}
