# sisop-praktikum-modul-2-2023-mhfg-it14

## Laporan Resmi Modul 2 Praktikum Sistem Operasi 2023
---
### Kelompok ITA14:
Aloysius Bataona Manullang - 5027211008  
Athaya Reyhan Nugroho - 5027211067    
Moh. Sulthan Arief Rahmatullah - 5027211045


---
## Soal 1
## Analisa soal:
Soal no.1 menggambarkan seorang penjaga hewan di kebun binatang yang diberi tugas untuk melakukan penjagaan pada beberapa hewan yang ada di kebun binatang. Sebelumnya, Grape-kun harus mengunduh folder gambar hewan yang harus dijaga dari drive kebun binatang dan melakukan unzip untuk melihat file gambar. Grape-kun kemudian membuat direktori dengan 
nama HewanDarat, HewanAmphibi, dan HewanAir untuk memilah file gambar tersebut, dan melakukan pemindahan file gambar sesuai dengan tempat tinggalnya. Akhirnya, Grape-kun melakukan zip pada direktori yang dia buat sebelumnya untuk menghemat penyimpanan. Tidak boleh menggunakan sistem untuk melakukan zip dan unzip.

## Cara pengerjaan soal 1:
Pertama untuk mendownload disini menggunakan system yaitu memanggil curl untuk mendownload gambar
```c
// URL file yang akan diunduh
	char url[] = "https://drive.google.com/uc?export=download&id=1oDgj5kSiDO0tlyS7-20uz7t20X3atwrq";

	// Nama file yang akan disimpan
	char outfilename[] = "animal.zip";

	// Buat perintah sistem untuk mengunduh file
	char command[1000];
	sprintf(command, "curl -L -o %s \"%s\"", outfilename, url);

	// Jalankan perintah sistem untuk mengunduh file
	system(command);
```

Selanjutnya melakukan unzip dengan fungsi ```int unzip(const char *zipPath, const char *desPath); ``` .
Fungsi unzip digunakan untuk mengekstrak isi file zip. Fungsi ini menerima dua parameter yaitu zipPath yang berisi path ke file zip yang akan diekstrak, dan desPath yang berisi path direktori tujuan hasil ekstraksi. 
```c
int unzip(const char *zipPath, const char *desPath)
{
    int err;
    struct zip *hZip = zip_open(zipPath, 0, &err);
    if (hZip)
    {
        size_t totalIndex = zip_get_num_entries(hZip, 0);
        for (size_t i = 0; i < totalIndex; i++)
        {
            struct zip_stat st;
            zip_stat_init(&st);
            zip_stat_index(hZip, i, 0, &st);

            struct zip_file *zf = zip_fopen_index(hZip, i, 0);
            if (!zf)
            {
                zip_close(hZip);
                return 0;
            }

            char *buffer = (char *)malloc(st.size);
            if (!buffer)
            {
                zip_fclose(zf);
                zip_close(hZip);
                return 0;
            }

            zip_fread(zf, buffer, st.size);
            zip_fclose(zf);

            // Menyusun path tujuan file
            char destFile[1024];
            snprintf(destFile, sizeof(destFile), "%s/%s", desPath, st.name);

            // Membuka file tujuan untuk ditulisi
            FILE *f = fopen(destFile, "wb");
            if (!f)
            {
                free(buffer);
                zip_close(hZip);
                return 0;
            }

            // Menuliskan buffer ke file tujuan
            fwrite(buffer, sizeof(char), st.size, f);
            fclose(f);
            free(buffer);
        }

        zip_close(hZip);
    }

    return 1;
}
```
Fungsi ini menggunakan library zip.h untuk membuka file zip dan mengekstrak isi file zip. Fungsi ini menerima dua parameter yaitu zipPath yang berisi path ke file zip yang akan diekstrak, dan desPath yang berisi path direktori tujuan hasil ekstraksi.

Pertama, fungsi membuka file zip menggunakan zip_open. Jika file zip berhasil dibuka, fungsi menghitung jumlah file yang ada di dalam file zip menggunakan zip_get_num_entries. Kemudian, fungsi melakukan loop untuk setiap file yang ada di dalam file zip menggunakan zip_stat_index.

Selanjutnya, fungsi membuka file di dalam file zip menggunakan zip_fopen_index, dan membaca isi file menggunakan zip_fread. Setelah itu, fungsi menyusun path tujuan file menggunakan snprintf, dan membuka file tujuan untuk ditulisi menggunakan fopen.

