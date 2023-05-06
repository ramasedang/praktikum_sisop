#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>


typedef struct {
    long msg_type;
    char msg_text[MSG_SIZE];
    char user_id[10];
} message;

void sigintHandler(int sig_num);
#define MSG_SIZE 256


int msgid;  
message msg;
int main() {
    key_t key = ftok("/home/ssudo/Matkul/sisop-modul3/song-playlist.json", 65);

    msgid = msgget(key, 0666 | IPC_CREAT);
    msg.msg_type = 1;
    sprintf(msg.user_id, "%d", getpid());

   
    signal(SIGINT, sigintHandler);

   
    strcpy(msg.msg_text, "START");
    msgsnd(msgid, &msg, sizeof(msg), 0);

    while (1) {
        printf("Masukkan perintah disini:");
        fgets(msg.msg_text, MSG_SIZE, stdin);
        msg.msg_text[strcspn(msg.msg_text, "\n")] = 0;

       
        if (strcmp(msg.msg_text, "EXIT") == 0) {
            msgsnd(msgid, &msg, sizeof(msg), 0);
            break;
        }

        msgsnd(msgid, &msg, sizeof(msg), 0);
    }

    return 0;
}

void sigintHandler(int sig_num) {
    
    strcpy(msg.msg_text, "EXIT");
    msgsnd(msgid, &msg, sizeof(msg), 0);

 
    exit(0);
}
