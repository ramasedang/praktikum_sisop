
# Deskripsi Soal

Pada soal ini diminta untuk mendownload folder .zip yang berisikan file png pemain sepak bola kemudian dikelompokkan dengan ketentuan sebagai berikut:

a. Pertama-tama, Program filter.c akan mengunduh file yang berisikan database para pemain bola. Kemudian dalam program yang sama diminta dapat melakukan extract “players.zip”, kemudian hapus file zip tersebut.

b. Hapus semua pemain yang bukan dari Manchester United yang ada di directory.  

c. Kategorikan pemain dengan 4 kategori pada folder yang berbeda. Untuk kategori folder akan menjadi 4 yaitu Kiper, Bek, Gelandang, dan Penyerang.

d. Setelah mengkategorikan anggota tim Manchester United, Ten Hag memerlukan Kesebelasan Terbaik untuk menjadi senjata utama MU berdasarkan rating terbaik dengan wajib adanya kiper, bek, gelandang, dan penyerang. (Kiper pasti satu pemain). Untuk output nya akan menjadi Formasi_[jumlah bek]-[jumlah gelandang]-[jumlah penyerang].txt dan akan ditaruh di /home/[users]/

# Cara pengerjaan

Pertama-tama, kita harus mendownload zip file yang tertera pada link soal. Untuk melakukan download, digunakan library c libcurl. Setelah file didownload, akan langsung diextract kemudian menggunakan library lzip. Selain itu, dibuat struct untuk menyimpan data pemain yang sudah diextract dan didownload.

```c
typedef struct
{
    char name[50];
    char position[20];
    char image_file[100];
} Player;

void downloadZip()
{
    char *args[] = {"curl", "-L", "-o", "players.zip", "https://drive.google.com/u/0/uc?id=1zEAneJ1-0sOgt13R1gL4i1ONWfKAtwBF&export=download", NULL};
    pid_t pid = fork();

    // check apakah process merupakan child process
    if (pid == 0)
    { 
        execvp(args[0], args);
        perror("execvp");
        exit(EXIT_FAILURE);
    }
    else
    { 
        // check apakah process merupakan parent process
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
            printf("Error extracting players\n");
            exit(EXIT_FAILURE);
        }
    }
}
```

Kemudian dibuat fungsi untuk melakukan pemilahan pemain, dimana pemain yang nama file .pngnya tidak mengandung kata "Manchester United" akan dihapus. Fungsi filterPlayers() akan membuka direktori ./players terlebih dahulu, lalu menghapus pemain yang bukan merupakan pemain Manchester United.

```c
void filterPlayers()
{
    DIR *dir = opendir("players");
    struct dirent *entry;
    char fileName[max_filename];

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG)
        { 
            if (fnmatch("*_ManUtd_*_*.png", entry->d_name, 0) != 0)
            {
                sprintf(fileName, "%s/%s", "players", entry->d_name);
                if (unlink(fileName) != 0)
                {
                    printf("Tidak bisa menghapus file: %s\n", fileName);
                }
            }
        }
    }

    if(dir == NULL){
        printf("Direktori tidak ada");
    }

    closedir(dir);
}
```

Kemudian pemain akan dimasukkan ke kategorinya masing-masing berdasarkan posisi. Pertama-tama dibuat sebuah array yang berisikan jenis posisi pemain yaitu Kiper, Bek, Gelandang, dan Penyerang. Kemudian fungsi ini membuat direktori dengan nama yang ada di array positions dengan dengan execvp(). 

```c
void categorizePlayers()
{
    DIR *dir = opendir("./players");
    struct dirent *entry;
    char *image_file;
    char *positions[] = {"Kiper", "Bek", "Gelandang", "Penyerang"};
    int status;
    char buffer[1024];
    size_t bytes_read;


    if (dir == NULL)
    {
        printf("Direktori belum dibuat \n");
        exit(EXIT_FAILURE);
    }

    // buat direktori baru
    for (int i = 0; i < 4; i++)
    {
        char dir_name[max_filename];
        sprintf(dir_name, "./%s", positions[i]);

        char* args[] = {"mkdir", dir_name, NULL};
        pid_t pid = fork();

        if (pid == -1)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            execvp(args[0], args);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
        else
        {
            
            waitpid(pid, &status, 0);
            if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
            {
                printf("Error membuat direktori: %s\n", dir_name);
                exit(EXIT_FAILURE);
            }
        }
    }

    rewinddir(dir);
```
Apabila direktori yang diperlukan sudah dibentuk, maka pemain akan dimasukkan ke direktori masing-masing berdasarkan posisinya. Pertama-tama fungsi ini akan mengecek terlebih dahulu apakah file yang ada merupakan file .png. Apabila file merupakan file .png, maka akan dimasukkan ke folder yang sesuai dengan posisinya masing-masing.