Fungsi menuliskan isi file yang telah dibaca dari file zip ke file tujuan menggunakan fwrite, dan menutup file tujuan menggunakan fclose. Terakhir, fungsi mengembalikan nilai 1 jika ekstraksi berhasil, dan 0 jika gagal.

Selanjutkan melakukan pemilihan secara random terhadap file gambar ``` char *select_random_image(void);```
Fungsi select_random_image digunakan untuk memilih sebuah file gambar secara acak dari direktori kerja. Berikut adalah potongan kode untuk fungsi tersebut:

```c
char *select_random_image(void)
{
    // Variabel untuk menyimpan nama file
    static char filename[MAX_FILENAME_LENGTH];

    // Buka direktori
    DIR *dir;
    struct dirent *ent;
    dir = opendir(".");
    if (dir != NULL)
    {
        // Variabel untuk menyimpan nama file
        char filenames[MAX_FILES][MAX_FILENAME_LENGTH];
        int n = 0;

        // Baca setiap file dalam direktori
        while ((ent = readdir(dir)) != NULL && n < MAX_FILES)
        {
            // Hanya tambahkan file dengan ekstensi .jpg
            if (strstr(ent->d_name, ".jpg") != NULL)
            {
                strcpy(filenames[n], ent->d_name);
                n++;
            }
        }

        closedir(dir);

        // Inisialisasi generator bilangan acak
        srand(time(NULL));

        // Pilih file gambar secara acak
        int index = rand() % n;
        const char *selected_filename = filenames[index];

        // Salin nama file yang dipilih ke variabel yang akan di-return
       

```

Fungsi ini menggunakan library dirent.h untuk membaca direktori kerja dan time.h untuk menginisialisasi generator bilangan acak. Fungsi ini tidak menerima parameter dan mengembalikan pointer ke nama file yang dipilih secara acak.

Pertama, fungsi membuka direktori kerja menggunakan opendir. Jika berhasil membuka direktori, fungsi membaca setiap file yang ada di dalam direktori menggunakan readdir. Fungsi hanya menambahkan file yang memiliki ekstensi .jpg ke dalam array filenames.

Selanjutnya, fungsi menginisialisasi generator bilangan acak menggunakan srand dengan nilai seed dari waktu saat ini. Fungsi kemudian memilih file gambar secara acak menggunakan rand. Fungsi menyalin nama file yang dipilih ke variabel filename menggunakan strncpy.

Terakhir, fungsi mengembalikan pointer ke variabel filename. Jika gagal membuka direktori, fungsi menampilkan pesan error menggunakan perror dan mengembalikan nilai NULL.



Fungsi klasifikasi_hewan digunakan untuk memindahkan file gambar hewan ke direktori yang sesuai berdasarkan nama file. Berikut adalah potongan kode untuk fungsi tersebut:

```c
void klasifikasi_hewan(void)
{
    // Buat direktori untuk memilah file gambar hewan
    mkdir("HewanDarat", 0777);
    mkdir("HewanAmphibi", 0777);
    mkdir("HewanAir", 0777);

    // Memindahkan file gambar hewan ke direktori yang sesuai
    DIR *dir;
    struct dirent *entry;
    char path[MAX_PATH];
    dir = opendir(".");
    if (dir == NULL)
    {
        printf("Error: Tidak bisa membuka direktori\n");
        return;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG && strstr(entry->d_name, ".jpg") != NULL)
        {
            if (strstr(entry->d_name, "_darat.jpg") != NULL)
            {
                snprintf(path, sizeof(path), "HewanDarat/%s", entry->d_name);
                rename(entry->d_name, path);
            }
            else if (strstr(entry->d_name, "_amphibi.jpg") != NULL)
            {
                snprintf(path, sizeof(path), "HewanAmphibi/%s", entry->d_name);
                rename(entry->d_name, path);
            }
            else if (strstr(entry->d_name, "_air.jpg") != NULL)
            {
                snprintf(path, sizeof(path), "HewanAir/%s", entry->d_name);
                rename(entry->d_name, path);
            }
        }
    }

    closedir(dir);
}

```

Fungsi ini menggunakan library dirent.h dan sys/stat.h. Fungsi ini tidak menerima parameter dan tidak mengembalikan nilai.

Pertama, fungsi membuat tiga direktori baru yaitu HewanDarat, HewanAmphibi, dan HewanAir menggunakan mkdir. Jika direktori sudah ada, fungsi akan melewatkannya dan tidak membuat direktori baru.

