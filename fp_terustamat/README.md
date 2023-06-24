# sisop-praktikum-fp-mhfd-it14

## Laporan Resmi FP Praktikum Sistem Operasi 2023
---
### Kelompok IT14:
Aloysius Bataona Manullang - 5027211008  
Athaya Reyhan Nugroho - 5027211067    
Moh. Sulthan Arief Rahmatullah - 5027211045
---

## Analisis Soal
Berdasarkan permintaan soal, dibutuhkan sebuah sistem database sederhana yang dapat menjalankan program server sebagai daemon dan diakses melalui program client dengan memasukkan perintah. Program client dan utama saling berinteraksi lewat socket dan harus bisa diakses dari mana saja. Di dalam sistem database ini harus terdapat beberapa fitur penting seperti autentikasi, otorisasi, data definition language (DDL), data manipulation language (DML), logging, keandalan, penanganan kesalahan (error handling), dan containerization. Selain itu, sistem juga harus dilengkapi dengan beberapa tambahan untuk memastikan kelancaran dan keamanan operasionalnya.

## Cara pengerjaan soal : 

Untuk cara cara pengerjaannya akan dibahas dari client.c , jadi pada client.c kami akan membuat fungsi untuk log dan juka mengecekan autentikasi dan autorisasi.


berikut untuk fungsi log nya
```c
void writeLog(const char *username, const char *command)
{
	time_t now = time(NULL);
	struct tm *timeinfo = localtime(&now);
	char timestamp[20];
	strftime(timestamp, sizeof(timestamp), "%d:%m:%Y:%H:%M:%S", timeinfo);
	FILE *file = fopen("log.txt", "a+");
 
	if (file == NULL)
	{
    	printf("Failed to open log.txt file.\n");
    	return;
	}
	fprintf(file, "%s:%s:%s\n", timestamp, username, command);
	fclose(file);
}
```
Fungsi writeLog dalam kode ini berfungsi untuk menulis log dari aktivitas pengguna ke dalam file teks "log.txt". Fungsi ini menerima dua parameter input: username dan command. Pertama, fungsi ini mendapatkan waktu lokal dan merubahnya menjadi format tanggal dan waktu tertentu. Kemudian, fungsi ini mencoba untuk membuka file "log.txt" dalam mode penambahan ('a+') sehingga informasi baru dapat ditambahkan ke akhir file tanpa menghapus informasi yang ada. Jika file berhasil dibuka, fungsi ini menulis timestamp, username, dan command ke dalam file. Jika file gagal dibuka, fungsi ini mencetak pesan kesalahan. Akhirnya, fungsi ini menutup file.
Selanjutnya adalah fungsi register user yang hanya bisa di akses user root
Berikut adalah fungsinya
```c
int registerUser(const char *username, const char *password)
{
 
	if (geteuid() != 0)
	{
    	printf("Registration can only be done by the root user.\n");
    	return 0;
	}
 
	FILE *file = fopen("users.txt", "a+");
 
	if (file == NULL)
	{
    	printf("Failed to open users.txt file.\n");
    	return 0;
	}
 
	struct User user;
 
	while (fscanf(file, "%[^;];%[^\n]\n", user.username, user.password) != EOF)
	{
    	if (strcmp(username, user.username) == 0)
    	{
        	printf("Username already exists.\n");
        	fclose(file);
        	return 0;
    	}
	}
 
	fprintf(file, "%s;%s\n", username, password);
	fclose(file);
 
	if (chmod("users.txt", 0644) != 0)
	{
    	printf("Failed to change file permissions.\n");
    	return 1;
	}
	printf("User registered successfully.\n");
	return 1;
}
```
 