```c
  while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG)
        { 
            // cek bentuk file
            image_file = entry->d_name;
            char *ext = strrchr(image_file, '.');
            
            // cek kalau extension tipe .png
            if (ext != NULL && strcmp(ext, ".png") == 0)
            {
                
                for (int i = 0; i < 4; i++)
                {
                    if (strstr(image_file, positions[i]) != NULL)
                    {
                        char new_path[max_filename];
                        sprintf(new_path, "./%s/%s", positions[i], image_file);

                        char old_path[max_filename];
                        sprintf(old_path, "./players/%s", image_file);

                        FILE *old_file = fopen(old_path, "rb");
                        
                        FILE *new_file = fopen(new_path, "wb");
                        if (new_file == NULL)
                        {
                            perror("Unable to create file");
                            fclose(old_file);
                            break;
                        }

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
```
Apabila pemain telah dimasukkan ke folder masing-masing, maka kita dapat membuat tim sesuai dengan formasi yang diinginkan lewat fungsi buatTim(). Fungsi ini menerima 3 parameter integer yaitu jumlah bek, gelandang, dan penyerang. Pertama-tama akan dibuat file .txt di /home/user dengan format nama Formasi_Bek_Gelandang_Penyerang. Lalu deklarasi pemain yang tersedia menurut posisinya masing-masing dalam sebuah array yaitu bek, gelandang, dan penyerang. Kemudian pemain akan disort berdasarkan rating tertinggi, lalu diprint sesuai dengan jumlah pemain yang diperlukan di tiap posisi.

```c
void buatTim(int bek, int gelandang, int striker)
{

    // Buat direktori sesuai dengan format nama yang ditentukan
    char fileName[50];
    sprintf(fileName, "/home/reyhanqb/Formasi_%d_%d_%d.txt", bek, gelandang, striker);
    FILE *fp = fopen(fileName, "w");

    // insert formasi
    fprintf(fp, "Formasi %d-%d-%d \n", bek, gelandang, striker);

    // inisaliasi player MU
    char *defenders[8] = {"Bissaka_ManUtd_80_Bek", "Dalot_ManUtd_Bek_80", "Lindelof_ManUtd_Bek_79", "Maguire_ManUtd_Bek_80", "Malacia_ManUtd_Bek_79", "Martinez_ManUtd_Bek_83", "Shaw_ManUtd_Bek_82", "Varane_ManUtd_Bek_85"};
    char *midfielders[7] = {"Casemiro_ManUtd_89", "Eriksen_ManUtd_83", "Fernandes_ManUtd_86", "Fred_ManUtd_Gelandang_80", "McTominay_ManUtd_Gelandang_79", "Sabitzer_ManUtd_Gelandang_81", "VanDeBeek_ManUtd_Gelandang_77"};
    char *strikers[5] = {"Antony_ManUtd_Penyerang_80", "Martial_ManUtd_Penyerang_81", "Rashford_ManUtd_Penyerang_84", "Sancho_ManUtd_Penyerang_83", "Weghorst_ManUtd_Penyerang_83"};

    // sort player sesuai rating
    int jmlBek = sizeof(defenders) / sizeof(defenders[0]);
    int jmlGelandang = sizeof(midfielders) / sizeof(midfielders[0]);
    int jmlPenyerang = sizeof(strikers) / sizeof(strikers[0]);
    for (int i = 0; i < jmlBek - 1; i++)
    {
        for (int j = i + 1; j < jmlBek; j++)
        {
            if (defenders[j] < defenders[i])
            {
                char *temp = defenders[i];
                defenders[i] = defenders[j];
                defenders[j] = temp;
            }
        }
    }
    for (int i = 0; i < jmlGelandang - 1; i++)
    {
        for (int j = i + 1; j < jmlGelandang; j++)
        {
            if (midfielders[j] < midfielders[i])
            {
                char *temp = midfielders[i];
                midfielders[i] = midfielders[j];
                midfielders[j] = temp;
            }
        }
    }
    for (int i = 0; i < jmlPenyerang - 1; i++)
    {
        for (int j = i + 1; j < jmlPenyerang; j++)
        {
            if (strikers[j] < strikers[i])
            {
                char *temp = strikers[i];
                strikers[i] = strikers[j];
                strikers[j] = temp;
            }
        }
    }

    // Print player dengan rating tertinggi pada tiap posisi


    // Karena diminta skuad terbaik, otomatis kipernya De Gea
    fprintf(fp, "\n Kiper: \n");
    fprintf(fp, "\n DeGea_ManUtd_Kiper_87 \n");

    fprintf(fp, "\n Bek: \n");
    for (int i = jmlBek - 1; i >= (jmlBek - bek) && i >= 0; i--)
    {
        fprintf(fp, "%s\n", defenders[i]);
    }
    fprintf(fp, "\n Gelandang: \n");
    for (int i = jmlGelandang - 1; i >= (jmlGelandang - gelandang) && i >= 0; i--)
    {
        fprintf(fp, "%s\n", midfielders[i]);
    }
    fprintf(fp, "\n Penyerang: \n");
    for (int i = jmlPenyerang - 1; i >= (jmlPenyerang - striker) && i >= 0; i--)
    {
        fprintf(fp, "%s\n", strikers[i]);
    }

    fclose(fp);
}

```
Kemudian fungsi yang ada akan dijalankan di main(). Setelah file diextract dan difilter, user dapat melakukan input formasi dengan menentukan jumlah bek, gelandang, dan penyerang yang diinginkan. Input user akan dimasukkan ke parameter fungsi buatTim(), lalu akan dibuat file .txt yang berisikan pemain-pemain terbaik sesuai dengan formasi yang ditentukan user.
```c
int main()
{
    int bek, gelandang, striker;
    

    downloadZip();
    extractZip();
    filterPlayers();
    categorizePlayers();

    printf("Masukkan jumlah bek : ");
    scanf("%d", &bek);

    printf("Masukkan jumlah gelandang : ");
    scanf("%d", &gelandang);

    printf("Masukkan jumlah penyerang : ");
    scanf("%d", &striker);

    buatTim(bek, gelandang, striker);

    return 0;
}
```

