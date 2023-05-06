#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <fnmatch.h>

int main()
{
    pid_t pid_download, pid_extract;
    int status;

    // Download hehe.zip
    char *args_download[] = {"curl", "-L", "-o", "hehe.zip", "https://drive.google.com/u/0/uc?id=1rsR6jTBss1dJh2jdEKKeUyTtTRi_0fqp&export=download", NULL};
    
    pid_download = fork();

    if (pid_download == -1)
    {
        printf("Error saat fork : \n");
        exit(EXIT_FAILURE);
    }
    else if (pid_download == 0)
    { 
        execvp(args_download[0], args_download);
        perror("execvp");
        exit(EXIT_FAILURE);
    }

    waitpid(pid_download, &status, 0);

    // Extract hehe.zip
    char *args_extract[] = {"unzip", "hehe.zip", NULL};

    pid_extract = fork();

    if (pid_extract == -1)
    {
        printf("Error saat fork : \n");
        exit(EXIT_FAILURE);
    }
    else if (pid_extract == 0)
    { 
        execvp(args_extract[0], args_extract);
        perror("execvp");
        exit(EXIT_FAILURE);
    }

    waitpid(pid_extract, &status, 0);

    return 0;
}
