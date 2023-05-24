#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

void akuUnzip(char *akuZipfile);

#define Max_Line 1024

int main()
{
    system("kaggle datasets download -d bryanb/fifa-player-stats-database");
    char akuZipfile[] = "fifa-player-stats-database.zip";

    akuUnzip(akuZipfile);

    FILE *file = fopen("FIFA23_official_data.csv", "r");

    if (!file)
    {
        printf("Could not open file\n");
        return 1;
    }

    char line[Max_Line];

    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file))
    {
        char *token = strtok(line, ",");
        char *id = token;
        token = strtok(NULL, ",");
        char *name = token;
        token = strtok(NULL, ",");
        char *age = token;
        token = strtok(NULL, ",");
        char *photo_url = token;
        for (int i = 0; i < 4; i++)
            token = strtok(NULL, ",");
        char *potential = token;
        token = strtok(NULL, ",");
        char *club = token;

        if (atoi(age) < 25 && atoi(potential) > 85 && strcmp(club, "Manchester City") != 0)
        {
            printf("Name: %s\nClub: %s\nAge: %s\nPotential: %s\nPhoto URL: %s\n\n", name, club, age, potential, photo_url);
        }
    }

    fclose(file);
    return 0;
}

void akuUnzip(char *akuZipfile)
{
    char command[50];

    strcpy(command, "akuUnzip ");
    strcat(command, akuZipfile);

    system(command);
}
