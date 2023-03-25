#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <fnmatch.h>

#define MAX_PLAYERS 100
#define team "ManUtd"
#define MAX_FILENAME 256

// Declare the struct for player
typedef struct
{
    char name[50];
    char position[20];
    char image_file[100];
} Player;

void downloadZip();
void extractZip();
void filterPlayers();
void categorizePlayers();
void buatTim(int bek, int gelandang, int striker);

void downloadZip()
{
    char *args[] = {"curl", "-L", "-o", "players.zip", "https://drive.google.com/u/0/uc?id=1zEAneJ1-0sOgt13R1gL4i1ONWfKAtwBF&export=download", NULL};
    pid_t pid = fork();

    if (pid == 0)
    { // child process
        execvp(args[0], args);
        perror("execvp");
        exit(EXIT_FAILURE);
    }
    else
    { // parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
        {
            printf("Error downloading players\n");
            exit(EXIT_FAILURE);
        }
    }
}

void extractZip()
{
    char *args[] = {"unzip", "players.zip", NULL};
    pid_t pid = fork();

    if (pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    { // child process
        execvp(args[0], args);
        perror("execvp");
        exit(EXIT_FAILURE);
    }
    else
    { // parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
        {
            printf("Error extracting players\n");
            exit(EXIT_FAILURE);
        }
    }
}

void filterPlayers()
{
    DIR *dir = opendir("players");
    struct dirent *entry;
    char fileName[MAX_FILENAME];

    if (dir == NULL)
    {
        printf("Error opening directory\n");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG)
        { // if entry is a regular file
            if (fnmatch("*_ManUtd_*_*.png", entry->d_name, 0) != 0)
            {
                sprintf(fileName, "%s/%s", "players", entry->d_name);
                if (unlink(fileName) != 0)
                {
                    printf("Error deleting file: %s\n", fileName);
                }
            }
        }
    }

    closedir(dir);
}

void categorizePlayers()
{
    DIR *dir = opendir("./players");
    struct dirent *entry;
    char *image_file;
    char *positions[] = {"Kiper", "Bek", "Gelandang", "Penyerang"};

    if (dir == NULL)
    {
        printf("Error opening directoryn");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG)
        { // if entry is a regular file
            image_file = entry->d_name;
            char *ext = strrchr(image_file, '.');

            if (ext != NULL && (strcmp(ext, ".jpeg") == 0 || strcmp(ext, ".jpg") == 0 || strcmp(ext, ".png") == 0))
            {
                int i;
                for (i = 0; i < sizeof(positions) / sizeof(positions[0]); i++)
                {
                    if (strstr(image_file, positions[i]) != NULL)
                    {
                        char new_path[MAX_FILENAME];
                        sprintf(new_path, "./%s/%s", positions[i], image_file);

                        char old_path[MAX_FILENAME];
                        sprintf(old_path, "./players/%s", image_file);

                        FILE *old_file = fopen(old_path, "rb");
                        if (old_file == NULL)
                        {
                            perror("Unable to read file");
                            break;
                        }

                        FILE *new_file = fopen(new_path, "wb");
                        if (new_file == NULL)
                        {
                            perror("Unable to create file");
                            fclose(old_file);
                            break;
                        }

                        char buffer[1024];
                        size_t bytes_read;
                        while ((bytes_read = fread(buffer, 1, sizeof(buffer), old_file)) > 0)
                        {
                            fwrite(buffer, 1, bytes_read, new_file);
                        }

                        fclose(old_file);
                        fclose(new_file);
                        break;
                    }
                }
            }
        }
    }

    closedir(dir);
}

void buatTim(int bek, int gelandang, int striker)
{
    // Create a file with the appropriate file name
    char fileName[50];
    sprintf(fileName, "Formasi_%d_%d_%d.txt", bek, gelandang, striker);
    FILE *fp = fopen(fileName, "w");

    // Write the formation to the file
    fprintf(fp, "Formasi %d-%d-%dn", bek, gelandang, striker);

    // Get the list of players
    char *beks[8] = {"Bissaka", "Dalot", "Lindelof", "Maguire", "Malacia", "Martinez", "Shaw", "Varane"};
    char *gelandangs[7] = {"Casemiro", "Eriksen", "Fernandes", "Fred", "McTominay", "Sabitzer", "VanDeBeek"};
    char *strikers[5] = {"Antony", "Martial", "Rashford", "Sancho", "Weghorst"};

    // Sort the players by name (ascending order)
    int numBeks = sizeof(beks) / sizeof(beks[0]);
    int numGelandangs = sizeof(gelandangs) / sizeof(gelandangs[0]);
    int numStrikers = sizeof(strikers) / sizeof(strikers[0]);
    for (int i = 0; i < numBeks - 1; i++)
    {
        for (int j = i + 1; j < numBeks; j++)
        {
            if (beks[j] < beks[i])
            {
                char *temp = beks[i];
                beks[i] = beks[j];
                beks[j] = temp;
            }
        }
    }
    for (int i = 0; i < numGelandangs - 1; i++)
    {
        for (int j = i + 1; j < numGelandangs; j++)
        {
            if (gelandangs[j] < gelandangs[i])
            {
                char *temp = gelandangs[i];
                gelandangs[i] = gelandangs[j];
                gelandangs[j] = temp;
            }
        }
    }
    for (int i = 0; i < numStrikers - 1; i++)
    {
        for (int j = i + 1; j < numStrikers; j++)
        {
            if (strikers[j] < strikers[i])
            {
                char *temp = strikers[i];
                strikers[i] = strikers[j];
                strikers[j] = temp;
            }
        }
    }

    // Write the players with the highest indices to the file
    fprintf(fp, "nBEK:n");
    for (int i = numBeks - 1; i >= (numBeks - bek) && i >= 0; i--)
    {
        fprintf(fp, "%sn", beks[i]);
    }
    fprintf(fp, "nGELANDANG:n");
    for (int i = numGelandangs - 1; i >= (numGelandangs - gelandang) && i >= 0; i--)
    {
        fprintf(fp, "%sn", gelandangs[i]);
    }
    fprintf(fp, "nPENYERANG:n");
    for (int i = numStrikers - 1; i >= (numStrikers - striker) && i >= 0; i--)
    {
        fprintf(fp, "%sn", strikers[i]);
    }

    fclose(fp);
}

int main()
{
    downloadZip();
    extractZip();
    filterPlayers();
    categorizePlayers();
    buatTim(4, 4, 2); 
    return 0;
}