Fungsi registerUser digunakan untuk melakukan registrasi pengguna baru dengan menyimpan username dan password ke dalam file "users.txt". Fungsi ini hanya bisa dijalankan oleh root user, jika bukan, akan mencetak pesan kesalahan. Jika file "users.txt" bisa dibuka, fungsi ini kemudian membaca setiap baris dari file untuk memeriksa apakah username sudah ada. Jika username sudah ada, fungsi ini mencetak pesan kesalahan dan berhenti. Jika username tidak ada, fungsi ini menulis username dan password ke dalam file dan kemudian menutup file. Fungsi ini juga mengubah perizinan file ke 0644. Jika perubahan perizinan file berhasil, fungsi ini mencetak pesan keberhasilan dan mengembalikan 1, sebaliknya, jika perubahan perizinan file gagal, fungsi ini mencetak pesan kesalahan dan juga mengembalikan 1.

 Selanjutnya adalah fungsi login, jadi jika kita menggunakan root kita tidak perlu password, tapi jika kita menggunakan user biasa kita harus menggunakan password dan username
```c
 int loginUser(const char *username, const char *password)
{
	FILE *file = fopen("users.txt", "r");
	if (file == NULL)
	{
    	printf("Failed to open users.txt file.\n");
    	return 0;
	}
	rewind(file);
	struct User user;
 
	while (fscanf(file, "%[^;];%[^\n]\n", user.username, user.password) != EOF)
	{
    	if (strcmp(username, user.username) == 0)
    	{
        	if (strcmp(password, user.password) == 0)
        	{
            	fclose(file);
                strcpy(loggedInUsername, username);
            	return 1;
        	}
        	break;
    	}
	}
	fclose(file);
	return 0;
}
```
 Fungsi loginUser digunakan untuk melakukan autentikasi pengguna dengan memverifikasi username dan password dari file "users.txt". Pertama, fungsi ini mencoba membuka file "users.txt" dalam mode baca ('r'). Jika file tidak bisa dibuka, fungsi ini mencetak pesan kesalahan dan mengembalikan 0. Fungsi ini kemudian membaca setiap baris file, membandingkan username dan password input dengan data di file. Jika username ditemukan dan password cocok, fungsi ini menutup file, meng-copy username ke dalam variabel global loggedInUsername, dan mengembalikan 1 yang menandakan sukses. Jika username tidak ditemukan atau password tidak cocok, fungsi ini menutup file dan mengembalikan 0 yang menandakan gagal.


 Selanjutnya adalah untuk database.c, setalah client telah melakukan autentikasi dan autorisasi, selanjutnya ketika client meminta untuk melakukan Data Definition Language, maka program database.c yang berfungsi sebagai berikut :
```c
void createDatabase(const char *databaseName)
{
    char sanitized_name[100];
    size_t len = strlen(databaseName);
    if (len > 0 && databaseName[len-1] == '\n') {
        strncpy(sanitized_name, databaseName, len-1);
        sanitized_name[len-1] = '\0';
    } else {
        strcpy(sanitized_name, databaseName);
    }
    mkdir(sanitized_name, 0700);
}
```
Fungsi di atas adalah untuk membuat database. Semua user bisa membuat database, otomatis user tersebut memiliki permission untuk database tersebut.
```c
void createTable(const char *databaseName, const char *tableName, char *columns[], char *types[])
    {
        char filename[100];
        sprintf(filename, "%s/%s.json", databaseName, tableName);
        json_t *json_arr = json_array();
        json_t *column_info = json_object();
        for (int i = 0; columns[i] != NULL; i++)
        {
            json_object_set_new(column_info, columns[i], json_string(types[i]));
        }
        json_array_append_new(json_arr, column_info);
        json_dump_file(json_arr, filename, 0);
        json_decref(json_arr);
    }
```
Fungsi di atas adalah membuat tabel setelah client melakukan autorisasis "USE". Pada fungsi tersebut root dan user yang memiliki permission untuk suatu database untuk bisa membuat tabel untuk database tersebut, tentunya setelah mengakses database tersebut. Tipe data dari semua kolom adalah string atau integer. Jumlah kolom bebas.
```c
void dropTable(const char *databaseName, const char *tableName)
{
    char filename[100];
    sprintf(filename, "%s/%s.json", databaseName, tableName);
    remove(filename);
}

void dropColumn(const char *databaseName, const char *tableName, const char *columnName)
{
    char filename[100];
    sprintf(filename, "%s/%s.json", databaseName, tableName);

    json_error_t error;
    json_t *root = json_load_file(filename, 0, &error);

    if (!root)
    {
        printf("Error reading table: %s\n", error.text);
        return;
    }

    size_t index;
    json_t *value;

    json_array_foreach(root, index, value)
    {
        json_object_del(value, columnName);
    }

    json_dump_file(root, filename, 0);
    json_decref(root);
}
```
Fungsi di atas untuk melakukan DROP databse, tabel, dan column. Pada program melakukan DROP database, table (setelah mengakses database menggunakan "USE"), dan kolom. Jika sasaran drop ada maka di-drop, jika tidak ada maka biarkan.

  Setalah database.c sudah memiliki Data Definition Language, selanjutnya program diminta untuk melakukan Data Manipulation Language.  
