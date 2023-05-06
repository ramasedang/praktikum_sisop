
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h> 
#include <ctype.h>

#define MAX_CHAR 256 // jumlah karakter ASCII yang berbeda

// Node untuk binary tree Huffman
struct Node {
    char symbol;  // simbol (karakter) yang direpresentasikan oleh node
    int freq;     // frekuensi kemunculan simbol dalam file
    struct Node *left, *right; // pointer ke child node (left dan right)
};

// fungsi untuk membuat node baru pada binary tree Huffman
struct Node* newNode(char symbol, int freq) {
    struct Node* node = (struct Node*) malloc(sizeof(struct Node));
    node->symbol = symbol;
    node->freq = freq;
    node->left = NULL;
    node->right = NULL;
    return node;
}

// fungsi untuk membandingkan frekuensi kemunculan dua node Huffman
int compareNodes(const void* a, const void* b) {
    struct Node* nodeA = *(struct Node**)a;
    struct Node* nodeB = *(struct Node**)b;
    return nodeA->freq - nodeB->freq;
}

// fungsi untuk membangun binary tree Huffman
struct Node* buildHuffmanTree(char* data, int* freq) {
    // inisialisasi daftar node Huffman untuk setiap simbol yang muncul pada data
    struct Node* nodeList[MAX_CHAR] = { NULL };
    int n = 0;  // jumlah node yang telah dibuat
    for (int i = 0; i < strlen(data); i++) {
        char symbol = toupper(data[i]); // ubah huruf kecil menjadi huruf kapital
        // abaikan karakter selain huruf
        if (symbol < 'A' || symbol > 'Z') {
            continue;
        }
        // periksa apakah simbol ini telah ditemukan sebelumnya
        int j = 0;
        for (j = 0; j < n; j++) {
            if (nodeList[j]->symbol == symbol) {
                nodeList[j]->freq++;
                break;
            }
        }
        // jika belum ditemukan sebelumnya, tambahkan sebagai node baru
        if (j == n) {
            nodeList[j] = newNode(symbol, freq[symbol]);
            n++;
        }
    }
    // urutkan daftar node Huffman berdasarkan frekuensi kemunculan simbol
    qsort(nodeList, n, sizeof(struct Node*), compareNodes);

    // membangun binary tree Huffman
    while (n > 1) {
        struct Node* nodeA = nodeList[0];
        struct Node* nodeB = nodeList[1];
        struct Node* parent = newNode('$', nodeA->freq + nodeB->freq);
        parent->left = nodeA;
        parent->right = nodeB;
        // hapus node A dan B dari daftar
        for (int i = 2; i < n; i++) {
            nodeList[i-2] = nodeList[i];
        }
        nodeList[n-2] = parent;
        n--;
        // urutkan kembali daftar node Huffman
        qsort(nodeList, n, sizeof(struct Node*), compareNodes);
    }
    // sisa node pada daftar adalah root node dari binary tree Huffman
    return nodeList[0];
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s [filename]\n", argv[0]);
        return 1;
    }

    char* filename = argv[1];
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        printf("Error: Failed to open file %s\n", filename);
        return 1;
    }

    // hitung frekuensi kemunculan simbol pada file dan jumlah bit
    int freq[MAX_CHAR] = { 0 };
    char symbol;
    int totalBits = 0;

    // pipes initialization
    int fd1[2]; // Used to store two ends of first pipe 
    int fd2[2]; // Used to store two ends of second pipe 

    if (pipe(fd1)==-1) 
    { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    } 
    if (pipe(fd2)==-1) 
    { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    } 

pid_t p; 

// fork process
p = fork(); 

if (p < 0) 
{ 
    fprintf(stderr, "fork Failed" ); 
    return 1; 
} 

// Parent process 
else if (p > 0) 
{ 
    close(fd1[0]); // Close reading end of first pipe 

    // Write data from file to child process using first pipe 
    while (read(fd, &symbol, 1) > 0) {
        symbol = toupper(symbol); // ubah huruf kecil menjadi huruf kapital

        if (symbol >= 'A' && symbol <= 'Z') {
            freq[(int)symbol]++;
            totalBits += 8; // tambahkan 8 bit untuk setiap simbol yang ditemukan
        }

        write(fd1[1], &symbol, 1); 
    }
    close(fd1[1]); 

    // Wait for child to send totalBits
    wait(NULL); 

    printf("Total bits in file %s: %d\n", filename, totalBits);

    printf("Frequency of each letter in file %s:\n", filename);
    for (int i = 0; i < MAX_CHAR;i++) {
        if (freq[i] > 0) {
            printf("%c: %d\n", i, freq[i]);
        }
    }

    close(fd);
    return 0;
} 

// child process 
else
{ 
    close(fd1[1]); // Close writing end of first pipe 

    // Read a string using first pipe 
    char symbol;
    while (read(fd1[0], &symbol, 1) > 0) {}

    close(fd1[0]); 
    close(fd2[0]); // Close reading end of second pipe 

    // Write totalBits and close writing end 
    write(fd2[1], &totalBits, sizeof(totalBits)); 
    close(fd2[1]); 

    exit(0); 
} 
}