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