```c
void insertIntoTable(const char *databaseName, const char *tableName, char *columns[], char *values[])
{
    char filename[100];
    sprintf(filename, "%s/%s.json", databaseName, tableName);
    json_error_t error;
    json_t *root = json_load_file(filename, 0, &error);

    if (!root)
    {
        printf("Error reading table: %s\n", error.text);
        return;
    }

    json_t *new_entry = json_object();
    for (int i = 0; columns[i] != NULL && values[i] != NULL; i++)
    {
        json_object_set_new(new_entry, columns[i], json_string(values[i]));
    }
    json_array_append_new(root, new_entry);
    json_dump_file(root, filename, 0);
    json_decref(root);
}

void updateTable(const char *databaseName, const char *tableName, const char *columnName, const char *value)
{
    char filename[100];
    sprintf(filename, "%s/%s.json", databaseName, tableName);

    json_error_t error;
    json_t *root = json_load_file(filename, 0, &error);

    if (!root)
    {
        printf("Error reading table: %s\n", error.text);
        return;
    }

    size_t index;
    json_t *entry;

    // Start from index 1 to skip the header
    for (index = 1; index < json_array_size(root); index++)
    {
        entry = json_array_get(root, index);

        json_t *field = json_object_get(entry, columnName);

        if (field)
        {
            json_object_set(entry, columnName, json_string(value));
        }
    }

    json_dump_file(root, filename, 0);
    json_decref(root);
}

void deleteFromTable(const char *databaseName, const char *tableName)
{
    char filename[100];
    sprintf(filename, "%s/%s.json", databaseName, tableName);
    FILE *file = fopen(filename, "w");
    fprintf(file, "[]");
    fclose(file);
}

void selectFromTable(const char *databaseName, const char *tableName, char *keys[])
{
    char filename[100];
    sprintf(filename, "%s/%s.json", databaseName, tableName);

    json_error_t error;
    json_t *table = json_load_file(filename, 0, &error);

    if (!table)
    {
        printf("Error reading table: %s\n", error.text);
        return;
    }

    size_t index;
    json_t *value;

    json_array_foreach(table, index, value)
    {
        for (int i = 0; keys[i] != NULL; i++)
        {
            const char *key = keys[i];
            json_t *field = json_object_get(value, key);

            if (json_is_string(field))
            {
                printf("%s: %s\n", key, json_string_value(field));
            }
        }
        printf("\n");
    }

    json_decref(table);
}


void extractColumnNames(const char *databaseName, const char *tableName, char *columns[])
{
    char filename[100];
    sprintf(filename, "%s/%s.json", databaseName, tableName);
    json_error_t error;

    // Open the JSON file and read its contents
    json_t *root = json_load_file(filename, 0, &error);
    if (!root)
    {
        printf("Error reading table: %s\n", error.text);
        return;
    }

    // Get the first object in the array (which contains column definitions)
    json_t *column_info = json_array_get(root, 0);
    if (!json_is_object(column_info))
    {
        printf("Error reading column info\n");
        json_decref(root);
        return;
    }

    // Copy column names to the output array
    const char *key;
    json_t *value;
    int i = 0;
    json_object_foreach(column_info, key, value)
    {
        columns[i] = strdup(key);
        i++;
    }
    columns[i] = NULL; // Mark the end of the array with NULL

    // Clean up
    json_decref(root);
}
```
Fungsi ```insertIntoTable``` digunakan untuk memasukkan data baru ke dalam tabel. Fungsi ini menerima tiga argumen: nama database, nama tabel, dan array kolom beserta array nilai yang akan dimasukkan ke tabel. Data baru akan ditulis dalam format JSON dan disimpan dalam file dengan ekstensi .json yang berada di direktori database. Fungsi ```updateTable`` digunakan untuk mengubah nilai dari sebuah kolom pada satu atau lebih baris di dalam tabel. Fungsi ini menerima empat argumen: nama database, nama tabel, nama kolom yang akan diubah, dan nilai baru yang akan diberikan pada kolom tersebut. Data yang telah diubah akan ditulis kembali dalam format JSON dan disimpan dalam file dengan ekstensi .json yang berada di direktori database. Fungsi ```deleteFromTable``` digunakan untuk menghapus seluruh isi dari sebuah tabel. Fungsi ini menerima dua argumen: nama database dan nama tabel yang akan dihapus. Data akan dihapus dari file dengan ekstensi .json yang berada di direktori database. Fungsi ```selectFromTable``` digunakan untuk membaca nilai-nilai dari sebuah tabel dan menampilkan hasilnya pada layar. Fungsi ini menerima tiga argumen: nama database, nama tabel, dan array kolom yang akan ditampilkan nilainya. Data akan dibaca dari file dengan ekstensi .json yang berada di direktori database.  Fungsi ```extractColumnNames``` digunakan untuk membaca nama-nama kolom dari sebuah tabel. Fungsi ini menerima dua argumen: nama database dan nama tabel yang akan dibaca.

	Selanjutnya ada membuat file dump.c , program ini akan melalui proses autentikasi terlebih dahulu. Program akan membuka database dan membaca setiap isi yang berada pada database. Berikut fungsi programnya :
```c

