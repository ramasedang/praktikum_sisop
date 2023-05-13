# sisop-praktikum-modul-3-2023-mhfg-it14

## Laporan Resmi Modul 3 Praktikum Sistem Operasi 2023
---
### Kelompok ITA14:
Aloysius Bataona Manullang - 5027211008  
Athaya Reyhan Nugroho - 5027211067    
Moh. Sulthan Arief Rahmatullah - 5027211045


---
# Soal 1


## Analisis

Soal tersebut mengharuskan kita untuk membuat program kompresi file menggunakan algoritma Huffman dengan menggunakan konsep pipes dan fork dan menggunakan 2 pipes. Pada parent process, kita harus membaca file yang akan dikompresi dan menghitung frekuensi kemunculan huruf pada file tersebut. Selanjutnya, hasil perhitungan frekuensi tiap huruf dikirim ke child process. Pada child process, dilakukan proses kompresi file menggunakan algoritma Huffman berdasarkan jumlah frekuensi setiap huruf yang diterima. Kemudian, Huffman tree disimpan pada file terkompresi. Setiap huruf pada file diubah menjadi kode Huffman dan kode tersebut dikirimkan ke program dekompresi menggunakan pipe. Hasil kompresi dikirim kembali ke parent process. Pada parent process, Huffman tree dari file terkompresi dibaca, kode Huffman juga dibaca, dan dilakukan proses dekompresi. Selanjutnya, di parent process, jumlah bit setelah dilakukan kompresi menggunakan algoritma Huffman dihitung dan perbandingan jumlah bit sebelum dan setelah dikompresi ditampilkan di layar. Selain itu, pada encoding ASCII, setiap karakter diwakili oleh 8 bit atau 1 byte. Karakter selain huruf tidak masuk ke perhitungan jumlah bit dan frekuensi kemunculannya. Untuk mempermudah, huruf kecil harus diubah menjadi huruf kapital sebelum proses kompresi.

## Cara pengerjaan soal 1 : 
Pertama kita menginisialisi pada fungsi main() di mana terdapat 2 pipes dan fork untuk parent proses dan child proses : 

```c
// Initialize pipes
    int fd1[2]; // Used to store two ends of first pipe
    int fd2[2]; // Used to store two ends of second pipe
```

Lalu kita membuat fungsi untuk mengubah setiap karakter pada file menjadi huruf kapital : 

```c
int freq[MAX_CHAR] = {0};
char symbol;
int total_bits = 0;
int total_chars = 0; // Initialize total_chars variable
while (read(input_fd, &symbol, 1) > 0)
{
    symbol = toupper(symbol); // Convert lowercase letters to uppercase

    if (symbol >= 'A' && symbol <= 'Z')
    {
        freq[(int)symbol]++;
        total_bits += 8; // Add 8 bits for each symbol found
        total_chars++; // Increment total_chars by 1 for each symbol found
    }
} 
```

Pada parent process, kita menghitung hasil frekuensi dari setiap karakter dari fungsi di atas :
```c
        printf("Frequency of each letter in file %s:\n", input_filename);
        int i = 0;
        int size = 0;
        while (i < MAX_CHAR)
        {
            if (freq[i] > 0)
            {
                printf("%c: %d\n", i, freq[i]);
                ++size;
            }
            i++;
        }
        // Send symbol frequency to child process
        write(fd1[1], freq, sizeof(freq));
        close(fd1[1]);
```
Setelah itu di kirim pada child process : 
```c
int freq[MAX_CHAR] = {0};
read(fd1[0], freq, sizeof(freq));
close(fd1[0]);

```
Lalu, di child process akan melakukan kompersi berdasarkan hasil jumlah frekuensi karakter menggunkan huffman code :
```c
    char data[MAX_CHAR];
    int i = 0;
    while (i < MAX_CHAR)
    {
        if (freq[i] > 0)
        {
            data[i] = (char)i;
        }
        i++;
    }
```
Kemudian pada child process akan disimpan Huffman tree pada file terkompresi. Untuk setiap huruf pada file, diubah karakter tersebut menjadi kode Huffman dan kirimkan kode tersebut ke program dekompresi menggunakan pipe. Hasil kompresi lalu dikirim ke parent process. Lalu, di parent process baca Huffman tree dari file terkompresi. Baca kode Huffman dan melakukan dekompresi.

```c
    struct HfNode *root = build_hftree(data, freq);
    int compressed_bits = calc_compressed_bits(root, 0);

    char code[MAX_CHAR];
    char compressed[MAX_CHAR];
    memset(compressed, 0, sizeof(compressed));

    // Find Huffman code for each symbol and compress the data
    find_hfcode(root, code, 0, data, compressed);

    // Send compressed bits and Huffman tree to parent process
    write(fd2[1], &compressed_bits, sizeof(compressed_bits));
    write(fd2[1], &root, sizeof(struct HfNode *));
```
Fungsi struct HfNode build hftree: 
```c
// Function to build the Huffman binary tree
struct HfNode *build_hftree(char *data, int *freq)
{
    struct HfNode **nodes = (struct HfNode **)malloc(MAX_CHAR * sizeof(struct HfNode *));
    int i = 0;
    int j = 0;
    while (i < MAX_CHAR)
    {
        if (freq[i] > 0)
        {
            nodes[j] = new_hfnode(data[i], freq[i]);
            j++;
        }
    i++;
    }

    int num_nodes = j;

    // Build Huffman binary tree
    while (num_nodes > 1)
    {
        qsort(nodes, num_nodes, sizeof(struct HfNode *), compare_hfnodes);

        struct HfNode *left = nodes[0];
        struct HfNode *right = nodes[1];

        struct HfNode *parent = new_hfnode('\0', left->freq + right->freq);
        parent->left = left;
        parent->right = right;

        nodes[0] = parent;
        nodes[1] = nodes[num_nodes - 1];

        num_nodes--;
    }
    return nodes[0];
}

```
Fungsi find_hfcode :
```c
// Function to find the Huffman code for each symbol in the binary tree and compress the data
void find_hfcode(struct HfNode *root, char *code, int code_len, char *input, char *compressed)
{
    if (root == NULL)
    return;

    // If node is a leaf node, add Huffman code to compressed data
    if (root->left == NULL && root->right == NULL)
    {
        code[code_len] = '\0';
        printf("%c: %s\n", root->symbol, code);
        compressed[(int)root->symbol] = code_len;
        strncpy(&compressed[(int)root->symbol + 1], code, code_len);
        return;
    }

    // Traverse left child and add '0' to Huffman code
    code[code_len] = '0';
    find_hfcode(root->left, code, code_len + 1, input, compressed);

    // Traverse right child and add '1' to Huffman code
    code[code_len] = '1';
    find_hfcode(root->right, code, code_len + 1, input, compressed);

}
```
Dan terakhir pada parent process, menghitung jumlah bit setelah dilakukan kompresi menggunakan algoritma Huffman dan menampilkan pada layar perbandingan jumlah bit antara setelah dan sebelum dikompres dengan algoritma Huffman

