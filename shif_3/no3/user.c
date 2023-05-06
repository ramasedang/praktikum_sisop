#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#define MSG_SIZE 256

typedef struct {
    long msg_type;
    char msg_text[MSG_SIZE];
    char user_id[10];
} message;

int msgid;  // Make it global so it can be accessed from the signal handler
message msg;

void sigintHandler(int sig_num) {
    // Send an "EXIT" message to the stream
    strcpy(msg.msg_text, "EXIT");
    msgsnd(msgid, &msg, sizeof(msg), 0);

    // Terminate the program
    exit(0);
}

int main() {
    key_t key = ftok("/home/ssudo/Matkul/sisop-modul3/song-playlist.json", 65);

    msgid = msgget(key, 0666 | IPC_CREAT);
    msg.msg_type = 1;
    sprintf(msg.user_id, "%d", getpid());

    // Register the signal handler for SIGINT (Ctrl+C)
    signal(SIGINT, sigintHandler);

    //send msg start to stream
    strcpy(msg.msg_text, "START");
    msgsnd(msgid, &msg, sizeof(msg), 0);

    while (1) {
        printf("Enter command: ");
        fgets(msg.msg_text, MSG_SIZE, stdin);
        msg.msg_text[strcspn(msg.msg_text, "\n")] = 0;

        // If the command is "EXIT", we should also stop this program
        if (strcmp(msg.msg_text, "EXIT") == 0) {
            msgsnd(msgid, &msg, sizeof(msg), 0);
            break;
        }

        msgsnd(msgid, &msg, sizeof(msg), 0);
    }

    return 0;
}