void dump_database(char *dir_name, FILE *backup_file)
{
    DIR *d;
    struct dirent *dir;
    d = opendir(dir_name);
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if (dir->d_type == DT_REG)
            { // Regular file
                char file_name[100];
                sprintf(file_name, "%s/%s", dir_name, dir->d_name);
                json_error_t error;
                json_t *root = json_load_file(file_name, 0, &error);

                if (!root)
                {
                    fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
                    return;
                }

                if (!json_is_array(root))
                {
                    fprintf(stderr, "error: root is not an array\n");
                    json_decref(root);
                    return;
                }

                size_t i, size;
                json_t *data, *value;
                const char *key;

                char table_name[100];
                strncpy(table_name, dir->d_name, strlen(dir->d_name) - 5); // Subtract 5 to remove ".json"
                table_name[strlen(dir->d_name) - 5] = '\0';

                fprintf(backup_file, "DROP TABLE %s;\n", table_name);

                // For CREATE TABLE statement
                fprintf(backup_file, "CREATE TABLE %s (", table_name);
                json_t *first_row = json_array_get(root, 0);
                json_object_foreach(first_row, key, value)
                {
                    fprintf(backup_file, "%s string, ", key); 
                }
                fseek(backup_file, -2, SEEK_CUR); 
                fprintf(backup_file, ");\n");

                json_array_foreach(root, i, data)
                {
                    fprintf(backup_file, "INSERT INTO %s (", table_name);
                    size_t count = 0, total = json_object_size(data);
                    json_object_foreach(data, key, value)
                    {
                        count++;
                        // Dump the data 
                        fprintf(backup_file, "%s%s", json_string_value(value), count < total ? ", " : "");
                    }
                    fprintf(backup_file, ");\n");
                }

                json_decref(root);
            }
        }
        closedir(d);
    }
}
jelaskan kode di atas
```
Pertama-tama, fungsi membuka database dengan menggunakan opendir() dan membaca setiap isi di dalamnya dengan menggunakan readdir(). Jika file yang sedang dibaca merupakan file regular, maka fungsi akan memuat isi file tersebut sebagai objek JSON menggunakan json_load_file() dari pustaka cJSON. Setelah itu, fungsi akan melakukan pengecekan tabel apakah objek JSON merupakan array atau bukan menggunakan json_is_array(). Jika bukan array, maka fungsi akan mencetak pesan error dan mengembalikan nilai kosong. Jika objek JSON merupakan array, maka fungsi akan membuat tabel dengan nama sesuai dengan nama file (di sini dilakukan substrings pada nama file untuk menghilangkan ekstensi .json), melakukan ```DROP TABLE``` lama (jika ada) serta mengekspor semua data dari file menuju kedalam sql file backup. Fungsi akan menuliskan ```CREATE TABLE``` statement ke dalam file backup dengan menggunakan fprintf() untuk menampilkan atribut-atribut kolom. Kemudian, saat melakukan iterasi pada array JSON, fungsi akan melakukan loop untuk setiap objek JSON dan menuliskan ```INSERT INTO``` statement ke dalam file backup. Semua query  yang ditulis ke file backup. Terakhir, fungsi akan menutup direktori dengan menggunakan closedir().

## Source code
client.c
```c
#include <arpa/inet.h>
#include <getopt.h>
#include <libgen.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

