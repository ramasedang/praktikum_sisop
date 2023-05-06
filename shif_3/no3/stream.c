#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <jansson.h>
#include <string.h>
#include <stdint.h>
#include <semaphore.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
sem_t sem;
sem_t user_sem; // Declare the semaphore

#define MSG_SIZE 256
#define MAX_USER 2

int user_count = 0;

typedef struct
{
    long msg_type;
    char msg_text[MSG_SIZE];
    char user_id[10]; // Add this line
} message;

void rot13(char *str)
{
    for (int i = 0; str[i]; i++)
    {
        char c = str[i];
        if ('a' <= c && c <= 'z')
        {
            str[i] = (c - 'a' + 13) % 26 + 'a';
        }
        else if ('A' <= c && c <= 'Z')
        {
            str[i] = (c - 'A' + 13) % 26 + 'A';
        }
    }
}

void base64_decode(const char *str, char *decoded_str)
{
    BIO *bio, *b64;
    size_t decodeLen = strlen(str), len = 0;

    bio = BIO_new_mem_buf(str, -1);
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    len = BIO_read(bio, decoded_str, decodeLen);
    decoded_str[len] = '\0';

    BIO_free_all(bio);
}

void hex_decode(const char *str, char *decoded_str)
{
    size_t len = strlen(str);
    for (size_t i = 0; i < len; i += 2)
    {
        unsigned int byte;
        sscanf(str + i, "%2x", &byte);
        decoded_str[i / 2] = byte;
    }
    decoded_str[len / 2] = '\0';
}

void sort_playlist()
{
    system("sort -o playlist.txt playlist.txt");
}

void decrypt_playlist()
{
    json_error_t error;
    json_t *root = json_load_file("/home/ssudo/Matkul/sisop-modul3/song-playlist.json", 0, &error);
    FILE *output = fopen("playlist.txt", "w");

    size_t index;
    json_t *value;

    json_array_foreach(root, index, value)
    {
        const char *method = json_string_value(json_object_get(value, "method"));
        const char *song = json_string_value(json_object_get(value, "song"));
        char decrypted_song[256];

        if (strcmp(method, "rot13") == 0)
        {
            strcpy(decrypted_song, song);
            rot13(decrypted_song);
        }
        else if (strcmp(method, "base64") == 0)
        {
            base64_decode(song, decrypted_song);
        }
        else if (strcmp(method, "hex") == 0)
        {
            hex_decode(song, decrypted_song);
        }
        else
        {
            continue;
        }
        fprintf(output, "%s\n", decrypted_song);
    }
    fclose(output);
    json_decref(root);
}

void handle_decrypt_command()
{
    decrypt_playlist();
    printf("Decrypted playlist saved to 'playlist.txt'\n");
}

void search_playlist(const char *query)
{
    FILE *file = fopen("playlist.txt", "r");
    if (!file)
    {
        printf("Error opening playlist file\n");
        return;
    }

    char line[256];
    int count = 0;
    while (fgets(line, sizeof(line), file))
    {
        // Remove newline character from the line read
        line[strcspn(line, "\n")] = '\0';

        // Case-insensitive search
        char *query_lower = strdup(query);
        char *line_lower = strdup(line);
        for (int i = 0; query_lower[i]; i++)
            query_lower[i] = tolower(query_lower[i]);
        for (int i = 0; line_lower[i]; i++)
            line_lower[i] = tolower(line_lower[i]);

        if (strstr(line_lower, query_lower))
        {
            count++;
            printf("%d. %s\n", count, line);
        }

        free(query_lower);
        free(line_lower);
    }

    if (count == 0)
    {
        printf("THERE IS NO SONG CONTAINING \"%s\"\n", query);
    }
    else
    {
        printf("THERE ARE \"%d\" SONGS CONTAINING \"%s\"\n", count, query);
    }

    fclose(file);
}

