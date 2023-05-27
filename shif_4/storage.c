#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define Max_Line 1024

void downloadDataset();
void unzipFile(const char *zipfile);
void processFile();

int main()
{
    downloadDataset();
    unzipFile("fifa-player-stats-database.zip");
    processFile();

    return 0;
}

void downloadDataset()
{
    system("kaggle datasets download -d bryanb/fifa-player-stats-database");
}

void unzipFile(const char *zipfile)
{
    char command[50];
    snprintf(command, sizeof(command), "unzip %s", zipfile);
    system(command);
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
        char *id, *name, *age, *photo_url, *potential, *club;

        if (line[strlen(line) - 1] == '\n')
            line[strlen(line) - 1] = '\0'; // Remove newline character

        token = strtok(line, ",");
        id = token;
        token = strtok(NULL, ",");
        name = token;
        token = strtok(NULL, ",");
        age = token;
        token = strtok(NULL, ",");
        //aaa
        //a
        //
        photo_url = token;
        for (int i = 0; i < 4; i++)
            token = strtok(NULL, ",");
        potential = token;
        token = strtok(NULL, ",");
        club = token;

        if (atoi(age) < 25 && atoi(potential) > 85 && strcmp(club, "Manchester City") != 0)
        {
            printf("LANJUT\n");
            printf("iniNama: %s\niniClub: %s\niniAge: %s\niniPotential: %s\niniPhoto URL: %s\n\n", name, club, age, potential, photo_url);
        }
    }

    fclose(file);
}