char loggedInUsername[1000];

struct User
{
    char username[100];
    char password[100];
};

int registerUser(const char *username, const char *password)
{
    if (geteuid() != 0)
    {
        printf("Registration can only be done by the root user.\n");
        return 0;
    }

    FILE *file = fopen("users.txt", "a+");

    if (file == NULL)
    {
        printf("Failed to open users.txt file.\n");
        return 0;
    }

    struct User user;

    while (fscanf(file, "%[^;];%[^\n]\n", user.username, user.password) != EOF)
    {
        if (strcmp(username, user.username) == 0)
        {
            printf("Username already exists.\n");
            fclose(file);
            return 0;
        }
    }

    fprintf(file, "%s;%s\n", username, password);
    fclose(file);

    if (chmod("users.txt", 0644) != 0)
    {
        printf("Failed to change file permissions.\n");
        return 1;
    }
    printf("User registered successfully.\n");
    return 1;
}

int loginUser(const char *username, const char *password)
{
    FILE *file = fopen("users.txt", "r");
    if (file == NULL)
    {
        printf("Failed to open users.txt file.\n");
        return 0;
    }
    rewind(file);
    struct User user;

    while (fscanf(file, "%[^;];%[^\n]\n", user.username, user.password) != EOF)
    {
        if (strcmp(username, user.username) == 0)
        {
            if (strcmp(password, user.password) == 0)
            {
                fclose(file);
                strcpy(loggedInUsername, username);
                return 1;
            }
            break;
        }
    }
    fclose(file);
    return 0;
}

void grantDatabaseAccess(const char *database, const char *username)
{
    if (geteuid() != 0)
    {
        printf("Only the root user can grant database access.\n");
        return;
    }

    FILE *file = fopen("database_access.txt", "a+");

    if (file == NULL)
    {
        printf("Failed to open database_access.txt file.\n");
        return;
    }

    fprintf(file, "%s;%s\n", database, username);
    fclose(file);

    if (chmod("database_access.txt", 0644) != 0)
    {
        printf("Failed to change file permissions.\n");
        return;
    }
    printf("Database access granted successfully.\n");
}

