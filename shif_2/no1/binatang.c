#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zip.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#define MAX_FILES 100	// maksimum jumlah file
#define MAX_FILENAME_LENGTH 255	// maksimum panjang nama file
#define MAX_PATH 1024

// referensi https://stackoverflow.com/questions/48182274/how-to-unzip-a-zipped-file-zip-using-c-in-visual-studio


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

			char *buffer = (char*) malloc(st.size);
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
	struct dirent * entry;
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
		char *buffer = (char*) malloc(size);
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
	DIR * dir;
	struct dirent * ent;
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
	DIR * dir;
	struct dirent * entry;
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

	return 0;
}