# Source Code

```c
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

#define max_players 100
#define team "ManUtd"
#define max_filename 256

// Buat struct untuk deklarasi player yang baru didownload
typedef struct
{
    char name[50];
    char position[20];
    char image_file[100];
} Player;


void extractZip()
{
    char *args[] = {"unzip", "players.zip", NULL};
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
            printf("Error extracting players\n");
            exit(EXIT_FAILURE);
        }
    }
}



void downloadZip()
{
    char *args[] = {"curl", "-L", "-o", "players.zip", "https://drive.google.com/u/0/uc?id=1zEAneJ1-0sOgt13R1gL4i1ONWfKAtwBF&export=download", NULL};
    pid_t pid = fork();

    // check apakah process merupakan child process
    if (pid == 0)
    { 
        execvp(args[0], args);
        perror("execvp");
        exit(EXIT_FAILURE);
    }
    else
    { 
        // check apakah process merupakan parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
        {
            printf("Error downloading players\n");
            exit(EXIT_FAILURE);
        }
    }
}

void categorizePlayers()
{
    DIR *dir = opendir("./players");
    struct dirent *entry;
    char *image_file;
    char *positions[] = {"Kiper", "Bek", "Gelandang", "Penyerang"};
    int status;
    char buffer[1024];
    size_t bytes_read;


    if (dir == NULL)
    {
        printf("Direktori belum dibuat \n");
        exit(EXIT_FAILURE);
    }

    // buat direktori baru
    for (int i = 0; i < 4; i++)
    {
        char dir_name[max_filename];
        sprintf(dir_name, "./%s", positions[i]);

        char* args[] = {"mkdir", dir_name, NULL};
        pid_t pid = fork();

        if (pid == -1)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            execvp(args[0], args);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
        else
        {
            
            waitpid(pid, &status, 0);
            if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
            {
                printf("Error membuat direktori: %s\n", dir_name);
                exit(EXIT_FAILURE);
            }
        }
    }

    rewinddir(dir);

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG)
        { 
            // cek bentuk file
            image_file = entry->d_name;
            char *ext = strrchr(image_file, '.');
            
            // cek kalau extension tipe .png
            if (ext != NULL && strcmp(ext, ".png") == 0)
            {
                
                for (int i = 0; i < 4; i++)
                {
                    if (strstr(image_file, positions[i]) != NULL)
                    {
                        char new_path[max_filename];
                        sprintf(new_path, "./%s/%s", positions[i], image_file);

                        char old_path[max_filename];
                        sprintf(old_path, "./players/%s", image_file);

                        FILE *old_file = fopen(old_path, "rb");
                        
                        FILE *new_file = fopen(new_path, "wb");
                        if (new_file == NULL)
                        {
                            perror("Unable to create file");
                            fclose(old_file);
                            break;
                        }

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

void filterPlayers()
{
    DIR *dir = opendir("players");
    struct dirent *entry;
    char fileName[max_filename];

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG)
        { 
            if (fnmatch("*_ManUtd_*_*.png", entry->d_name, 0) != 0)
            {
                sprintf(fileName, "%s/%s", "players", entry->d_name);
                if (unlink(fileName) != 0)
                {
                    printf("Tidak bisa menghapus file: %s\n", fileName);
                }
            }
        }
    }

    if(dir == NULL){
        printf("Direktori tidak ada");
    }

    closedir(dir);
}

void buatTim(int bek, int gelandang, int striker)
{

    // Buat direktori sesuai dengan format nama yang ditentukan
    char fileName[50];
    sprintf(fileName, "/home/reyhanqb/Formasi_%d_%d_%d.txt", bek, gelandang, striker);
    FILE *fp = fopen(fileName, "w");

    // insert formasi
    fprintf(fp, "Formasi %d-%d-%d \n", bek, gelandang, striker);

    // inisaliasi player MU
    char *defenders[8] = {"Bissaka_ManUtd_80_Bek", "Dalot_ManUtd_Bek_80", "Lindelof_ManUtd_Bek_79", "Maguire_ManUtd_Bek_80", "Malacia_ManUtd_Bek_79", "Martinez_ManUtd_Bek_83", "Shaw_ManUtd_Bek_82", "Varane_ManUtd_Bek_85"};
    char *midfielders[7] = {"Casemiro_ManUtd_89", "Eriksen_ManUtd_83", "Fernandes_ManUtd_86", "Fred_ManUtd_Gelandang_80", "McTominay_ManUtd_Gelandang_79", "Sabitzer_ManUtd_Gelandang_81", "VanDeBeek_ManUtd_Gelandang_77"};
    char *strikers[5] = {"Antony_ManUtd_Penyerang_80", "Martial_ManUtd_Penyerang_81", "Rashford_ManUtd_Penyerang_84", "Sancho_ManUtd_Penyerang_83", "Weghorst_ManUtd_Penyerang_83"};

    // sort player sesuai rating
    int jmlBek = sizeof(defenders) / sizeof(defenders[0]);
    int jmlGelandang = sizeof(midfielders) / sizeof(midfielders[0]);
    int jmlPenyerang = sizeof(strikers) / sizeof(strikers[0]);
    for (int i = 0; i < jmlBek - 1; i++)
    {
        for (int j = i + 1; j < jmlBek; j++)
        {
            if (defenders[j] < defenders[i])
            {
                char *temp = defenders[i];
                defenders[i] = defenders[j];
                defenders[j] = temp;
            }
        }
    }
    for (int i = 0; i < jmlGelandang - 1; i++)
    {
        for (int j = i + 1; j < jmlGelandang; j++)
        {
            if (midfielders[j] < midfielders[i])
            {
                char *temp = midfielders[i];
                midfielders[i] = midfielders[j];
                midfielders[j] = temp;
            }
        }
    }
    for (int i = 0; i < jmlPenyerang - 1; i++)
    {
        for (int j = i + 1; j < jmlPenyerang; j++)
        {
            if (strikers[j] < strikers[i])
            {
                char *temp = strikers[i];
                strikers[i] = strikers[j];
                strikers[j] = temp;
            }
        }
    }

    // Print player dengan rating tertinggi pada tiap posisi


    // Karena diminta skuad terbaik, otomatis kipernya De Gea
    fprintf(fp, "\n Kiper: \n");
    fprintf(fp, "\n DeGea_ManUtd_Kiper_87 \n");

    fprintf(fp, "\n Bek: \n");
    for (int i = jmlBek - 1; i >= (jmlBek - bek) && i >= 0; i--)
    {
        fprintf(fp, "%s\n", defenders[i]);
    }
    fprintf(fp, "\n Gelandang: \n");
    for (int i = jmlGelandang - 1; i >= (jmlGelandang - gelandang) && i >= 0; i--)
    {
        fprintf(fp, "%s\n", midfielders[i]);
    }
    fprintf(fp, "\n Penyerang: \n");
    for (int i = jmlPenyerang - 1; i >= (jmlPenyerang - striker) && i >= 0; i--)
    {
        fprintf(fp, "%s\n", strikers[i]);
    }

    fclose(fp);
}

int main()
{
    int bek, gelandang, striker;
    

    downloadZip();
    extractZip();
    filterPlayers();
    categorizePlayers();

    printf("Masukkan jumlah bek : ");
    scanf("%d", &bek);

    printf("Masukkan jumlah gelandang : ");
    scanf("%d", &gelandang);

    printf("Masukkan jumlah penyerang : ");
    scanf("%d", &striker);

    buatTim(bek, gelandang, striker);

    return 0;
}
```

# Test Output

Program setelah di run
![test_run_filter](https://user-images.githubusercontent.com/107137535/230083534-51421004-0798-4c13-9ea6-cc72987c8b19.png)
File .txt yang dihasilkan
![output_txt_file](https://user-images.githubusercontent.com/107137535/230083521-21ad5c96-6fe5-4174-b225-63b3b2615ade.png)


# Kendala
Beberapa kali download zip file gagal dan file txt yang dihasilkan outputnya tidak sesuai, selain itu tidak boleh menggunakan system() dan mkdir() maka harus dilakukan beberapa step tambahan.
