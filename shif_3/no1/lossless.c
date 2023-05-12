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

