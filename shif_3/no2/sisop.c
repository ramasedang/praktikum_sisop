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