Selanjutnya, fungsi membaca setiap file yang ada di dalam direktori menggunakan readdir. Fungsi hanya memproses file yang memiliki ekstensi .jpg dan memindahkannya ke direktori yang sesuai dengan nama file. Misalnya, jika nama file mengandung _darat.jpg, fungsi akan memindahkannya ke direktori HewanDarat.

Fungsi memindahkan file menggunakan rename dengan mengubah path file tujuan menggunakan snprintf. Terakhir, fungsi menutup direktori menggunakan closedir.

Fungsi zip_dir digunakan untuk membuat file zip dari sebuah direktori dan memasukkan setiap file yang ada di dalamnya ke dalam file zip. Berikut adalah potongan kode untuk fungsi tersebut:

```c
int zip_dir(const char *dirPath, const char *zipPath)
{
    // Buka direktori yang akan di-zip
    DIR *dir = opendir(dirPath);
    if (!dir)
    {
        return 0;
    }

    // Membuka file zip untuk ditulisi
    struct zip *hZip = zip_open(zipPath, ZIP_CREATE, NULL);
    if (!hZip)
    {
        closedir(dir);
        return 0;
    }

    // Looping untuk membaca isi direktori dan men-zip setiap file
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        // Skip current and parent directory
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        // Susun path file yang akan di-zip
        char filePath[1024];
        snprintf(filePath, sizeof(filePath), "%s/%s", dirPath, entry->d_name);

        // Buka file yang akan di-zip
        FILE *f = fopen(filePath, "rb");
        if (!f)
        {
            continue;
        }

        // Baca isi file dan simpan ke buffer
        fseek(f, 0, SEEK_END);
        int size = ftell(f);
        rewind(f);
        char *buffer = (char *)malloc(size);
        fread(buffer, sizeof(char), size, f);
        fclose(f);

        // Masukkan file ke dalam zip
        struct zip_source *source = zip_source_buffer(hZip, buffer, size, 0);
        zip_file_add(hZip, entry->d_name, source, ZIP_FL_OVERWRITE);

        // Bebaskan buffer
        free(buffer);
    }

    // Tutup zip file dan direktori
    zip_close(hZip);
    closedir(dir);

    return 1;
}

 ```

Fungsi ini menggunakan library dirent.h, stdio.h, zip.h, dan sys/stat.h. Fungsi ini menerima dua parameter yaitu dirPath yang merupakan path ke direktori yang akan di-zip, dan zipPath yang merupakan path untuk file zip yang akan dibuat. Fungsi mengembalikan nilai 1 jika berhasil membuat file zip dan 0 jika gagal.

Pertama, fungsi membuka direktori yang akan di-zip menggunakan opendir. Jika gagal membuka direktori, fungsi mengembalikan nilai 0. Selanjutnya, fungsi membuka file zip untuk ditulisi menggunakan zip_open dengan flag ZIP_CREATE. Jika gagal membuka file zip, fungsi menutup direktori dan mengembalikan nilai 0.

Selanjutnya, fungsi membaca setiap file yang ada di dalam direktori menggunakan readdir. Fungsi melewatkan file yang merupakan current directory (.) atau parent directory (..). Fungsi membuka setiap file yang ada di dalam direktori menggunakan fopen dengan mode rb. Jika gagal membuka file, fungsi melewatinya dan melanjutkan ke file selanjutnya.

Selanjutnya, fungsi membaca isi file menggunakan fseek, ftell, dan fread. Fungsi menyimpan isi file ke dalam buffer dan menutup file menggunakan
## Source code
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zip.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#define MAX_FILES 100			// maksimum jumlah file
#define MAX_FILENAME_LENGTH 255 // maksimum panjang nama file
#define MAX_PATH 1024

// referensi https://stackoverflow.com/questions/48182274/how-to-unzip-a-zipped-file-zip-using-c-in-visual-studio

int unzip(const char *zipPath, const char *desPath);

int zip_dir(const char *dirPath, const char *zipPath);

char *select_random_image(void);

void klasifikasi_hewan(void);

