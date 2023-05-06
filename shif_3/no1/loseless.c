
// Include necessary header files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <ctype.h>

#define MAX_CHAR 256 // number of different ASCII characters

// Node for Huffman binary tree
struct HfNode
{
char symbol; // symbol (character) represented by the node
int freq; // frequency of symbol occurrence in the file
struct HfNode *left, *right; // pointers to child nodes (left and right)
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
struct HfNode *nodeA = *(struct HfNode **)a;
struct HfNode *nodeB = *(struct HfNode **)b;
return nodeA->freq - nodeB->freq;
}

// Function to build the Huffman binary tree
struct HfNode *build_hftree(char *data, int *freq)
{
// Initialize the list of Huffman nodes for each symbol that appears in the data
struct HfNode *node_list[MAX_CHAR] = {NULL};
int n = 0; // number of nodes created
int i = 0;
while (i < strlen(data))
{
char symbol = toupper(data[i]); // convert lowercase letters to uppercase
// Ignore non-letter characters
if (symbol < 'A' || symbol > 'Z')
{
i++;
continue;
}
// Check if this symbol has been found before
int j = 0;
while (j < n)
{
if (node_list[j]->symbol == symbol)
{
node_list[j]->freq++;
break;
}
j++;
}
// If it hasn't been found before, add it as a new node
if (j == n)
{
node_list[j] = new_hfnode(symbol, freq[symbol]);
n++;
}
i++;
}
// Sort the list of Huffman nodes by symbol frequency
qsort(node_list, n, sizeof(struct HfNode *), compare_hfnodes);

// Build the Huffman binary tree
while (n > 1)
{
    struct HfNode *nodeA = node_list[0];
    struct HfNode *nodeB = node_list[1];
    struct HfNode *parent = new_hfnode('$', nodeA->freq + nodeB->freq);
    parent->left = nodeA;
    parent->right = nodeB;
    // Remove node A and B from the list
    int i = 2;
    while (i < n)
    {
        node_list[i - 2] = node_list[i];
        i++;
    }
    node_list[n - 2] = parent;
    n--;
    // Sort the Huffman node list again
    qsort(node_list, n, sizeof(struct HfNode *), compare_hfnodes);
}
// The remaining node in the list is the root node of the Huffman binary tree
return node_list[0];
}

// Function to calculate the number of bits needed to compress the data using the Huffman binary tree
int calc_compressed_bits(struct HfNode *root, int depth)
{
if (!root)
return 0;
if (!root->left && !root->right)
return root->freq * depth;
return calc_compressed_bits(root->left, depth + 1) + calc_compressed_bits(root->right, depth + 1);
}

// Function to find the Huffman code for each symbol in the binary tree and compress the data
void find_hfcode(struct HfNode *root, char *code, int code_len, char *input, char *compressed)
{
if (root->left == NULL && root->right == NULL)
{
code[code_len] = '\0';
printf("%c: %s\n", root->symbol, code);
int i = 0;
while (i < strlen(input))
{
if (toupper(input[i]) == root->symbol)
{
strcat(compressed, code);
}
i++;
}
return;
}
code[code_len] = '0';
find_hfcode(root->left, code, code_len + 1, input, compressed);
code[code_len] = '1';
find_hfcode(root->right, code, code_len + 1, input, compressed);
}

int main(int argc, char *argv[])
{
// Check if the correct number of arguments are provided
if (argc != 2)
{
printf("Usage: %s [filename]\n", argv[0]);
return 1;
}

char *filename = argv[1];
int fd = open(filename, O_RDONLY);
if (fd == -1)
{
printf("Error: Failed to open file %s\n", filename);
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
while (read(fd, &symbol, 1) > 0)
{
    symbol = toupper(symbol); // Convert lowercase letters to uppercase

    if (symbol >= 'A' && symbol <= 'Z')
    {
        freq[(int)symbol]++;
        total_bits += 8; // Add 8 bits for each symbol found
    }
}

printf("Frequency of each letter in file %s:\n", filename);
int i = 0;
while (i < MAX_CHAR)
{
    if (freq[i] > 0)
    {
        printf("%c: %d\n", i, freq[i]);
    }
    i++;
}
printf("\n");
close(fd);

// Send symbol frequency to child process
write(fd1[1], freq, sizeof(freq));
close(fd1[1]);

// Wait for the child process to send compressed bits
int compressed_bits;
read(fd2[0], &compressed_bits, sizeof(compressed_bits));
close(fd2[0]);

printf("\nTotal bits in file %s: %d\n", filename, total_bits);
printf("Total bits after compression: %d\n", compressed_bits);
float compression_ratio = (float)compressed_bits / total_bits;
printf("Compression ratio: %.2f\n", compression_ratio);

return 0;
}

// Child process
else
{
close(fd1[1]); // Close writing end of first pipe
close(fd2[0]); // Close reading end of second pipe

// Read symbol frequency from parent process
int freq[MAX_CHAR] = {0};
read(fd1[0], freq, sizeof(freq));
close(fd1[0]);

char input[MAX_CHAR * 100] = "";
int i = 0;
while (i < MAX_CHAR)
{
    int j = 0;
    while (j < freq[i])
    {
        strncat(input, (char *)&i, 1);
        j++;
    }
    i++;
}

struct HfNode *root = build_hftree(input, freq);
char code[MAX_CHAR] = "";
char compressed[MAX_CHAR * 100] = "";
find_hfcode(root, code, 0, input, compressed);

int compressed_bits = strlen(compressed);
printf("Total bits after compression: %d\n", compressed_bits);
float compression_ratio = (float)compressed_bits / calc_compressed_bits(root, 0);
printf("Compression ratio: %.2f\n", compression_ratio);

// Send compression result back to parent process
write(fd2[1], &compressed_bits, sizeof(compressed_bits));
close(fd2[1]);

exit(0);
}
}