```c
        while (read(input_fd, &symbol, 1) > 0)
        {
            symbol = toupper(symbol); // Convert lowercase letters to uppercase

            if (symbol >= 'A' && symbol <= 'Z')
            {
                freq[(int)symbol]++;
                total_bits += 8; // Add 8 bits for each symbol found
                total_chars++; // Increment total_chars by 1 for each symbol found
            }
        }

        int i = 0;
        int size = 0;
        while (i < MAX_CHAR)
        {
            if (freq[i] > 0)
            {
                printf("%c: %d\n", i, freq[i]);
                ++size;
            }
            i++;
        }

        int compressed_bits;
        read(fd2[0], &compressed_bits, sizeof(compressed_bits));

        struct HfNode *root = NULL;
        read(fd2[0], &root, sizeof(struct HfNode *));
        close(fd2[0]);

	printf("\nTotal bits in file %s: %d\n", input_filename, total_bits);
   	printf("Total bits after compression: %d\n", compressed_bits + size * 8 + total_chars);
   	float compression_ratio = (float)(compressed_bits + size * 8 + total_chars) / total_bits;
    	printf("Compression ratio: %.2f\n", compression_ratio);
```
```c
// Function to calculate the number of bits needed to compress the data using the Huffman binary tree
int calc_compressed_bits(struct HfNode *root, int depth)
{
    if (root == NULL)
    return 0;

    if (root->left == NULL && root->right == NULL)
        return root->freq * depth;

    return calc_compressed_bits(root->left, depth + 1) + calc_compressed_bits(root->right, depth + 1);

}
```
## Source Code
```c++
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <ctype.h>

#define MAX_CHAR 256 // number of different ASCII characters

// Node for Huffman binary tree
struct HfNode;
// Function to create a new node in the Huffman binary tree
struct HfNode *new_hfnode(char symbol, int freq);
// Function to compare the frequency of two Huffman nodes
int compare_hfnodes(const void *a, const void *b);
// Function to build the Huffman binary tree
struct HfNode *build_hftree(char *data, int *freq);
// Function to calculate the number of bits needed to compress the data using the Huffman binary tree
int calc_compressed_bits(struct HfNode *root, int depth);
// Function to find the Huffman code for each symbol in the binary tree and compress the data
void find_hfcode(struct HfNode *root, char *code, int code_len, char *input, char *compressed);

int main(int argc, char *argv[])
{
    // Check if the correct number of arguments are provided
    if (argc != 2)
    {
        printf("Usage: %s [input_filename]\n", argv[0]);
        return 1;
    }

    char *input_filename = argv[1];


    int input_fd = open(input_filename, O_RDONLY);
    if (input_fd == -1)
    {
        printf("Error: Failed to open file %s\n", input_filename);
        return 1;
    }

    // Initialize pipes
    int fd1[2]; // Used to store two ends of first pipe
    int fd2[2]; // Used to store two ends of second pipe

    if (pipe(fd1) == -1)
    {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }
    if (pipe(fd2) == -1)
    {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }

    pid_t p;

    // Fork process
    p = fork();

    if (p < 0)
    {
        fprintf(stderr, "fork Failed");
        return 1;
    }

    // Parent process
    else if (p > 0)
    {
        close(fd1[0]); // Close reading end of first pipe
        close(fd2[1]); // Close writing end of second pipe

        int freq[MAX_CHAR] = {0};
        char symbol;
        int total_bits = 0;
        int total_chars = 0; // Initialize total_chars variable
        while (read(input_fd, &symbol, 1) > 0)
        {
            symbol = toupper(symbol); // Convert lowercase letters to uppercase

            if (symbol >= 'A' && symbol <= 'Z')
            {
                freq[(int)symbol]++;
                total_bits += 8; // Add 8 bits for each symbol found
                total_chars++; // Increment total_chars by 1 for each symbol found
            }
        }

        printf("Frequency of each letter in file %s:\n", input_filename);
        int i = 0;
        int size = 0;
        while (i < MAX_CHAR)
        {
            if (freq[i] > 0)
            {
                printf("%c: %d\n", i, freq[i]);
                ++size;
            }
            i++;
        }
        printf("\n");

        close(input_fd);

        // Send symbol frequency to child process
        write(fd1[1], freq, sizeof(freq));
        close(fd1[1]);

        // Wait for the child process to send compressed bits and Huffman tree
        int compressed_bits;
        read(fd2[0], &compressed_bits, sizeof(compressed_bits));

        struct HfNode *root = NULL;
        read(fd2[0], &root, sizeof(struct HfNode *));
        close(fd2[0]);

	printf("\nTotal bits in file %s: %d\n", input_filename, total_bits);
   	printf("Total bits after compression: %d\n", compressed_bits + size * 8 + total_chars);
   	float compression_ratio = (float)(compressed_bits + size * 8 + total_chars) / total_bits;
   	printf("Compression ratio: %.2f\n", compression_ratio);


}

// Child process
else
{
    close(fd1[1]); // Close writing end of first pipe
    close(fd2[0]); // Close reading end of second pipe

    int freq[MAX_CHAR] = {0};
    read(fd1[0], freq, sizeof(freq));
    close(fd1[0]);

    char data[MAX_CHAR];
    int i = 0;
    while (i < MAX_CHAR)
    {
        if (freq[i] > 0)
        {
            data[i] = (char)i;
        }
        i++;
    }

    struct HfNode *root = build_hftree(data, freq);
    int compressed_bits = calc_compressed_bits(root, 0);


    char code[MAX_CHAR];
    char compressed[MAX_CHAR];
    memset(compressed, 0, sizeof(compressed));

    // Find Huffman code for each symbol and compress the data
    find_hfcode(root, code, 0, data, compressed);

    // Send compressed bits and Huffman tree to parent process
    write(fd2[1], &compressed_bits, sizeof(compressed_bits));
    write(fd2[1], &root, sizeof(struct HfNode *));
    close(fd2[1]);
}

return 0;

}

// Node for Huffman binary tree
struct HfNode
{
char symbol;
int freq;
struct HfNode *left;
struct HfNode *right;
};

// Function to create a new node in the Huffman binary tree
struct HfNode *new_hfnode(char symbol, int freq)
{
struct HfNode *node = (struct HfNode *)malloc(sizeof(struct HfNode));
node->symbol = symbol;
node->freq = freq;
node->left = NULL;
node->right = NULL;

return node;

}

// Function to compare the frequency of two Huffman nodes
int compare_hfnodes(const void *a, const void *b)
{
struct HfNode **na = (struct HfNode **)a;
struct HfNode **nb = (struct HfNode **)b;

if ((*na)->freq > (*nb)->freq)
    return 1;
else if ((*na)->freq < (*nb)->freq)
    return -1;
else
    return 0;

}

// Function to build the Huffman binary tree
struct HfNode *build_hftree(char *data, int *freq)
{
    struct HfNode **nodes = (struct HfNode **)malloc(MAX_CHAR * sizeof(struct HfNode *));
    int i = 0;
    int j = 0;
    while (i < MAX_CHAR)
    {
        if (freq[i] > 0)
        {
            nodes[j] = new_hfnode(data[i], freq[i]);
            j++;
        }
    i++;
    }

    int num_nodes = j;

    // Build Huffman binary tree
    while (num_nodes > 1)
    {
        qsort(nodes, num_nodes, sizeof(struct HfNode *), compare_hfnodes);

        struct HfNode *left = nodes[0];
        struct HfNode *right = nodes[1];

        struct HfNode *parent = new_hfnode('\0', left->freq + right->freq);
        parent->left = left;
        parent->right = right;

        nodes[0] = parent;
        nodes[1] = nodes[num_nodes - 1];

        num_nodes--;
    }

    return nodes[0];

}

// Function to calculate the number of bits needed to compress the data using the Huffman binary tree
int calc_compressed_bits(struct HfNode *root, int depth)
{
    if (root == NULL)
    return 0;

    if (root->left == NULL && root->right == NULL)
        return root->freq * depth;

    return calc_compressed_bits(root->left, depth + 1) + calc_compressed_bits(root->right, depth + 1);

}

// Function to find the Huffman code for each symbol in the binary tree and compress the data
void find_hfcode(struct HfNode *root, char *code, int code_len, char *input, char *compressed)
{
    if (root == NULL)
    return;

    // If node is a leaf node, add Huffman code to compressed data
    if (root->left == NULL && root->right == NULL)
    {
        code[code_len] = '\0';
        printf("%c: %s\n", root->symbol, code);
        compressed[(int)root->symbol] = code_len;
        strncpy(&compressed[(int)root->symbol + 1], code, code_len);
        return;
    }

    // Traverse left child and add '0' to Huffman code
    code[code_len] = '0';
    find_hfcode(root->left, code, code_len + 1, input, compressed);

    // Traverse right child and add '1' to Huffman code
    code[code_len] = '1';
    find_hfcode(root->right, code, code_len + 1, input, compressed);

}

```
## Test Output
![image](https://github.com/ramasedang/praktikum_sisop/assets/100351038/75ab96a8-a26e-4266-9b09-7ab289b0a1c3)

## Kendala
Kendala yang dialami adalah masih kurang pemahaan tentang algoritma huffman untuk lossless compression sehingga kesulitan dalam pengerjaan.
# Soal 2
## Analisa soal:
Tugas yang diberikan adalah membuat tiga program dalam bahasa pemrograman C terkait operasi matriks. Program pertama (kalian.c) akan melakukan perkalian matriks, program kedua (cinta.c) akan mengambil hasil perkalian matriks dari program pertama menggunakan konsep shared memory, dan program ketiga (sisop.c) akan melakukan hal yang sama tanpa menggunakan thread dan multithreading.

Program kalian.c:

Melakukan perkalian matriks dengan ukuran 4×2 dan 2×5.
Menghasilkan matriks hasil perkalian dan menampilkannya di layar.
Program cinta.c:

Mengambil hasil perkalian matriks dari program kalian.c menggunakan shared memory.
Menampilkan matriks hasil perkalian dari shared memory ke layar.
Menghitung faktorial dari setiap elemen matriks.
Menampilkan hasil faktorial dalam format matriks.
Program sisop.c:

Sama dengan cinta.c, tetapi tanpa menggunakan thread dan multithreading.
## Cara pengerjaan soal 2:
Pertama kita membuat untuk fungsi fungsi yang akan digunakan
```c
.........
void isi_matriks(int baris, int kolom, long long matriks[baris][kolom], int min, int max);

void cetak_matriks(int baris, int kolom, long long matriks[baris][kolom]);

void kalikan_matriks(int baris_a, int kolom_a, long long matriks_a[baris_a][kolom_a],
                     int baris_b, int kolom_b, long long matriks_b[baris_b][kolom_b],
                     long long hasil[baris_a][kolom_b]);

void copy_to_shared_mem(long long hasil[INI_BARIS_A_WOI][KOLOM_B]);
.........
```
Diatas adalah fungsi fungsi yang dipakai untuk membuat program `kalian.c`.
Setalah kita membuat fungsi-fungsinya berikut int main nya
```c
int main()
{
    srand(time(NULL));
    long long matriks_a[INI_BARIS_A_WOI][KOLOM_A];
    long long matriks_b[BARIS_B][KOLOM_B];
    long long hasil[INI_BARIS_A_WOI][KOLOM_B];

    isi_matriks(INI_BARIS_A_WOI, KOLOM_A, matriks_a, 1, 5);
    isi_matriks(BARIS_B, KOLOM_B, matriks_b, 1, 4);

    printf("M a t r i k s A:\n");
    cetak_matriks(INI_BARIS_A_WOI, KOLOM_A, matriks_a);
    printf("M a t r i k s B:\n");
    cetak_matriks(BARIS_B, KOLOM_B, matriks_b);

    kalikan_matriks(INI_BARIS_A_WOI, KOLOM_A, matriks_a, BARIS_B, KOLOM_B, matriks_b, hasil);

    printf("HAsil_perkAlian_matrIks:\n");
    cetak_matriks(INI_BARIS_A_WOI, KOLOM_B, hasil);

    copy_to_shared_mem(hasil);

    return 0;
}
```
Berikut juga hal terpenting dalam kalian.c adalah untuk menyimpan matriks hasil perkalian kedalam shared memory
```c
void copy_to_shared_mem(long long hasil[INI_BARIS_A_WOI][KOLOM_B])
{
    // Buat shared memory untuk hasil perkalian matriks
    int shmid_hasil = shmget(1234, INI_BARIS_A_WOI * KOLOM_B * sizeof(int), IPC_CREAT | 0666);
    long long(*shm_hasil)[KOLOM_B] = shmat(shmid_hasil, NULL, 0);

    // Salin hasil perkalian matriks ke shared memory
    int i = 0, j = 0;
    while (i < INI_BARIS_A_WOI)
    {
        j = 0;
        while (j < KOLOM_B)
        {
            shm_hasil[i][j] = hasil[i][j];
            j++;
        }
        i++;
    }
}
```
Selanjutnya kita membuat cinta.c . berikut fungsi fungsi yg digunakan untuk membuat cinta.c. yang paling penting disini adalah untuk mengambil value matriks yang disimpan pada shared memory
```c
void inisialisasi_shared_memory()
{
    int shmid_hasil = shmget(1234, BARIS_A * KOLOM_B * sizeof(int), 0666);
    shm_hasil = shmat(shmid_hasil, NULL, 0);
}
```
diatas adalah fungsi untuk mengambil nilai matriks dari shared memory, selanjutnya adalah sisa kode untuk faktorial dan menghitung waktunya
```c
void cetak_hasil_perkalian_matriks()
{
    printf("Hasil perkalian matriks:\n");
    for (int i = 0; i < BARIS_A; i++)
    {
        for (int j = 0; j < KOLOM_B; j++)
        {
            printf("%lld ", shm_hasil[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

unsigned long long faktorial(unsigned long long x)
{
    unsigned long long hasil = 1;
    for (int i = 1; i <= x; i++)
    {
        hasil *= i;
    }
    return hasil;
}

void hitung_faktorial_setiap_elemen_matriks()
{
    printf("Menghitung faktorial setiap elemen matriks...\n");

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < BARIS_A; i++)
    {
        for (int j = 0; j < KOLOM_B; j++)
        {
            shm_hasil[i][j] = faktorial(shm_hasil[i][j]);
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    printf("Selesai menghitung faktorial setiap elemen matriks.\n");

    unsigned long long start_nsec = start.tv_sec * 1000000000 + start.tv_nsec;
    unsigned long long end_nsec = end.tv_sec * 1000000000 + end.tv_nsec;
    unsigned long long duration = end_nsec - start_nsec;
    double seconds = (double)duration / 1000000000;
    printf("\nWaktu eksekusi: %.9f detik\n", seconds);
}

void cetak_hasil_faktorial_setiap_elemen_matriks()
{
    printf("\nHasil faktorial matriks:\n");
    for (int i = 0; i < BARIS_A; i++)
    {
        for (int j = 0; j < KOLOM_B; j++)
        {
            printf("%llu ", shm_hasil[i][j]);
        }
        printf("\n");
    }
}

void hapus_shared_memory(int shmid_hasil)
{
    shmdt(shm_hasil);
    shmctl(shmid_hasil, IPC_RMID, NULL);
}
```

untuk sisop.c nya sama cuma bedanya pada sisop.c menggunakan multithread untuk menghitung faktorialnya, berikut fungsi yang me implementasikan multithread
```c
void cari_faktorial_setiap_elemen_matriks(pthread_t threads[])
{
    int counter = 0;
    for (int i = 0; i < BARIS_A; i++)
    {
        for (int j = 0; j < KOLOM_B; j++)
        {
            unsigned long long *arg = &shm_hasil[i][j];
            pthread_create(&threads[counter++], NULL, faktorial, arg);
        }
    }
}
```

## Source code
kalian.c
```c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define INI_BARIS_A_WOI 4
#define KOLOM_A 2
#define BARIS_B 2
#define KOLOM_B 5

void isi_matriks(int baris, int kolom, long long matriks[baris][kolom], int min, int max);

void cetak_matriks(int baris, int kolom, long long matriks[baris][kolom]);

void kalikan_matriks(int baris_a, int kolom_a, long long matriks_a[baris_a][kolom_a],
                     int baris_b, int kolom_b, long long matriks_b[baris_b][kolom_b],
                     long long hasil[baris_a][kolom_b]);

void copy_to_shared_mem(long long hasil[INI_BARIS_A_WOI][KOLOM_B]);

int main()
{
    srand(time(NULL));
    long long matriks_a[INI_BARIS_A_WOI][KOLOM_A];
    long long matriks_b[BARIS_B][KOLOM_B];
    long long hasil[INI_BARIS_A_WOI][KOLOM_B];

    isi_matriks(INI_BARIS_A_WOI, KOLOM_A, matriks_a, 1, 5);
    isi_matriks(BARIS_B, KOLOM_B, matriks_b, 1, 4);

    printf("M a t r i k s A:\n");
    cetak_matriks(INI_BARIS_A_WOI, KOLOM_A, matriks_a);
    printf("M a t r i k s B:\n");
    cetak_matriks(BARIS_B, KOLOM_B, matriks_b);

    kalikan_matriks(INI_BARIS_A_WOI, KOLOM_A, matriks_a, BARIS_B, KOLOM_B, matriks_b, hasil);

    printf("HAsil_perkAlian_matrIks:\n");
    cetak_matriks(INI_BARIS_A_WOI, KOLOM_B, hasil);

    copy_to_shared_mem(hasil);

    return 0;
}

void isi_matriks(int baris, int kolom, long long matriks[baris][kolom], int min, int max)
{
    int i = 0, j = 0;
    while (i < baris)
    {
        j = 0;
        while (j < kolom)
        {
            matriks[i][j] = rand() % (max - min + 1) + min;
            j++;
        }
        i++;
    }
}

void cetak_matriks(int baris, int kolom, long long matriks[baris][kolom])
{
    int i = 0, j = 0;
    while (i < baris)
    {
        j = 0;
        while (j < kolom)
        {
            printf("%lld ", matriks[i][j]);
            j++;
        }
        printf("\n");
        i++;
    }
}

void kalikan_matriks(int baris_a, int kolom_a, long long matriks_a[baris_a][kolom_a],
                     int baris_b, int kolom_b, long long matriks_b[baris_b][kolom_b],
                     long long hasil[baris_a][kolom_b])
{
    int i = 0, j = 0, k = 0;
    while (i < baris_a)
    {
        j = 0;
        while (j < kolom_b)
        {
            long long temp = 0;
            k = 0;
            while (k < kolom_a)
            {
                temp += matriks_a[i][k] * matriks_b[k][j];
                k++;
            }
            hasil[i][j] = temp;
            j++;
        }
        i++;
    }
}

void copy_to_shared_mem(long long hasil[INI_BARIS_A_WOI][KOLOM_B])
{
    // Buat shared memory untuk hasil perkalian matriks
    int shmid_hasil = shmget(1234, INI_BARIS_A_WOI * KOLOM_B * sizeof(int), IPC_CREAT | 0666);
    long long(*shm_hasil)[KOLOM_B] = shmat(shmid_hasil, NULL, 0);

    // Salin hasil perkalian matriks ke shared memory
    int i = 0, j = 0;
    while (i < INI_BARIS_A_WOI)
    {
        j = 0;
        while (j < KOLOM_B)
        {
            shm_hasil[i][j] = hasil[i][j];
            j++;
        }
        i++;
    }
}
```
cinta.c
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <time.h>

#define BARIS_A 4
#define KOLOM_B 5

long long (*shm_hasil)[KOLOM_B];

void inisialisasi_shared_memory()
{
    int shmid_hasil = shmget(1234, BARIS_A * KOLOM_B * sizeof(int), 0666);
    shm_hasil = shmat(shmid_hasil, NULL, 0);
}

void cetak_hasil_perkalian_matriks()
{
    printf("Hasil perkalian matriks:\n");
    for (int i = 0; i < BARIS_A; i++)
    {
        for (int j = 0; j < KOLOM_B; j++)
        {
            printf("%lld ", shm_hasil[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void *faktorial(void *arg)
{
    unsigned long long *x = (unsigned long long *)arg;
    unsigned long long hasil = 1;
    for (int i = 1; i <= *x; i++)
    {
        hasil *= i;
    }
    *x = hasil;
    return NULL;
}

void cari_faktorial_setiap_elemen_matriks(pthread_t threads[])
{
    int counter = 0;
    for (int i = 0; i < BARIS_A; i++)
    {
        for (int j = 0; j < KOLOM_B; j++)
        {
            unsigned long long *arg = &shm_hasil[i][j];
            pthread_create(&threads[counter++], NULL, faktorial, arg);
        }
    }
}

void tunggu_semua_thread_selesai(pthread_t threads[])
{
    for (int i = 0; i < BARIS_A * KOLOM_B; i++)
    {
        pthread_join(threads[i], NULL);
    }
}

void cetak_hasil_faktorial_setiap_elemen_matriks()
{
    printf("Hasil faktorial matriks:\n");
    for (int i = 0; i < BARIS_A; i++)
    {
        for (int j = 0; j < KOLOM_B; j++)
        {
            printf("%llu ", shm_hasil[i][j]);
        }
        printf("\n");
    }
}

void cetak_waktu_eksekusi(struct timespec start, struct timespec end)
{
    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("Waktu eksekusi: %lf detik\n", time_taken);
}

void hapus_shared_memory(int shmid_hasil)
{
    shmdt(shm_hasil);
    shmctl(shmid_hasil, IPC_RMID, NULL);
}

int main()
{
    struct timespec start, end;
    pthread_t threads[BARIS_A * KOLOM_B];
    int shmid_hasil;
    inisialisasi_shared_memory();

    cetak_hasil_perkalian_matriks();

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    cari_faktorial_setiap_elemen_matriks(threads);

    tunggu_semua_thread_selesai(threads);

    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    cetak_hasil_faktorial_setiap_elemen_matriks();

    cetak_waktu_eksekusi(start, end);

    hapus_shared_memory(shmid_hasil);

    return 0;
}
```
sisop.c
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

#define BARIS_A 4
#define KOLOM_B 5

long long (*shm_hasil)[KOLOM_B];

void inisialisasi_shared_memory()
{
    int shmid_hasil = shmget(1234, BARIS_A * KOLOM_B * sizeof(int), 0666);
    shm_hasil = shmat(shmid_hasil, NULL, 0);
}

void cetak_hasil_perkalian_matriks()
{
    printf("Hasil perkalian matriks:\n");
    for (int i = 0; i < BARIS_A; i++)
    {
        for (int j = 0; j < KOLOM_B; j++)
        {
            printf("%lld ", shm_hasil[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

unsigned long long faktorial(unsigned long long x)
{
    unsigned long long hasil = 1;
    for (int i = 1; i <= x; i++)
    {
        hasil *= i;
    }
    return hasil;
}

void hitung_faktorial_setiap_elemen_matriks()
{
    printf("Menghitung faktorial setiap elemen matriks...\n");

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < BARIS_A; i++)
    {
        for (int j = 0; j < KOLOM_B; j++)
        {
            shm_hasil[i][j] = faktorial(shm_hasil[i][j]);
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    printf("Selesai menghitung faktorial setiap elemen matriks.\n");

    unsigned long long start_nsec = start.tv_sec * 1000000000 + start.tv_nsec;
    unsigned long long end_nsec = end.tv_sec * 1000000000 + end.tv_nsec;
    unsigned long long duration = end_nsec - start_nsec;
    double seconds = (double)duration / 1000000000;
    printf("\nWaktu eksekusi: %.9f detik\n", seconds);
}

void cetak_hasil_faktorial_setiap_elemen_matriks()
{
    printf("\nHasil faktorial matriks:\n");
    for (int i = 0; i < BARIS_A; i++)
    {
        for (int j = 0; j < KOLOM_B; j++)
        {
            printf("%llu ", shm_hasil[i][j]);
        }
        printf("\n");
    }
}

void hapus_shared_memory(int shmid_hasil)
{
    shmdt(shm_hasil);
    shmctl(shmid_hasil, IPC_RMID, NULL);
}

int main()
{
    int shmid_hasil;
    inisialisasi_shared_memory();

    cetak_hasil_perkalian_matriks();

    hitung_faktorial_setiap_elemen_matriks();

    cetak_hasil_faktorial_setiap_elemen_matriks();

    hapus_shared_memory(shmid_hasil);

    return 0;
}
```
## Test output
[![no2.png](https://i.postimg.cc/kMwjrFFV/no2.png)](https://postimg.cc/xqJGMmZY)
[![no3.png](https://i.postimg.cc/bvQTmQww/no3.png)](https://postimg.cc/Lq8LXgPc)
## Kendala
Mungkin pada saat implementasi shared memory
# Soal 3
## Analisa soal:
a. Implementasikan sistem stream menggunakan bahasa pemrograman C dengan message queue untuk komunikasi antara user dan sistem.

b. Sistem stream membaca file song-playlist.json, melakukan decrypt/decode/konversi sesuai metode yang ditentukan, dan menyimpan hasilnya dalam file playlist.txt yang diurutkan secara alfabet.

c. Sistem stream menampilkan daftar lagu yang telah di-decrypt dari file playlist.txt saat menerima perintah LIST dari user.

d. Saat menerima perintah PLAY <SONG> dari user, sistem stream mencocokkan query dengan lagu-lagu dalam playlist.txt dan menampilkan informasi lagu yang cocok. Jika tidak ada yang cocok, sistem memberikan pesan sesuai.

e. Saat menerima perintah ADD <SONG> dari user, sistem stream memeriksa apakah lagu tersebut sudah ada dalam playlist.txt sebelum menambahkannya. Jika sudah ada, sistem memberikan pesan "SONG ALREADY ON PLAYLIST".

f. Gunakan semaphore untuk membatasi akses user terhadap playlist. Jika ada lebih dari dua user yang mencoba mengirim perintah, sistem menampilkan pesan "STREAM SYSTEM OVERLOAD".

g. Jika perintah yang diterima tidak dikenali oleh sistem, sistem menampilkan pesan "UNKNOWN COMMAND".
## Cara pengerjaan soal 3:
Kita membuat fungsi fungsi yang dibutuhkan untuk membuat stream.c
```c
void sort_playlist();
void decrypt_playlist();
void search_playlist(const char *query);
void ini_rot13(char *str);

void ini_decode_base64(const char *str, char *decoded_str);

void ini_hex_decode(const char *str, char *decoded_str);
void add_song(const char *user_id, const char *song);
void handle_list_command();
void handle_add_command(const char *user_id, const char *command);
void handle_decrypt_command();

int can_access_system();

void release_system_access();
```
ini_rot13: Fungsi ini melakukan enkripsi ROT13 pada string yang diberikan. Setiap karakter di-shift sebanyak 13 karakter.
ini_decode_base64: Fungsi ini melakukan dekode base64 pada string yang diberikan dan menghasilkan string hasil dekode.
ini_hex_decode: Fungsi ini melakukan dekode hex pada string yang diberikan dan menghasilkan string hasil dekode.
sort_playlist: Fungsi ini mengurutkan lagu-lagu dalam file playlist.txt secara alfabetis.
decrypt_playlist: Fungsi ini membaca file song-playlist.json, melakukan dekripsi sesuai dengan metode yang ditentukan (ROT13, base64, atau hex), dan menyimpan hasil dekripsi dalam file playlist.txt.
handle_decrypt_command: Fungsi ini dipanggil saat menerima perintah "DECRYPT" dari user. Fungsi ini akan melakukan dekripsi playlist dan mencetak pesan bahwa playlist telah di-dekripsi.
search_playlist: Fungsi ini melakukan pencarian lagu dalam playlist.txt berdasarkan query yang diberikan. Lagu-lagu yang cocok dengan query akan ditampilkan.
add_song: Fungsi ini menambahkan lagu ke dalam playlist.txt jika lagu tersebut belum ada di playlist.
handle_list_command: Fungsi ini dipanggil saat menerima perintah "LIST" dari user. Fungsi ini akan mencetak seluruh lagu yang ada dalam playlist.
handle_add_command: Fungsi ini dipanggil saat menerima perintah "ADD" dari user. Fungsi ini akan memproses perintah tersebut dan memanggil fungsi add_song untuk menambahkan lagu ke dalam playlist.
can_access_system: Fungsi ini memeriksa apakah sistem dapat diakses oleh user. Pada kode yang diberikan, fungsi ini selalu mengembalikan nilai 1, sehingga user selalu dapat mengakses sistem.
release_system_access: Fungsi ini mengurangi jumlah pengguna yang mengakses sistem. Namun, dalam kode yang diberikan, fungsi ini tidak digunakan.

Setelah kita membuat semua yang dibutuh kan dalam `stream.c`, selanjutnya kita membuat `user.c` nya
```c
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
```
Pada user.c kita menerapan message queue untuk mengirim command nya ke dalam stream. setelah itu akan melihat command apa yang di kirim lalu akan me ngeksekusi sesuai if nya. untuk limit user disini kita menggunakan semaphore

## Source code
stream.c
```c
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

#define MSG_SIZEeeeeeeeee 256
#define MAKSIMAL_USERRR 2

int user_count = 0;

typedef struct
{
    long msg_type;
    char msg_text[MSG_SIZEeeeeeeeee];
    char user_id[10]; // Add this line
} message;


void sort_playlist();
void decrypt_playlist();
void search_playlist(const char *query);
void ini_rot13(char *str);

void ini_decode_base64(const char *str, char *decoded_str);

void ini_hex_decode(const char *str, char *decoded_str);
void add_song(const char *user_id, const char *song);
void handle_list_command();
void handle_add_command(const char *user_id, const char *command);
void handle_decrypt_command();

int can_access_system();

void release_system_access();

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
        if (strncmp(msg.msg_text, "EXIT", 4) == 0)
        {
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

void ini_rot13(char *str)
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

void ini_decode_base64(const char *str, char *decoded_str)
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

void ini_hex_decode(const char *str, char *decoded_str)
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

        if (strcmp(method, "ini_rot13") == 0)
        {
            strcpy(decrypted_song, song);
            ini_rot13(decrypted_song);
        }
        else if (strcmp(method, "base64") == 0)
        {
            ini_decode_base64(song, decrypted_song);
        }
        else if (strcmp(method, "hex") == 0)
        {
            ini_hex_decode(song, decrypted_song);
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
    // if (user_count < MAKSIMAL_USERRR)
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
```
user.c
```c
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
```
## Test output
[![NO44444.png](https://i.postimg.cc/nVWyKWYn/NO44444.png)](https://postimg.cc/NL1CBpzn)
[![LISTT.png](https://i.postimg.cc/3xXzTxMw/LISTT.png)](https://postimg.cc/yDNLhB8q)
[![overload.png](https://i.postimg.cc/cJ9SpLYn/overload.png)](https://postimg.cc/gwZQRdgz)
## Kendala
dalam penggunaan semaphore cukup membingungkan tapi alhamdulillah bisa

# Soal 4

# Analisa Soal

Pada soal ini, diminta untuk membuat 3 program dengan keterangan sebagai berikut : 

## unzip.c
Download dan ekstrak isi dari link yang diberikan di soal.
## categorize.c
Setelah menjalankan program untuk unzip, lakukan pemilahan file dengan ketentuan sebagai berikut : 
    
    1. Buatlah sebuah direktori bernama categorize yang berisikan subdirektori untuk menyimpan file dengan jenis 
       extension yang terdapat dalam file extensions.txt. Untuk setiap subdirektori, hanya boleh menyimpan 10 
       file maksimum kecuali untuk subdirektori other yang menyimpan semua file dengan jenis extension yang tidak terdapat didalam file extensions.txt 
    2. Setelah program dirun, tampilkan jumlah file dengan extension yang valid (yang terdapat didalam extensions.txt) dan 
       jumlah file yang terdapat dalam subdirektori other terurut secara ascending
    3. Gunakan multithreading saat melakukan kategorisasi file
    4. Tulis semua pemindahan, pembuatan, ataupun pengaksesan file / direktori kedalam sebuah txt file bernama log.txt 
## logchecker.c  
Ekstrak informasi yang terdapat didalam file log.txt untuk menghitung banyaknya akses untuk tiap file / direktori, menampilkan banyaknya total file tiap extension, dan membuat list seluruh folder yang telah dibuat dan banyaknya file yang dikumpulkan ke folder tersebut, terurut secara ascending.


# Cara Pengerjaan

## unzip
Untuk melakukan unzip, digunakan library curl dan zip.h. Untuk kodenya adalah sebagai berikut : 

```c
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
```

Pertama-tama dilakukan deklarasi dua pid yaitu pid_download dan pid_extract. Kemudian deklarasikan argumen yang akan dijalankan untuk melakukan download dan ekstrak isi dari file. Kemudian akan dilakukan fork terlebih dahulu sebelum menjalankan download, lalu argumen untuk extract akan dijalankan apabila proses download sudah selesai.
## categorize
Pertama-tama deklarasikan absolute path untuk folder files yang sudah didownload dan jenis extension yang valid.  

```c
char *extensions[] = {"jpg", "txt", "js", "py", "png", "emc", "xyz"};
char *files_dir = "/home/reyhanqb/Documents/Shift 3/soal4/files";
```
Lalu dibuat fungsi untuk menulis log :
```c
 FILE *logchecker = fopen("log.txt", "a");

    if (logchecker != NULL)
    {
        time(&current_time);
        timeline = localtime(&current_time);

        strftime(buff, sizeof(buff), "%d-%m-%Y %H:%M:%S", timeline);
        fprintf(logchecker, "%s %s\n", buff, message);
        fclose(logchecker);
    }
```
Pertama-tama deklarasikan file log dengan nama logchecker, lalu buka file menggunakan fopen() append. 

Kemudian, dibuat fungsi untuk melakukan kategorisasi file. Pertama-tama, fungsi akan mendeklarasikan variabel yang akan digunakan. Disini, dideklarasi variabel logs untuk menulis log didalam fungsi createLog()  

```c
 DIR *dir;
 struct dirent *ent;
 char current_dir[256];
 strcpy(current_dir, dir_name);
 char logs[512];
```
Kemudian, fungsi akan mencatat setiap pengaksesan file / direktori untuk ditulis kedalam file log.txt. Lalu dibuat kondisi yang bersifat rekursif untuk melakukan pengecekan jenis file dengan strchr, dan akan ditentukan apakah file yang diakses memiliki extension yang valid (ada didalam log.txt) atau tidak. Jenis extension dari file akan dilowercase kemudian dibandingkan dengan array extensions menggunakan strcmp untuk menentukan apakah file memiliki extension yang valid atau tidak. Apabila file tidak memiliki jenis extension yang valid, maka fungsi akan berhenti
```c
 snprintf(logs, sizeof(logs), "ACCESSED %s", current_dir);
    createLog(logs);

    if ((dir = opendir(current_dir)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (ent->d_type == DT_REG)
            {
                char *validExtension = strrchr(ent->d_name, '.');
                int validFiles = 0;
                char valid_ext[256] = "other";

                if (validExtension)
                {
                    validExtension++;
                    strcpy(valid_ext, validExtension);

                    for (char *p = valid_ext; *p; ++p)
                    {
                        *p = tolower(*p);
                    }

                    for (int i = 0; i < ext_size; i++)
                    {
                        if (strcmp(valid_ext, extensions[i]) == 0)
                        {
                            validFiles = 1;
                            break;
                        }
                    }
                }
```
Selanjutnya, apabila file tidak memiliki extension yang valid, maka file akan dipindahkan ke subdirektori other.
```c
if (!validFiles)
                {
                    snprintf(dest_folder, sizeof(dest_folder), "categorized/other");

                    if (dest_folder == NULL)
                    {
                        mkdir(dest_folder, 0700);
                    }

                    snprintf(src, sizeof(src), "%s/%s", current_dir, ent->d_name);
                    snprintf(dest, sizeof(dest), "%s/%s", dest_folder, ent->d_name);

                    rename(src, dest);
                    snprintf(logs, sizeof(logs), "MOVED %s file : %s > %s", valid_ext, src, dest);
                    createLog(logs);
                }
                else
                {
                    snprintf(destination, sizeof(destination), "categorized/%s", valid_ext);

                    struct stat st;
                    if (stat(destination, &st) == -1)
                    {
                        mkdir(destination, 0700);
                        snprintf(logs, sizeof(logs), "MADE %s", destination);
                        createLog(logs);
                    }

                    int dirs = 1;
                    strcpy(dest_folder, destination);
```
Kemudian, akan dijalankan sebuah loop untuk memastikan bahwa setiap subdirektori kecuali subdirektori other hanya memiliki maksimum 10 file saja. Hal ini dilakukan dengan mendeklarasikan variabel count++, yang dimana apabila jumlahnya kurang dari maksimum, akan berhenti untuk kemudian membuat direktori baru. Setelah pembuatan direktori baru, dipanggil fungsi createLog() untuk menulis log kedalam file log.txt.
```c
                    while (1)
                    {
                        DIR *dest_dir = opendir(dest_folder);
                        int count = 0;

                        if (dest_dir)
                        {
                            struct dirent *file_ent;
                            while ((file_ent = readdir(dest_dir)) != NULL)
                            {
                                if (file_ent->d_type == DT_REG)
                                {
                                    count++;
                                }
                            }
                            closedir(dest_dir);
                        }

                        if (count < maximum)
                        {
                            break;
                        }

                        dirs++;
                        snprintf(dest_folder, sizeof(dest_folder), "%s (%d)", destination, dirs);
                        if (stat(dest_folder, &st) == -1)
                        {
                            mkdir(dest_folder, 0700);
                            snprintf(logs, sizeof(logs), "MADE %s", dest_folder);
                            createLog(logs);
                        }
                    }

                    snprintf(src, sizeof(src), "%s/%s", current_dir, ent->d_name);
                    snprintf(dest, sizeof(dest), "%s/%s", dest_folder, ent->d_name);

                    rename(src, dest);
                    snprintf(logs, sizeof(logs), "MOVED %s file : %s > %s", valid_ext, src, dest);
                    createLog(logs);
                }
            }

```
Apabila kedua kondisi diatas tidak terpenuhi, maka fungsi akan lanjut mengakses direktori yang ada, mengingat beberapa direktori didalam folder files punya banyak subdirektori. Proses ini akan dilakukan secara multithreading 
```c
 else if (ent->d_type == DT_DIR && ent->d_name[0] != '.')
            {
                
                snprintf(subdir, sizeof(subdir), "%s/%s", current_dir, ent->d_name);

                pthread_t thread;
                pthread_create(&thread, NULL, categorizeFiles, (void *)subdir);
                pthread_join(thread, NULL);
            }
        }
        closedir(dir);
    }

```
Selanjutnya dibuat fungsi untuk menghitung jumlah file yang terdapat didalam folder other. Didalam fungsi ini terdapat kondisional yang berjalan selama direktori masih memiliki file, maka variabel found akan diset menjadi 1 dan variabel count ditambahkan. loop akan terus berlanjut sampai tidak ada lagi file yang ditemukan, lalu fungsi akan mereturn nilai count.
```c
 int countOtherFiles(const char *current_dir_path)
{
    int count = 0;
    int dirs = 1;
    
    strcpy(directory_path, current_dir_path);

    while (true)
    {
        int found = 0;
        DIR *dir;
        struct dirent *ent;

        if ((dir = opendir(directory_path)) != NULL)
        {
            while ((ent = readdir(dir)) != NULL)
            {
                if (ent->d_type == DT_REG)
                {
                    count++;
                    found = 1;
                }
            }
            closedir(dir);
        }

        if (!found)
        {
            break;
        }

        dirs++;
        snprintf(directory_path, sizeof(directory_path), "%s (%d)", current_dir_path, dirs);
    }

    return count;
}
```
Selanjutnya adalah fungsi untuk mencatat jumlah file untuk tiap extension. Cara kerja fungsi yaitu dengan membuat file temp.txt yang akan diisi dengan nama subdirektori dan file yang ditemukan dalam direktori categorized. Fungsi akan mencari karakter "." pada tiap subdirektori kemudian mencocokkannya dengan jenis extension yang valid, lalu print line untuk setiap subdirektori yang ditemukan, kemudian menghitung jumlah file untuk tiap extension satu persatu. Apabila sudah dilakukan perhitungan jumlah file, file temp.txt akan dihapus.
```c
void getAmountOfFiles(){
       // Membuat file temp.txt untuk menyimpan output 'ls'
    system("ls -R categorized > temp.txt");

    // Membuka file sementara
    FILE *temp_file = fopen("temp.txt", "r");
    char line[256];

    int png_count = 0;
    int jpg_count = 0;
    int emc_count = 0;
    int xyz_count = 0;
    int js_count = 0;
    int py_count = 0;
    int txt_count = 0;
    int other_count = 0;

    // Menghitung jumlah file setiap ekstensi dalam urutan ascending
    while (fgets(line, sizeof(line), temp_file))
    {
        
        line[strcspn(line, "\n")] = 0;

        // Memeriksa apakah baris merupakan path direktori
        if (line[strlen(line) - 1] == ':')
        {
            continue;
        }
        else
        {
            // Memeriksa apakah baris merupakan nama file atau direktori
            char *file_name = strrchr(line, '/');
            if (file_name != NULL)
            {
                file_name++;
            }
            else
            {
                file_name = line;
            }

            // Memeriksa apakah file memiliki ekstensi yang valid
            int is_valid = 0;
            int exist = 0;
            for (int i = 0; i < ext_size; i++)
            {
                if (strstr(file_name, extensions[i]) != NULL)
                {
                    exist++;
                    is_valid = 1;
                    break;
                }
            }

            // Menghitung jumlah file dengan jenis ekstensi yg valid
            if (is_valid)
            {
                if (strstr(file_name, ".png") != NULL)
                {
                    png_count++;
                }
                else if (strstr(file_name, ".jpg") != NULL)
                {
                    jpg_count++;
                }
                else if (strstr(file_name, ".emc") != NULL)
                {
                    emc_count++;
                }
                else if (strstr(file_name, ".xyz") != NULL)
                {
                    xyz_count++;
                }
                else if (strstr(file_name, ".js") != NULL)
                {
                    js_count++;
                }
                else if (strstr(file_name, ".py") != NULL)
                {
                    py_count++;
                }
                else if (strstr(file_name, ".txt") != NULL)
                {
                    txt_count++;
                }
            }      
        }
    }

    // Menampilkan hasil penghitungan
    printf("Jumlah file txt: %d\n", txt_count);
    printf("Jumlah file emc: %d\n", emc_count);
    printf("Jumlah file jpg: %d\n", jpg_count);
    printf("Jumlah file png: %d\n", png_count);
    printf("Jumlah file js: %d\n", js_count);
    printf("Jumlah file xyz: %d\n", xyz_count);
    printf("Jumlah file py: %d\n", py_count);

    // Menutup file sementara
    fclose(temp_file);

    // Menghapus file sementara
    remove("temp.txt");
}

```
Terakhir di fungsi main(), akan dibuat direktori categorized dan subdirektori other dan dicatat ke log.txt. 
Apabila kategorisasi sudah selesai, 
program akan print jumlah file tiap extension yang valid dan jumlah file dalam subdirektori other
```c
int main()
{

    pthread_t tid;

    mkdir("categorized", 0700);
    createLog("MADE categorized");

    mkdir("categorized/other", 0700);
    createLog("MADE categorized/other");

    pthread_create(&tid, NULL, categorizeFiles, files_dir);
    pthread_join(tid, NULL);

    printf("Jumlah file : ");
    getAmountOfFiles();    

    int others = countOtherFiles("categorized/other");
    printf("Jumlah isi subdirektori other: %d\n", others);

    return 0;
}
```

## logchecker
Pada logchecker, digunakan dua fungsi untuk menghitung jumlah file untuk tiap extension yang valid dan jumlah isi dari direktori categorized. Fungsi yang digunakan sama dengan fungsi yang ada di kode categorize.c. Berikut ini adalah fungsi yang digunakan untuk menghitung isi subdirektori other 

```c
int countTotalFiles(const char *current_dir_path)
{
    int count = 0;
    int dirs = 1;
    
    strcpy(directory_path, current_dir_path);

    while (true)
    {
        int found = 0;
        DIR *dir;
        struct dirent *ent;

        if ((dir = opendir(directory_path)) != NULL)
        {
            while ((ent = readdir(dir)) != NULL)
            {
                if (ent->d_type == DT_REG)
                {
                    count++;
                    found = 1;
                }
            }
            closedir(dir);
        }

        if (!found)
        {
            break;
        }

        dirs++;
        snprintf(directory_path, sizeof(directory_path), "%s (%d)", current_dir_path, dirs);
    }

    return count;
}

```
Kemudian, untuk menghitung jumlah file didalam tiap subdirektori juga digunakan fungsi yang sama dengan yang ada di categorize.c, namun ditambahkan variabel total untuk menghitung total file didalam direktori categorized.
```c
void getAmountOfFiles(){
       // Membuat file temp.txt untuk menyimpan output 'ls'
    system("ls -R categorized > temp.txt");

    // Membuka file sementara
    FILE *temp_file = fopen("temp.txt", "r");
    char line[256];

    int png_count = 0;
    int jpg_count = 0;
    int emc_count = 0;
    int xyz_count = 0;
    int js_count = 0;
    int py_count = 0;
    int txt_count = 0;
    int other_count = 0;

    // Menghitung jumlah file setiap ekstensi dalam urutan ascending
    while (fgets(line, sizeof(line), temp_file))
    {
        
        line[strcspn(line, "\n")] = 0;

        // Memeriksa apakah baris merupakan path direktori
        if (line[strlen(line) - 1] == ':')
        {
            continue;
        }
        else
        {
            // Memeriksa apakah baris merupakan nama file atau direktori
            char *file_name = strrchr(line, '/');
            if (file_name != NULL)
            {
                file_name++;
            }
            else
            {
                file_name = line;
            }

            // Memeriksa apakah file memiliki ekstensi yang valid
            int is_valid = 0;
            int exist = 0;
            for (int i = 0; i < ext_size; i++)
            {
                if (strstr(file_name, extensions[i]) != NULL)
                {
                    exist++;
                    is_valid = 1;
                    break;
                }
            }

            // Menghitung jumlah file dengan jenis ekstensi yg valid
            if (is_valid)
            {
                if (strstr(file_name, ".png") != NULL)
                {
                    png_count++;
                }
                else if (strstr(file_name, ".jpg") != NULL)
                {
                    jpg_count++;
                }
                else if (strstr(file_name, ".emc") != NULL)
                {
                    emc_count++;
                }
                else if (strstr(file_name, ".xyz") != NULL)
                {
                    xyz_count++;
                }
                else if (strstr(file_name, ".js") != NULL)
                {
                    js_count++;
                }
                else if (strstr(file_name, ".py") != NULL)
                {
                    py_count++;
                }
                else if (strstr(file_name, ".txt") != NULL)
                {
                    txt_count++;
                }
            }      
        }
    }

    // Menampilkan hasil penghitungan
    printf("Jumlah file txt: %d\n", txt_count);
    printf("Jumlah file emc: %d\n", emc_count);
    printf("Jumlah file jpg: %d\n", jpg_count);
    printf("Jumlah file png: %d\n", png_count);
    printf("Jumlah file js: %d\n", js_count);
    printf("Jumlah file xyz: %d\n", xyz_count);
    printf("Jumlah file py: %d\n", py_count);

    int others = countTotalFiles("categorized/other");

    int total = others + txt_count + emc_count + jpg_count + png_count + js_count + xyz_count + py_count;


    printf("Jumlah file dalam direktori categorize : %d\n", total);

    // Menutup file sementara
    fclose(temp_file);

    // Menghapus file sementara
    remove("temp.txt");
}
```
Terakhir di dalam fungsi main(), kode akan membuka file log.txt dan mendeklarasikan variabel untuk menghitung jumlah akses, pemindahan, dan pembuatan tiap subdirektori ataupun files. Kode akan mencocokkan tiap line dengan kata ACCESSED, MADE, dan MOVED, lalu menambahkan jumlah count times_accessed, times_made, times_moved untuk menampilkan jumlah akses, pemindahan, ataupun pembuatan subdirektori / files di terminal.
```c
int main() {
    char *log_file_name = "log.txt";
    char *dir_path = "./categorized";

    // Ekstrak isi file log.txt
    FILE *log_file = fopen(log_file_name, "r");
    if (log_file == NULL) {
        printf("Gagal membuka file %s\n", log_file_name);
        return 1;
    }

    int times_accessed = 0;
    int times_made = 0;
    int times_moved = 0;
    while (fgets(line, sizeof(line), log_file)) {
        if (strstr(line, "ACCESSED") != NULL) {
            times_accessed++;
        }
        else if(strstr(line, "MADE") != NULL){
            times_made++;
        }
        else if(strstr(line, "MOVED") != NULL){
            times_moved++;
        }
    }
    fclose(log_file);

    // Hitung jumlah file di direktori
    DIR *dir = opendir(dir_path);
    if (dir == NULL) {
        printf("Gagal membuka direktori %s\n", dir_path);
        return 1;
    }
    
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            file_count++;
        }
    }
    closedir(dir);

    printf("Jumlah ACCESSED pada direktori: %d\n", times_accessed);
    printf("Jumlah MOVED pada direktori: %d\n", times_moved);
    printf("Jumlah CREATED pada direktori: %d\n", times_made);
    printf("Jumlah file untuk tiap extension : \n");
    getAmountOfFiles();

    return 0;
}
```
# Source Code
## unzip.c
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
```
## categorize.c
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>

#define MAX_EXT_COUNT 100
#define MAX_EXT_LEN 10

char *extensions[] = {"jpg", "txt", "js", "py", "png", "emc", "xyz"};
char *files_dir = "/home/reyhanqb/Documents/Shift 3/soal4/files";
char *dir_name;

int ext_size = sizeof(extensions) / sizeof(extensions[0]);
const int maximum = 10;

time_t current_time;
struct tm *timeline;

char buff[80], valid_dir[256];
char src[256], dest[256];
char subdir[256], destination[256], directory_path[256], dest_folder[256];

char ext_list[MAX_EXT_COUNT][MAX_EXT_LEN]; // list ekstensi file yang akan diproses

void createLog();

void *categorizeFiles(void *dir_name)
{
    DIR *dir;
    struct dirent *ent;
    char current_dir[256];
    strcpy(current_dir, dir_name);
    char logs[512];

    snprintf(logs, sizeof(logs), "ACCESSED %s", current_dir);
    createLog(logs);

    if ((dir = opendir(current_dir)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (ent->d_type == DT_REG)
            {
                char *validExtension = strrchr(ent->d_name, '.');
                int validFiles = 0;
                char valid_ext[256] = "other";

                if (validExtension)
                {
                    validExtension++;
                    strcpy(valid_ext, validExtension);

                    for (char *p = valid_ext; *p; ++p)
                    {
                        *p = tolower(*p);
                    }

                    for (int i = 0; i < ext_size; i++)
                    {
                        if (strcmp(valid_ext, extensions[i]) == 0)
                        {
                            validFiles = 1;
                            break;
                        }
                    }
                }

                if (!validFiles)
                {
                    snprintf(dest_folder, sizeof(dest_folder), "categorized/other");

                    if (dest_folder == NULL)
                    {
                        mkdir(dest_folder, 0700);
                    }

                    snprintf(src, sizeof(src), "%s/%s", current_dir, ent->d_name);
                    snprintf(dest, sizeof(dest), "%s/%s", dest_folder, ent->d_name);

                    rename(src, dest);
                    snprintf(logs, sizeof(logs), "MOVED %s file : %s > %s", valid_ext, src, dest);
                    createLog(logs);
                }
                else
                {
                    snprintf(destination, sizeof(destination), "categorized/%s", valid_ext);

                    struct stat st;
                    if (stat(destination, &st) == -1)
                    {
                        mkdir(destination, 0700);
                        snprintf(logs, sizeof(logs), "MADE %s", destination);
                        createLog(logs);
                    }

                    int dirs = 1;
                    strcpy(dest_folder, destination);

                    while (1)
                    {
                        DIR *dest_dir = opendir(dest_folder);
                        int count = 0;

                        if (dest_dir)
                        {
                            struct dirent *file_ent;
                            while ((file_ent = readdir(dest_dir)) != NULL)
                            {
                                if (file_ent->d_type == DT_REG)
                                {
                                    count++;
                                }
                            }
                            closedir(dest_dir);
                        }

                        if (count < maximum)
                        {
                            break;
                        }

                        dirs++;
                        snprintf(dest_folder, sizeof(dest_folder), "%s (%d)", destination, dirs);
                        if (stat(dest_folder, &st) == -1)
                        {
                            mkdir(dest_folder, 0700);
                            snprintf(logs, sizeof(logs), "MADE %s", dest_folder);
                            createLog(logs);
                        }
                    }

                    snprintf(src, sizeof(src), "%s/%s", current_dir, ent->d_name);
                    snprintf(dest, sizeof(dest), "%s/%s", dest_folder, ent->d_name);

                    rename(src, dest);
                    snprintf(logs, sizeof(logs), "MOVED %s file : %s > %s", valid_ext, src, dest);
                    createLog(logs);
                }
            }
            else if (ent->d_type == DT_DIR && ent->d_name[0] != '.')
            {
                
                snprintf(subdir, sizeof(subdir), "%s/%s", current_dir, ent->d_name);

                pthread_t thread;
                pthread_create(&thread, NULL, categorizeFiles, (void *)subdir);
                pthread_join(thread, NULL);
            }
        }
        closedir(dir);
    }

    return NULL;
}

int countOtherFiles(const char *current_dir_path)
{
    int count = 0;
    int dirs = 1;
    
    strcpy(directory_path, current_dir_path);

    while (true)
    {
        int found = 0;
        DIR *dir;
        struct dirent *ent;

        if ((dir = opendir(directory_path)) != NULL)
        {
            while ((ent = readdir(dir)) != NULL)
            {
                if (ent->d_type == DT_REG)
                {
                    count++;
                    found = 1;
                }
            }
            closedir(dir);
        }

        if (!found)
        {
            break;
        }

        dirs++;
        snprintf(directory_path, sizeof(directory_path), "%s (%d)", current_dir_path, dirs);
    }

    return count;
}

void getAmountOfFiles(){
       // Membuat file temp.txt untuk menyimpan output 'ls'
    system("ls -R categorized > temp.txt");

    // Membuka file sementara
    FILE *temp_file = fopen("temp.txt", "r");
    char line[256];

    int png_count = 0;
    int jpg_count = 0;
    int emc_count = 0;
    int xyz_count = 0;
    int js_count = 0;
    int py_count = 0;
    int txt_count = 0;
    int other_count = 0;

    // Menghitung jumlah file setiap ekstensi dalam urutan ascending
    while (fgets(line, sizeof(line), temp_file))
    {
        
        line[strcspn(line, "\n")] = 0;

        // Memeriksa apakah baris merupakan path direktori
        if (line[strlen(line) - 1] == ':')
        {
            continue;
        }
        else
        {
            // Memeriksa apakah baris merupakan nama file atau direktori
            char *file_name = strrchr(line, '/');
            if (file_name != NULL)
            {
                file_name++;
            }
            else
            {
                file_name = line;
            }

            // Memeriksa apakah file memiliki ekstensi yang valid
            int is_valid = 0;
            int exist = 0;
            for (int i = 0; i < ext_size; i++)
            {
                if (strstr(file_name, extensions[i]) != NULL)
                {
                    exist++;
                    is_valid = 1;
                    break;
                }
            }

            // Menghitung jumlah file dengan jenis ekstensi yg valid
            if (is_valid)
            {
                if (strstr(file_name, ".png") != NULL)
                {
                    png_count++;
                }
                else if (strstr(file_name, ".jpg") != NULL)
                {
                    jpg_count++;
                }
                else if (strstr(file_name, ".emc") != NULL)
                {
                    emc_count++;
                }
                else if (strstr(file_name, ".xyz") != NULL)
                {
                    xyz_count++;
                }
                else if (strstr(file_name, ".js") != NULL)
                {
                    js_count++;
                }
                else if (strstr(file_name, ".py") != NULL)
                {
                    py_count++;
                }
                else if (strstr(file_name, ".txt") != NULL)
                {
                    txt_count++;
                }
            }      
        }
    }

    // Menampilkan hasil penghitungan
    printf("Jumlah file txt: %d\n", txt_count);
    printf("Jumlah file emc: %d\n", emc_count);
    printf("Jumlah file jpg: %d\n", jpg_count);
    printf("Jumlah file png: %d\n", png_count);
    printf("Jumlah file js: %d\n", js_count);
    printf("Jumlah file xyz: %d\n", xyz_count);
    printf("Jumlah file py: %d\n", py_count);

    // Menutup file sementara
    fclose(temp_file);

    // Menghapus file sementara
    // remove("temp.txt");
}

void createLog(const char *message)
{
    FILE *logchecker = fopen("log.txt", "a");

    if (logchecker != NULL)
    {
        time(&current_time);
        timeline = localtime(&current_time);

        strftime(buff, sizeof(buff), "%d-%m-%Y %H:%M:%S", timeline);
        fprintf(logchecker, "%s %s\n", buff, message);
        fclose(logchecker);
    }
}


int main()
{

    pthread_t tid;

    mkdir("categorized", 0700);
    createLog("MADE categorized");

    mkdir("categorized/other", 0700);
    createLog("MADE categorized/other");

    pthread_create(&tid, NULL, categorizeFiles, files_dir);
    pthread_join(tid, NULL);

    printf("Jumlah file : ");
    getAmountOfFiles();    

    int others = countOtherFiles("categorized/other");
    printf("Jumlah isi subdirektori other: %d\n", others);

    return 0;
}

```
## logchecker.c
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <stddef.h>

char line[1000], directory_path[1000];
int file_count = 0;
struct dirent *entry;

char *extensions[] = {"jpg", "txt", "js", "py", "png", "emc", "xyz"};
int ext_size = sizeof(extensions) / sizeof(extensions[0]);

int countTotalFiles(const char *current_dir_path)
{
    int count = 0;
    int dirs = 1;
    
    strcpy(directory_path, current_dir_path);

    while (true)
    {
        int found = 0;
        DIR *dir;
        struct dirent *ent;

        if ((dir = opendir(directory_path)) != NULL)
        {
            while ((ent = readdir(dir)) != NULL)
            {
                if (ent->d_type == DT_REG)
                {
                    count++;
                    found = 1;
                }
            }
            closedir(dir);
        }

        if (!found)
        {
            break;
        }

        dirs++;
        snprintf(directory_path, sizeof(directory_path), "%s (%d)", current_dir_path, dirs);
    }

    return count;
}

void getAmountOfFiles(){
       // Membuat file temp.txt untuk menyimpan output 'ls'
    system("ls -R categorized > temp.txt");

    // Membuka file sementara
    FILE *temp_file = fopen("temp.txt", "r");
    char line[256];

    int png_count = 0;
    int jpg_count = 0;
    int emc_count = 0;
    int xyz_count = 0;
    int js_count = 0;
    int py_count = 0;
    int txt_count = 0;
    int other_count = 0;

    // Menghitung jumlah file setiap ekstensi dalam urutan ascending
    while (fgets(line, sizeof(line), temp_file))
    {
        
        line[strcspn(line, "\n")] = 0;

        // Memeriksa apakah baris merupakan path direktori
        if (line[strlen(line) - 1] == ':')
        {
            continue;
        }
        else
        {
            // Memeriksa apakah baris merupakan nama file atau direktori
            char *file_name = strrchr(line, '/');
            if (file_name != NULL)
            {
                file_name++;
            }
            else
            {
                file_name = line;
            }

            // Memeriksa apakah file memiliki ekstensi yang valid
            int is_valid = 0;
            int exist = 0;
            for (int i = 0; i < ext_size; i++)
            {
                if (strstr(file_name, extensions[i]) != NULL)
                {
                    exist++;
                    is_valid = 1;
                    break;
                }
            }

            // Menghitung jumlah file dengan jenis ekstensi yg valid
            if (is_valid)
            {
                if (strstr(file_name, ".png") != NULL)
                {
                    png_count++;
                }
                else if (strstr(file_name, ".jpg") != NULL)
                {
                    jpg_count++;
                }
                else if (strstr(file_name, ".emc") != NULL)
                {
                    emc_count++;
                }
                else if (strstr(file_name, ".xyz") != NULL)
                {
                    xyz_count++;
                }
                else if (strstr(file_name, ".js") != NULL)
                {
                    js_count++;
                }
                else if (strstr(file_name, ".py") != NULL)
                {
                    py_count++;
                }
                else if (strstr(file_name, ".txt") != NULL)
                {
                    txt_count++;
                }
            }      
        }
    }

    // Menampilkan hasil penghitungan
    printf("Jumlah file txt: %d\n", txt_count);
    printf("Jumlah file emc: %d\n", emc_count);
    printf("Jumlah file jpg: %d\n", jpg_count);
    printf("Jumlah file png: %d\n", png_count);
    printf("Jumlah file js: %d\n", js_count);
    printf("Jumlah file xyz: %d\n", xyz_count);
    printf("Jumlah file py: %d\n", py_count);

    int others = countTotalFiles("categorized/other");

    int total = others + txt_count + emc_count + jpg_count + png_count + js_count + xyz_count + py_count;


    printf("Jumlah file dalam direktori categorize : %d\n", total);

    // Menutup file sementara
    fclose(temp_file);

    // Menghapus file sementara
    remove("temp.txt");
}


int main() {
    char *log_file_name = "log.txt";
    char *dir_path = "./categorized";

    // Ekstrak isi file log.txt
    FILE *log_file = fopen(log_file_name, "r");
    if (log_file == NULL) {
        printf("Gagal membuka file %s\n", log_file_name);
        return 1;
    }

    int times_accessed = 0;
    int times_made = 0;
    int times_moved = 0;
    while (fgets(line, sizeof(line), log_file)) {
        if (strstr(line, "ACCESSED") != NULL) {
            times_accessed++;
        }
        else if(strstr(line, "MADE") != NULL){
            times_made++;
        }
        else if(strstr(line, "MOVED") != NULL){
            times_moved++;
        }
    }
    fclose(log_file);

    // Hitung jumlah file di direktori
    DIR *dir = opendir(dir_path);
    if (dir == NULL) {
        printf("Gagal membuka direktori %s\n", dir_path);
        return 1;
    }
    
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            file_count++;
        }
    }
    closedir(dir);

    printf("Jumlah ACCESSED pada direktori: %d\n", times_accessed);
    printf("Jumlah MOVED pada direktori: %d\n", times_moved);
    printf("Jumlah CREATED pada direktori: %d\n", times_made);
    printf("Jumlah file untuk tiap extension : \n");
    getAmountOfFiles();



    return 0;
}
```
# Test Output

## unzip
![unzip_run](https://github.com/reyhanqb/sisop-latihan/assets/107137535/0e512e53-fea5-4c04-82d7-54d4c9e760a5)
## categorize
![run_Categorize](https://github.com/reyhanqb/sisop-latihan/assets/107137535/059dbe59-b599-41bb-a066-f46fd5f91df7)
![Screenshot (101)](https://github.com/reyhanqb/sisop-latihan/assets/107137535/c88abecf-0581-4880-89b4-74f4107b4838)
![Screenshot (102)](https://github.com/reyhanqb/sisop-latihan/assets/107137535/78fa4962-6211-4293-a38d-9997e642ed8f)
## logchecker
![Screenshot 2023-05-13 133129](https://github.com/reyhanqb/sisop-latihan/assets/107137535/c467e868-90cc-4864-bb84-2fc4dfbe58fa)
# Kendala
Terdapat beberapa kendala saat melakukan kategorisasi files, karena banyak subdirektori dari folder files yang tidak menyimpan file sama sekali namun punya banyak subdirektori beruntun, contohnya seperti subdirektori count files. Selain itu beberapa kali fungsi countTotalFiles tidak memberikan output yang sesuai