int main(void)
{
	// URL file yang akan diunduh
	char url[] = "https://drive.google.com/uc?export=download&id=1oDgj5kSiDO0tlyS7-20uz7t20X3atwrq";

	// Nama file yang akan disimpan
	char outfilename[] = "animal.zip";

	// Buat perintah sistem untuk mengunduh file
	char command[1000];
	sprintf(command, "curl -L -o %s \"%s\"", outfilename, url);

	// Jalankan perintah sistem untuk mengunduh file
	system(command);

	// Ekstrak isi file zip
	if (unzip(outfilename, ".") == 0)
	{
		printf("Gagal mengekstrak file zip\n");
		return 1;
	}

	// Pilih gambar secara acak
	char *filename = select_random_image();
	if (filename != NULL)
	{
		printf("Gambar yang dipilih: %s\n", filename);
	}

	klasifikasi_hewan();

	// membuat file zip dari direktori HewanDarat
	zip_dir("HewanDarat", "HewanDarat.zip");

	// membuat file zip dari direktori HewanAmphibi

	zip_dir("HewanAmphibi", "HewanAmphibi.zip");

	// membuat file zip dari direktori HewanAir

	zip_dir("HewanAir", "HewanAir.zip");

	// menghapus direktori HewanDarat, HewanAmphibi, HewanAir

	system("rm -r -f HewanDarat");
	system("rm -r -f HewanAmphibi");
	system("rm -r -f HewanAir");

	return 0;
}

int unzip(const char *zipPath, const char *desPath)
{
	int err;
	struct zip *hZip = zip_open(zipPath, 0, &err);
	if (hZip)
	{
		size_t totalIndex = zip_get_num_entries(hZip, 0);
		for (size_t i = 0; i < totalIndex; i++)
		{
			struct zip_stat st;
			zip_stat_init(&st);
			zip_stat_index(hZip, i, 0, &st);

			struct zip_file *zf = zip_fopen_index(hZip, i, 0);
			if (!zf)
			{
				zip_close(hZip);
				return 0;
			}

			char *buffer = (char *)malloc(st.size);
			if (!buffer)
			{
				zip_fclose(zf);
				zip_close(hZip);
				return 0;
			}

			zip_fread(zf, buffer, st.size);
			zip_fclose(zf);

			// Menyusun path tujuan file
			char destFile[1024];
			snprintf(destFile, sizeof(destFile), "%s/%s", desPath, st.name);

			// Membuka file tujuan untuk ditulisi
			FILE *f = fopen(destFile, "wb");
			if (!f)
			{
				free(buffer);
				zip_close(hZip);
				return 0;
			}

			// Menuliskan buffer ke file tujuan
			fwrite(buffer, sizeof(char), st.size, f);
			fclose(f);
			free(buffer);
		}

		zip_close(hZip);
	}

	return 1;
}

int zip_dir(const char *dirPath, const char *zipPath)
{
	// Buka direktori yang akan di-zip
	DIR *dir = opendir(dirPath);
	if (!dir)
	{
		return 0;
	}

	// Membuka file zip untuk ditulisi
	struct zip *hZip = zip_open(zipPath, ZIP_CREATE, NULL);
	if (!hZip)
	{
		closedir(dir);
		return 0;
	}

	// Looping untuk membaca isi direktori dan men-zip setiap file
	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL)
	{
		// Skip current and parent directory
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
		{
			continue;
		}

		// Susun path file yang akan di-zip
		char filePath[1024];
		snprintf(filePath, sizeof(filePath), "%s/%s", dirPath, entry->d_name);

		// Buka file yang akan di-zip
		FILE *f = fopen(filePath, "rb");
		if (!f)
		{
			continue;
		}

		// Baca isi file dan simpan ke buffer
		fseek(f, 0, SEEK_END);
		int size = ftell(f);
		rewind(f);
		char *buffer = (char *)malloc(size);
		fread(buffer, sizeof(char), size, f);
		fclose(f);

		// Masukkan file ke dalam zip
		struct zip_source *source = zip_source_buffer(hZip, buffer, size, 0);
		zip_file_add(hZip, entry->d_name, source, ZIP_FL_OVERWRITE);

		// Bebaskan buffer
		free(buffer);
	}

	// Tutup zip file dan direktori
	zip_close(hZip);
	closedir(dir);

	return 1;
}

char *select_random_image(void)
{
	// Variabel untuk menyimpan nama file
	static char filename[MAX_FILENAME_LENGTH];

	// Buka direktori
	DIR *dir;
	struct dirent *ent;
	dir = opendir(".");
	if (dir != NULL)
	{
		// Variabel untuk menyimpan nama file
		char filenames[MAX_FILES][MAX_FILENAME_LENGTH];
		int n = 0;

		// Baca setiap file dalam direktori
		while ((ent = readdir(dir)) != NULL && n < MAX_FILES)
		{
			// Hanya tambahkan file dengan ekstensi .jpg
			if (strstr(ent->d_name, ".jpg") != NULL)
			{
				strcpy(filenames[n], ent->d_name);
				n++;
			}
		}

		closedir(dir);

		// Inisialisasi generator bilangan acak
		srand(time(NULL));

		// Pilih file gambar secara acak
		int index = rand() % n;
		const char *selected_filename = filenames[index];

		// Salin nama file yang dipilih ke variabel yang akan di-return
		strncpy(filename, selected_filename, MAX_FILENAME_LENGTH);

		return filename;
	}
	else
	{
		// Gagal membuka direktori
		perror("");
		return NULL;
	}
}

