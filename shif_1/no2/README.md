# Soal 2
# Analisa Soal

Diminta untuk membuat script dimana sebuah gambar akan didownload sebanyak n kali selama 10 jam, dengan n adalah jam saat script dijalankan. Contoh: script dijalankan pukul 13:20, maka dilakukan download gambar sebanyak 13 kali. Selain itu diminta untuk melakukan zip terhadap gambar yang sudah didownload setiap 1 hari, atau setiap pukul 00:00 dengan format yang ditentukan.

## Cara pengerjaan

Inisialisasi jam saat ini dan set interval menjadi 10 jam
```shell
#!/bin/bash
HOUR=$(date + %H)
INTERVAL=10

```

Set agar download dilakukan sebanyak n kali yaitu jam saat ini, dan lakukan download sekali apabila saat ini jam 00:00

```bash
if [ "$HOUR" -eq 0 ]; then
    # Kalau sedang jam 00:00, set download menjadi 1
    DOWNLOADS=1
else
    # Kalau tidak download sebanyak jam saat ini
    DOWNLOADS="$HOUR"
fi

```
Selanjutnya mulai download, dan buat direktori sesuai dengan format nama yang ditentukan

```bash
if [ "$1" = "download" ]; then
    DIRNAME="kumpulan_$(($(ls -d kumpulan_* | wc -l) + 1))"
    mkdir "$DIRNAME"
    # mulai download file
    for ((i = 1; i <= DOWNLOADS; i++)); do
        FILENAME="perjalanan_$i"
        wget "https://id.wikipedia.org/wiki/Soekarno#/media/Berkas:Presiden_Sukarno.jpg" -O "$DIRNAME/$FILENAME"
    done
elif [ "$1" = "zip" ]; then
   #!/bin/bash
```
Kemudian buat fungsi untuk melakukan zip setiap jam 00:00. 
```bash
DATETIME=$(date +"%Y-%m-%d_%H-%M-%S")

DEVIL_ZIP_COUNT=$(ls -l | grep -c "devil_[0-9]*\.zip")

NEXT_DEVIL_ZIP_NAME="devil_$((DEVIL_ZIP_COUNT + 1)).zip"

```
Cari semua direktori yang berawalan dengan kata kumpulan lalu dibuat zip.

```bash
zip -r $NEXT_DEVIL_ZIP_NAME $DIRECTORIES
else
    echo "Usage: $0 [download|zip]"
    exit 1
fi
```
Untuk memastikan bahwa gambar didownload setiap 10 jam sekali dan dilakukan zip setiap 24 jam, maka dibuat cron job sebagai berikut

```
0 13,23 * * * /home/reyhan/Documents/shift1/kobeni_liburan.sh download
0 0 * * * /home/reyhan/Documents/shift1/kobeni_liburan.sh
```


# Source code
```bash
#!/bin/bash

# Cek jam saat ini
HOUR=$(date +%H)

# Set interval download menjadi 10 jam
INTERVAL=10

# Cek apabila saat ini jam 12 malam
if [ "$HOUR" -eq 0 ]; then
    # Kalau sedang jam 12 malam, set download menjadi 1
    DOWNLOADS=1
else
    # Set download sebanyak jam saat ini
    DOWNLOADS="$HOUR"
fi

# Buat direktori baru untuk file yang baru didownload

if [ "$1" = "download" ]; then
    DIRNAME="kumpulan_$(($(ls -d kumpulan_* | wc -l) + 1))"
    mkdir "$DIRNAME"
    # Download file
    for ((i = 1; i <= DOWNLOADS; i++)); do
        FILENAME="perjalanan_$i"
        wget "https://id.wikipedia.org/wiki/Soekarno#/media/Berkas:Presiden_Sukarno.jpg" -O "$DIRNAME/$FILENAME"
    done
elif [ "$1" = "zip" ]; then
    #!/bin/bash

# Cek tanggal dan waktu saat ini
DATETIME=$(date +"%Y-%m-%d_%H-%M-%S")

DEVIL_ZIP_COUNT=$(ls -l | grep -c "devil_[0-9]*\.zip")

NEXT_DEVIL_ZIP_NAME="devil_$((DEVIL_ZIP_COUNT + 1)).zip"


# Cari semua direktori yang berawalan dengan "kumpulan_"
DIRECTORIES=$(find . -type d -name "kumpulan_*")

# Zip direktori yang berawalan dengan kumpulan
zip -r $NEXT_DEVIL_ZIP_NAME $DIRECTORIES
else
    echo "Usage: $0 [download|zip]"
    exit 1
fi
```

## Output
Download menggunakan script 

![kobeni_download](https://user-images.githubusercontent.com/107137535/222383394-82ba4e6c-c8c2-45ac-a172-70c53aca6247.png)

Zip menggunakan script

![kobeni_zip](https://user-images.githubusercontent.com/107137535/222383475-b5bbe6c5-bf81-4da3-9df6-41564756a17b.png)

Cron job yang diset agar script berjalan sesuai dengan waktu yang ditentukan

![Screenshot 2023-03-02 133212](https://user-images.githubusercontent.com/107137535/222383603-29ab6b77-7522-4b18-9539-d88bdaea6ed9.png)


## Kendala
Tidak ada kendala yang ditemukan pada soal ini.

