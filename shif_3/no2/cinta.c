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