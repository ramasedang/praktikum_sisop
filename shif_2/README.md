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