int checkDatabaseAccess(const char *database, const char *username)
{
    FILE *file = fopen("database_access.txt", "r");

    if (file == NULL)
    {
        printf("Failed to open database_access.txt file.\n");
        return 0;
    }

    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        char dbName[100];
        char userName[100];

        sscanf(line, "%[^;];%s\n", dbName, userName);

        if (strcmp(database, dbName) == 0 && strcmp(username, userName) == 0)
        {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

void writeLog(const char *username, const char *command)
{
    time_t now = time(NULL);
    struct tm *timeinfo = localtime(&now);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%d:%m:%Y:%H:%M:%S", timeinfo);
    FILE *file = fopen("log.txt", "a+");

    if (file == NULL)
    {
        printf("Failed to open log.txt file.\n");
        return;
    }
    fprintf(file, "%s:%s:%s\n", timestamp, username, command);
    fclose(file);
}

int main(int argc, char *argv[])
{
    int client_socket;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE] = {0};
    bool isLoggedIn = false;
    char loggedInUsername[100];

    // Check if the program is running as root (sudo)
    if (geteuid() == 0)
    {
        // If the program is running as root, set isLoggedIn to true and
        // loggedInUsername to "sudo"
        isLoggedIn = true;
        strcpy(loggedInUsername, "sudo");
    }
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (connect(client_socket, (struct sockaddr *)&server_address,
                sizeof(server_address)) < 0)
    {
        perror("connection failed");
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        if (!isLoggedIn)
        {
            printf("Type a random character to continue or 'exit' to exit.\n");
            char option[10];
            fgets(option, sizeof(option), stdin);
            if (strcmp(option, "exit\n") == 0)
            {
                break;
            }
            else
            {
                char username[100];
                char password[100];
                int option;
                while ((option = getopt(argc, argv, "u:p:")) != -1)
                {
                    switch (option)
                    {
                    case 'u':
                        strcpy(username, optarg);
                        break;
                    case 'p':
                        strcpy(password, optarg);
                        break;
                    default:
                        printf("Invalid option.\n");
                        break;
                    }
                }
                if (strlen(username) > 0 && strlen(password) > 0)
                {
                    if (loginUser(username, password))
                    {
                        printf("Login successful.\n");
                        isLoggedIn = true;
                        strcpy(loggedInUsername, username);
                    }
                    else
                    {
                        printf("Invalid credentials. Please try again.\n");
                    }
                }
            }
        }
        else
        {
            while (isLoggedIn)
            {
                printf("LOGGED IN AS %s\n", loggedInUsername);
                char command[100];
                printf("Enter command: ");
                fgets(command, sizeof(command), stdin);
                if (command[strlen(command) - 1] == '\n')
                {
                    strtok(command, "\n");
                }
                if (strcmp(loggedInUsername, "sudo") == 0 && geteuid() == 0 &&
                    strncmp(command, "CREATE USER", 11) == 0)
                {
                    char username[100];
                    char password[100];

                    if (sscanf(command, "CREATE USER %s IDENTIFIED BY %s", username,
                               password) == 2)
                    {
                        registerUser(username, password);

                        printf("User %s registered successfully.\n", username);
                    }
                    else
                    {
                        printf(
                            "Invalid format. Usage: CREATE USER [username] IDENTIFIED BY "
                            "[password]\n");
                    }
                    continue;
                }
                if (strcmp(loggedInUsername, "sudo") == 0 && geteuid() == 0 &&
                    strncmp(command, "GRANT PERMISSION", 16) == 0)
                {
                    char database[100];
                    char username[100];

                    if (sscanf(command, "GRANT PERMISSION %s INTO %s", database,
                               username) == 2)
                    {
                        grantDatabaseAccess(database, username);

                        printf("Access to %s granted to user %s successfully.\n", database,
                               username);
                    }
                    else
                    {
                        printf(
                            "Invalid format. Usage: GRANT PERMISSION [database] INTO "
                            "[username]\n");
                    }
                    continue;
                }
                if (strncmp(command, "USE ", 4) == 0)
                {
                    char database[100];

                    if (sscanf(command, "USE %s", database) == 1)
                    {
                        if (strcmp(loggedInUsername, "sudo") == 0 || checkDatabaseAccess(database, loggedInUsername))
                        {
                            printf("Now using database %s.\n", database);
                        }
                        else
                        {
                            printf("You do not have access to this database.\n");
                            continue;
                        }
                    }
                    else
                    {
                        printf("Invalid format. Usage: USE [database]\n");
                        continue;
                    }
                }
                writeLog(loggedInUsername, command);
                send(client_socket, command, strlen(command), 0);
                memset(buffer, 0, BUFFER_SIZE);

                if (recv(client_socket, buffer, BUFFER_SIZE, 0) > 0)
                {
                    printf("%s\n", buffer);
                }
            }
        }
    }
    close(client_socket);
    return 0;
}
```

database.c
```c

```
dump.c
```c
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <jansson.h>
#include <time.h>
#include <string.h>

void dump_database(char *dir_name, FILE *backup_file)
{
    DIR *d;
    struct dirent *dir;
    d = opendir(dir_name);
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if (dir->d_type == DT_REG)
            { // Regular file
                char file_name[100];
                sprintf(file_name, "%s/%s", dir_name, dir->d_name);
                json_error_t error;
                json_t *root = json_load_file(file_name, 0, &error);

                if (!root)
                {
                    fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
                    return;
                }

                if (!json_is_array(root))
                {
                    fprintf(stderr, "error: root is not an array\n");
                    json_decref(root);
                    return;
                }

                size_t i, size;
                json_t *data, *value;
                const char *key;

                char table_name[100];
                strncpy(table_name, dir->d_name, strlen(dir->d_name) - 5); // Subtract 5 to remove ".json"
                table_name[strlen(dir->d_name) - 5] = '\0';

                fprintf(backup_file, "DROP TABLE %s;\n", table_name);

                // For CREATE TABLE statement
                fprintf(backup_file, "CREATE TABLE %s (", table_name);
                json_t *first_row = json_array_get(root, 0);
                json_object_foreach(first_row, key, value)
                {
                    fprintf(backup_file, "%s string, ", key); 
                }
                fseek(backup_file, -2, SEEK_CUR); 
                fprintf(backup_file, ");\n");

                json_array_foreach(root, i, data)
                {
                    fprintf(backup_file, "INSERT INTO %s (", table_name);
                    size_t count = 0, total = json_object_size(data);
                    json_object_foreach(data, key, value)
                    {
                        count++;
                        // Dump the data 
                        fprintf(backup_file, "%s%s", json_string_value(value), count < total ? ", " : "");
                    }
                    fprintf(backup_file, ");\n");
                }

                json_decref(root);
            }
        }
        closedir(d);
    }
}
int main()
{
    DIR *d;
    struct dirent *dir;
    d = opendir("./database");
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if (dir->d_type == DT_DIR && strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0)
            { // Check if it's a directory
                char dir_name[100];
                sprintf(dir_name, "./database/%s", dir->d_name);

                char backup_file_name[100];
                time_t t = time(NULL);
                struct tm tm = *localtime(&t);
                sprintf(backup_file_name, "%s_%04d%02d%02d%02d%02d%02d.backup", dir->d_name, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

                FILE *backup_file = fopen(backup_file_name, "w");
                dump_database(dir_name, backup_file);
                fclose(backup_file);

                char zip_command[150];
                sprintf(zip_command, "zip %s.zip %s", backup_file_name, backup_file_name);
                system(zip_command); // Run the zip command

                // rm file backup
                remove(backup_file_name);
            }
        }
        closedir(d);
    }

    return 0;
}
```
## Test output
![1](https://github.com/ramasedang/praktikum_sisop/assets/100351038/bf5248e9-3565-415d-a4e9-75057666fc4a)
![2](https://github.com/ramasedang/praktikum_sisop/assets/100351038/0723fea4-5de0-4d27-9456-65bb07b782cd)
![3](https://github.com/ramasedang/praktikum_sisop/assets/100351038/8ceed22b-4420-4c04-9f0f-c99f9fd34e9f)
![4](https://github.com/ramasedang/praktikum_sisop/assets/100351038/2d2013ef-9444-4d0a-9f44-a2987345ac7f)
![5](https://github.com/ramasedang/praktikum_sisop/assets/100351038/8fc2b1b4-9da4-4fb8-be81-19629f2c6046)
![6](https://github.com/ramasedang/praktikum_sisop/assets/100351038/93c9ced9-084d-4673-935a-4c549d43606c)
![7](https://github.com/ramasedang/praktikum_sisop/assets/100351038/26462e81-43cc-40a5-b488-ebf88be83550)
![8](https://github.com/ramasedang/praktikum_sisop/assets/100351038/1fc5e06d-50b0-4a8b-aba6-618ecd6b9342)
![0](https://github.com/ramasedang/praktikum_sisop/assets/100351038/5fabbe80-26c8-4b91-afa1-5eea39600f1a)
![10](https://github.com/ramasedang/praktikum_sisop/assets/100351038/c5e87cbc-50bf-4c08-af10-19f8a7c3f2d7)

## Kendala







