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
