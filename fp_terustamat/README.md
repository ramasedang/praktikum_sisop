# sisop-praktikum-modul-3-2023-mhfg-it14

## Laporan Resmi Modul 3 Praktikum Sistem Operasi 2023
---
### Kelompok ITA14:
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