void klasifikasi_hewan(void)
{
	// Buat direktori untuk memilah file gambar hewan
	mkdir("HewanDarat", 0777);
	mkdir("HewanAmphibi", 0777);
	mkdir("HewanAir", 0777);

	// Memindahkan file gambar hewan ke direktori yang sesuai
	DIR *dir;
	struct dirent *entry;
	char path[MAX_PATH];
	dir = opendir(".");
	if (dir == NULL)
	{
		printf("Error: Tidak bisa membuka direktori\n");
		return;
	}

	while ((entry = readdir(dir)) != NULL)
	{
		if (entry->d_type == DT_REG && strstr(entry->d_name, ".jpg") != NULL)
		{
			if (strstr(entry->d_name, "_darat.jpg") != NULL)
			{
				snprintf(path, sizeof(path), "HewanDarat/%s", entry->d_name);
				rename(entry->d_name, path);
			}
			else if (strstr(entry->d_name, "_amphibi.jpg") != NULL)[![tree.png](https://i.postimg.cc/JhS7gpHq/tree.png)](https://postimg.cc/wtcdRcVy)
			{
				snprintf(path, sizeof(path), "HewanAmphibi/%s", entry->d_name);
				rename(entry->d_name, path);
			}
			else if (strstr(entry->d_name, "_air.jpg") != NULL)
			{
				snprintf(path, sizeof(path), "HewanAir/%s", entry->d_name);
				rename(entry->d_name, path);
			}
		}
	}

	closedir(dir);
}

```
## Test output
[![no1.png](https://i.postimg.cc/y8XNDm56/no1.png)](https://postimg.cc/K1jbVMxC)

[![tree.png](https://i.postimg.cc/JhS7gpHq/tree.png)](https://postimg.cc/wtcdRcVy)

## Kendala
Kendala saat melakukan zip dan unzip karena tidak boleh menggunakan system, dan juga bahasa C yang jarang digunakan untuk hal seperti ini, jadi harus riset lebih lanjut

# Soal 2

## Analisis Soal

Berdasarkan soal kita diminta untuk membuat folder secara otomatis dengan nama timestamp setiap 30 detik, lalu endownload 15 gambar dari https://picsum.photos/ untuk setiap folder yang dibuat dengan 5 detik setiap foto diberi nama pada setiap gambar yang diunduh dengan format timestamp [YYYY-mm-dd_HH:mm:ss]. Tiap gambar berbentuk persegi dengan ukuran (t%1000)+50 piksel dimana t adalah detik Epoch Unix. Setelah setiap folder terisi dengan 15 gambar, folder akan di-zip dan folder aslinya akan dihapus.Lalu, membuat program "killer" yang dapat digunakan untuk menghentikan program utama dan menghapus dirinya sendiri. Memungkinkan program utama dijalankan dalam dua mode yaitu MODE_A dan MODE_B dengan argumen -a atau -b. Dalam MODE_A, program utama akan langsung berhenti ketika program killer dijalankan. Dalam MODE_B, program utama akan berhenti ketika program killer dijalankan, tetapi membiarkan proses di setiap folder dan zip yang masih berjalan sampai selesai.

## Cara pengerjaan soal 2 :

Pertama, perlu dilakukan fungsi - fungsi untuk pengerjaan soal.
```c 
//fungsi mendownload gambar sesuai perintah pada soal
void download_gambar(const char* nama_folder, int thread_num) {
    time_t t = time(NULL);
    char nama_gambar[50];
    sprintf(nama_gambar, "%04d-%02d-%02d_%02d:%02d:%02d_%d.jpg",
        1900 + localtime(&t)->tm_year, localtime(&t)->tm_mon + 1, localtime(&t)->tm_mday,
        localtime(&t)->tm_hour, localtime(&t)->tm_min, localtime(&t)->tm_sec, thread_num);
    char url[50];
    sprintf(url, "https://picsum.photos/%d", (int) (t%1000)+50);
    char path[100];
    sprintf(path, "%s/%s", nama_folder, nama_gambar);

    pid_t pid = fork();
    if (pid == 0) {
        execl("/usr/bin/wget", "wget", "-q", "-O", path, url, NULL);
        exit(EXIT_SUCCESS);
    }
}
``` 
Fungsi di atas merupakan fungsi mendownload gambar, kita perlu library ```time``` sehingga mengambil localtime untuk format nama dan mendapatkan Epoch Unix. ```thread_num``` yang digunakan untuk mengidentifikasi thread yang sedang menjalankan fungsi ini.  Fungsi ini akan melakukan ``fork()`` untuk membuat sebuah proses baru yang akan menjalankan perintah wget menggunakan `execl`. Perintah wget ini akan mendownload gambar dari URL yang telah dibuat sebelumnya dan menyimpannya ke dalam path yang telah dibuat. Setelah perintah wget selesai dieksekusi, proses baru akan keluar menggunakan ``exit()``. Sementara itu, proses utama akan kembali ke kode yang sedang dieksekusi.

```c
//fungsi melakukan zip pada folder
void zip_folder(const char* nama_folder) {
    char nama_zip[100];
    sprintf(nama_zip, "%s.zip", nama_folder);

    pid_t pid = fork();
    if (pid == 0) {
        execl("/usr/bin/zip", "zip", "-rmq", nama_zip, nama_folder, NULL);
        exit(EXIT_SUCCESS);
    }
}

//fungsi menghapus folder
void delete_folder(const char* nama_folder) {
    sleep(5);
    pid_t pid = fork();
    if (pid == 0) {
        execl("/bin/rm", "rm", "-rf", nama_folder, NULL);
        exit(EXIT_SUCCESS);
    }
}
```
Fungsi zip_folder digunakan untuk mengompresi sebuah folder menjadi file zip dengan format nama <nama_folder>.zip. Selanjutnya, fungsi ini akan melakukan fork untuk membuat proses baru. Proses baru tersebut akan menjalankan perintah ```zip -rmq <nama_zip> <nama_folder>``` dengan menggunakan fungsi ```execl```. 

Fungsi delete_folder digunakan untuk menghapus sebuah folder beserta isinya. Fungsi ini menerima parameter berupa nama_folder yang merupakan nama folder yang akan dihapus. Pertama-tama, fungsi ini akan menunda proses selama 5 detik menggunakan fungsi sleep. Hal ini dilakukan untuk memastikan bahwa proses zip sebelumnya telah selesai sebelum folder dihapus.Selanjutnya, fungsi ini akan melakukan fork untuk membuat proses baru. Argumen -rf pada perintah rm berarti melakukan operasi remove secara rekursif dan menghapus folder beserta seluruh isiannya. Setelah menjalankan perintah rm, proses tersebut akan keluar dengan menggunakan perintah exit.

```c
//fungsi membuat file killer sesuai dengan Modenya
void create_killer(char* nama_program) {
    FILE* fp;
    fp = fopen("killer", "w");
    fprintf(fp, "#!/bin/bash\n");

    //mengecek mode yang digunakan
    if (mode == MODE_A) {
        fprintf(fp, "killall -SIGKILL %s\n", nama_program);
    } else {
        fprintf(fp, "kill -SIGTERM %d\n", getpid());
    }

    //menghapus file killer setelah digunakan
    fprintf(fp, "rm killer\n");
    fclose(fp);
    chmod("killer", 0700);
}

//Handler untuk sinyal SIGTERM
void sigterm_handler(int sig) {
    is_running = false;
}

```
Fungsi create_killer digunakan untuk membuat file killer yang akan digunakan untuk menterminasi program.  Jika mode yang digunakan adalah MODE_A, maka perintah killall -SIGKILL %s\n akan men-kill semua proses dengan nama program yang sama. Jika mode yang digunakan adalah MODE_B, maka perintah kill -SIGTERM akan mengirim sinyal SIGTERM ke program sehingga program akan berhenti tetapi akan membiarkan proses yang masih berjalan sampai selesai.Setelah itu, fungsi ini menuliskan perintah ```rm killer\n```` ke dalam file killer, yang akan menghapus file killer setelah file tersebut digunakan.Terakhir, fungsi ini menutup file killer dan mengubah permission file killer menjadi 0755 sehingga file menjadi executable.

Fungsi sigterm_handler adalah sebuah handler (penangan) yang akan dipanggil ketika program menerima sinyal SIGTERM. Pada contoh kode yang diberikan, ketika handler ini dipanggil, maka variabel is_running akan diubah nilainya menjadi false. Hal ini berguna untuk memberikan sinyal pada program agar berhenti secara normal.
```c
int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s [-a | -b]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    //mengecek mode yang yang digunakan
    if (strcmp(argv[1], "-a") == 0) {
        mode = MODE_A;
    } else if (strcmp(argv[1], "-b") == 0) {
        mode = MODE_B;
    } else {
        printf("Invalid argument\n");
        exit(EXIT_FAILURE);
    }

    signal(SIGTERM, sigterm_handler);

    // Variabel untuk menyimpan PID dan SID
    pid_t pid, sid;

    // Menyimpan PID dari Child Process
    pid = fork();

    /* Keluar saat fork gagal
    * (nilai variabel pid < 0) */
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }

    /* Keluar saat fork berhasil
    * (nilai variabel pid adalah PID dari child process) */
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    // mengubah file mode mask
    umask(0);

    // create new session
    sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }

    // change working directory
    if (chdir(".") < 0) {
        exit(EXIT_FAILURE);
    }

    // close standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    create_killer(argv[0]);

    // start the main loop
    while (is_running) {
        time_t t = time(NULL);
        char nama_folder[50];
        sprintf(nama_folder, "%04d-%02d-%02d_%02d:%02d:%02d", 
            1900 + localtime(&t)->tm_year, localtime(&t)->tm_mon + 1, localtime(&t)->tm_mday,
            localtime(&t)->tm_hour, localtime(&t)->tm_min, localtime(&t)->tm_sec);

        pid_t child_pid = fork();

        if (child_pid == 0) {
            // membuat folder
            mkdir(nama_folder, 0777);

            // download gambar
            for (int i = 0; i < 15; i++) {
                download_gambar(nama_folder, i + 1);
                sleep(5);
            }

            // zip folder
            zip_folder(nama_folder);

            // menghapus folder
            delete_folder(nama_folder);

            exit(EXIT_SUCCESS);
        } else {
            sleep(30);
        }
    }
    exit(EXIT_SUCCESS);
}

```
Pertama - tama, program akan meminta untuk mengecek argument untuk pilihan mode A atau B. Untuk menjalankan program ini sebagai daemon, program akan membuat child process dengan menggunakan fork(). Child process kemudian akan melakukan beberapa hal seperti membuat folder dengan format nama yang sesuai, mengunduh gambar-gambar dari internet, mengompres folder tersebut menjadi file zip, dan menghapus folder tersebut. Setelah itu, child process akan selesai dan program utama akan sleep selama 30 detik sebelum membuat child process baru dan mengulang kembali proses di atas. Sehingga, folder akan dibuat setiap 30 detik. File gambar juga akan di download setiap 5 detik. Untuk menghentikan program, dipanngil fungsi ```create_killer```

Program utama juga menggunakan signal handler untuk menangani sinyal SIGTERM yang akan dikirimkan ke program saat dihentikan. Ketika sinyal SIGTERM diterima, variabel is_running akan diubah menjadi false dan program akan berhenti secara bertahap sesuai dengan sleep time pada loop utama.

## Source Code
```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>
#include <signal.h>
#include <stdbool.h>

#define MODE_A 1
#define MODE_B 2

bool is_running = true;
int mode;

//Handler untuk sinyal SIGTERM
void sigterm_handler(int sig);
//fungsi menghapus folder
void delete_folder(const char* nama_folder);
//fungsi melakukan zip pada folder
void zip_folder(const char* nama_folder);
//fungsi mendownload gambar sesuai perintah pada soal
void download_gambar(const char* nama_folder, int thread_num);
//fungsi membuat file killer sesuai dengan Modenya
void create_killer(char* nama_program);

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s [-a | -b]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    //mengecek mode yang yang digunakan
    if (strcmp(argv[1], "-a") == 0) {
        mode = MODE_A;
    } else if (strcmp(argv[1], "-b") == 0) {
        mode = MODE_B;
    } else {
        printf("Invalid argument\n");
        exit(EXIT_FAILURE);
    }

    signal(SIGTERM, sigterm_handler);

    // Variabel untuk menyimpan PID dan SID
    pid_t pid, sid;

    // Menyimpan PID dari Child Process
    pid = fork();

    /* Keluar saat fork gagal
    * (nilai variabel pid < 0) */
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }

    /* Keluar saat fork berhasil
    * (nilai variabel pid adalah PID dari child process) */
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    // mengubah file mode mask
    umask(0);

    // membuat new session
    sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }

    // mengubah working directory
    if (chdir(".") < 0) {
        exit(EXIT_FAILURE);
    }

    // menutup standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    //file killer
    create_killer(argv[0]);

    // memulai main loop
    while (is_running) {
        time_t t = time(NULL);
        char nama_folder[50];
        //format folder
        sprintf(nama_folder, "%04d-%02d-%02d_%02d:%02d:%02d", 
            1900 + localtime(&t)->tm_year, localtime(&t)->tm_mon + 1, localtime(&t)->tm_mday,
            localtime(&t)->tm_hour, localtime(&t)->tm_min, localtime(&t)->tm_sec);

        pid_t child_pid = fork();

        if (child_pid == 0) {
            // membuat folder
            mkdir(nama_folder, 0777);

            // download gambar
            for (int i = 0; i < 15; i++) {
                download_gambar(nama_folder, i + 1);
                sleep(5);
            }

            // zip folder
            zip_folder(nama_folder);

            // menghapus folder
            delete_folder(nama_folder);

            exit(EXIT_SUCCESS);
        } else {
            sleep(30);
        }
    }
    exit(EXIT_SUCCESS);
}


//Handler untuk sinyal SIGTERM
void sigterm_handler(int sig) {
    is_running = false;
}


//fungsi menghapus folder
void delete_folder(const char* nama_folder) {
    sleep(5);
    pid_t pid = fork();
    if (pid == 0) {
        execl("/bin/rm", "rm", "-rf", nama_folder, NULL);
        exit(EXIT_SUCCESS);
    }
}

//fungsi melakukan zip pada folder
void zip_folder(const char* nama_folder) {
    char nama_zip[100];
    sprintf(nama_zip, "%s.zip", nama_folder);

    pid_t pid = fork();
    if (pid == 0) {
        execl("/usr/bin/zip", "zip", "-rmq", nama_zip, nama_folder, NULL);
        exit(EXIT_SUCCESS);
    }
}

//fungsi mendownload gambar sesuai perintah pada soal
void download_gambar(const char* nama_folder, int thread_num) {
    time_t t = time(NULL);
    char nama_gambar[50];
    //format gambar
    sprintf(nama_gambar, "%04d-%02d-%02d_%02d:%02d:%02d_%d.jpg",
        1900 + localtime(&t)->tm_year, localtime(&t)->tm_mon + 1, localtime(&t)->tm_mday,
        localtime(&t)->tm_hour, localtime(&t)->tm_min, localtime(&t)->tm_sec, thread_num);
    char url[50];
    //Tiap gambar berbentuk persegi dengan ukuran (t%1000)+50 piksel dimana t adalah detik Epoch Unix
    sprintf(url, "https://picsum.photos/%d", (int) (t%1000)+50);
    char path[100];
    sprintf(path, "%s/%s", nama_folder, nama_gambar);

    pid_t pid = fork();
    if (pid == 0) {
        execl("/usr/bin/wget", "wget", "-q", "-O", path, url, NULL);
        exit(EXIT_SUCCESS);
    }
}

//fungsi membuat file killer sesuai dengan Modenya
void create_killer(char* nama_program) {
    FILE* fp;
    fp = fopen("killer", "w");
    fprintf(fp, "#!/bin/bash\n");

    //mengecek mode yang digunakan
    if (mode == MODE_A) {
        fprintf(fp, "killall -SIGKILL %s\n", nama_program);
    } else {
        fprintf(fp, "kill -SIGTERM %d\n", getpid());
    }

    //menghapus file killer setelah digunakan
    fprintf(fp, "rm killer\n");
    fclose(fp);
    //membuat file agar executable
    chmod("killer", 0755);
}

```
## Test Output
```./lukisan -a```
![image](https://user-images.githubusercontent.com/100351038/227714014-94824755-1d37-4374-971d-9f6dbc9048dc.png)
``` ./lukisan -b ```
![image](https://user-images.githubusercontent.com/100351038/227714333-8da801bf-a0d1-428e-93a4-8a6fdc6340fa.png)

![image](https://user-images.githubusercontent.com/100351038/227714941-c32d91b0-10ec-4c6a-b653-49f2f3fcfb40.png)

## Kendala
Kendala saat pengerjaan soal karena tidak boleh menggunakan system(), dan kesulitan dalam melakukan overlapping.

# Soal 3


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

Kemudian dibuat fungsi untuk melakukan pemilahan pemain, dimana pemain yang nama file .pngnya tidak mengandung kata "Manchester United" akan dihapus. Fungsi filterPlayers() akan membuka direktori ./players terlebih dahulu, lalu menghapus pemain yang bukan merupakan pemain Manchester United, dengan cara mencocokkan nama file png dengan kata "ManUtd".

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