void add_song(const char *user_id, const char *song)
{
    FILE *file = fopen("/home/ssudo/Matkul/sisop-modul3/playlist.txt", "r+");
    if (!file)
    {
        printf("Error opening playlist file\n");
        return;
    }

    char line[256];
    int song_exists = 0;
    while (fgets(line, sizeof(line), file))
    {
        // Remove newline character from the line read
        line[strcspn(line, "\n")] = '\0';

        if (strcmp(line, song) == 0)
        {
            song_exists = 1;
            break;
        }
    }

    if (song_exists)
    {
        printf("SONG ALREADY ON PLAYLIST\n");
    }
    else
    {
        fseek(file, 0, SEEK_END);
        fprintf(file, "%s\n", song);
        printf("USER %s ADD %s\n", user_id, song); // Change this line
    }

    fclose(file);
}

void handle_list_command()
{
    FILE *file = fopen("playlist.txt", "r");
    if (!file)
    {
        printf("Error opening playlist file\n");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        // Remove newline character from the line read
        line[strcspn(line, "\n")] = '\0';

        printf("%s\n", line);
    }

    fclose(file);
}

void handle_add_command(const char *user_id, const char *command)
{
    // Change this function to accept user_id
    char song[256];
    strcpy(song, command);
    add_song(user_id, song);
}

int can_access_system()
{
    // if (user_count < MAX_USER)
    // {
    //     user_count++;
    //     return 1;
    // }
    // else
    // {
    //     return 0;
    // }
    return 1;
}

void release_system_access()
{
    // user_count--;
}

int main()
{

    int BISMILLAH_MAX = 0;
    sem_init(&sem, 0, 2);      // Initialize the semaphore for the ADD command
    sem_init(&user_sem, 0, 1); // Initialize the semaphore for user access

    key_t key = ftok("song-playlist.json", 65);

    int msgid = msgget(key, 0666 | IPC_CREAT);
    message msg;

    while (1)
    {
        msgrcv(msgid, &msg, sizeof(msg), 1, 0);

        sem_wait(&user_sem); // Lock user semaphore before checking user access
        if(strncmp(msg.msg_text, "EXIT", 4) == 0){
                sem_post(&user_sem); // Unlock user semaphore if the system cannot be accessed
                BISMILLAH_MAX--;
        }
        if (BISMILLAH_MAX < 3)
        {
            if (can_access_system()) // Check if the system can be accessed by the user
            {
                sem_post(&user_sem); // Unlock user semaphore after checking

                if (strncmp(msg.msg_text, "DECRYPT", 7) == 0)
                {
                    handle_decrypt_command();
                    sort_playlist(); // Add this line
                }
                else if (strncmp(msg.msg_text, "LIST", 4) == 0)
                {
                    handle_list_command();
                }
                else if (strncmp(msg.msg_text, "PLAY", 4) == 0)
                {
                    char query[256];
                    sscanf(msg.msg_text, "PLAY %[^\n]", query);
                    search_playlist(query);
                }
                else if (strncmp(msg.msg_text, "ADD", 3) == 0)
                {
                    char song[256];
                    sscanf(msg.msg_text, "ADD %[^\n]", song);

                    sem_wait(&sem);                        // Lock semaphore before adding a song
                    handle_add_command(msg.user_id, song); // Pass the user_id when adding a song
                    sem_post(&sem);                        // Unlock semaphore after adding a song
                }
                // add handle msg "start"
                else if (strncmp(msg.msg_text, "START", 5) == 0)
                {
                    BISMILLAH_MAX++;
                }
                else
                {
                    printf("INVALID COMMAND\n");
                    printf("BISMILLAH MAX : %d\n", BISMILLAH_MAX);
                }

                sem_wait(&user_sem);     // Lock user semaphore before releasing system access
                release_system_access(); // Decrease the user count after the user has finished using the system
                sem_post(&user_sem);     // Unlock user semaphore after releasing system access
            }
            
        }
        else
        {
            sem_post(&user_sem); // Unlock user semaphore if the system cannot be accessed
            printf("STREAM SYSTEM OVERLOAD\n");
            // print bismillah max
            printf("BISMILLAH MAX : %d\n", BISMILLAH_MAX);
        }
    }

    msgctl(msgid, IPC_RMID, NULL);

    sem_destroy(&sem);      // Destroy the semaphore for the ADD command
    sem_destroy(&user_sem); // Destroy the semaphore for user access

    return 0;
}
