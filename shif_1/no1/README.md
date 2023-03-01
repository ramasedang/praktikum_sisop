# sisop-praktikum-modul-1-2023-mhfg-it14

## Laporan Resmi Modul 1 Praktikum Sistem Operasi 2023
---
### Kelompok ITA14:
nama - nrp  
nama - nrp    
nama - nrp  

---
## Soal 1
## Analisa soal:
Pada nomer 1 pertama Bocchi ingin Menampilkan 5 Universitas dengan ranking tertinggi di Jepang berdasarkan file CSV nya, 5 Universitas paling rendah berdasarkan kolom fsr score dan hasil filter yang pertama, Mencari 10 Universitas di Jepang dengan ger rank paling tinggi, Mencari unniversitas dengan kata kunci "keren".

## Cara pengerjaan soal 1:
Pertama kita perlu membuat command untuk Menampilkan 5 Universitas dengan ranking tertinggi di Jepang berdasarkan file CSV nya

```sh
#!/bin/bash

grep "Japan" list_univ.csv | sort -n -t , -k 1 | head -n 5 | awk -F "," '{print $1","$2","$3}'
 ```
disini menggunakan ```grep``` untuk mengambil baris yang mengandung kata "Japan" , menggunakan ```sort``` untuk me-nyorting berdasarkan kolom ke 1 dengan parameter ```-k 1```, ``` head -n 5 ``` disini digunakan untuk mengambil 5 baris teratas dari hasil yang sudah di sorting, ``` awk ``` Perintah ```awk -F "," '{print $1","$2","$3}'``` digunakan untuk memproses suatu file atau output yang memuat data yang dipisahkan oleh koma (",") dan menampilkan kolom pertama, kedua, dan ketiga dari setiap baris dalam format yang sama dengan menggunakan koma sebagai pemisah antar kolom.

Selanjutnya 5 Universitas paling rendah berdasarkan kolom fsr score dan hasil filter yang pertama

```sh
grep -e "Japan" list_univ.csv | sort  -n -t , -k 1 | head -n 5 | sort -t , -k 9 | awk -F "," '{print $2","$9}'
```
Penjelasannya sama seperti diatas namun karena kita me sort berdasarkan fs score yang mana itu ada pada kolom ke-9 maka cukup ditambah ```sort -t , -k 9```

Selanjutnya  Mencari 10 Universitas di Jepang dengan ger rank paling tinggi

```sh 
grep "Japan" list_univ.csv | sort -n -t , -k 20 | head -n 10 | awk -F "," '{print $2","$20}'
```

Penjelasannya sama seperti diatas namun karena kita me sort berdasarkan ger rank yang mana itu ada pada kolom ke-20 dan mengambil 10 teratas maka cukup ditambah ``` sort -n -t , -k 20 | head -n 10``` 

Selanjutnya Mencari unniversitas dengan kata kunci “keren”.

```sh
grep -i "keren" list_univ.csv | awk -F "," '{print $1","$2","$3}'
```
## Source code
```sh
#!/bin/bash

echo -e "\nTampilkan 5 Universitas dengan ranking tertinggi di Jepang: \n"

grep "Japan" list_univ.csv | sort -n -t , -k 1 | head -n 5 | awk -F "," '{print $1","$2","$3}'

echo -e "\n(fsr score) yang paling rendah diantara 5 Universitas dari hasil filter poin a. : \n"

grep -e "Japan" list_univ.csv | sort  -n -t , -k 1 | head -n 5 | sort -t , -k 9 | awk -F "," '{print $2","$9}'

echo -e "\ncari 10 Universitas di Jepang dengan Employment Outcome Rank(ger rank) paling tinggi. : \n"

grep "Japan" list_univ.csv | sort -n -t , -k 20 | head -n 10 | awk -F "," '{print $2","$20}'

echo -e "\nBantu bocchi mencari universitas tersebut dengan kata kunci keren. : \n"

grep -i "keren" list_univ.csv | awk -F "," '{print $1","$2","$3}'
```
## Test output
[![image](https://www.linkpicture.com/q/asasasasa_1.png)](https://www.linkpicture.com/view.php?img=LPic63fd870844073856134278)

## Kendala
Penggunaan awk yang belum familiar sehingga perlu memperlajarinya, dan juga sedikit kebingungan saat sorting ascending dan descending

## Soal 4

## Analisis soal

Soal tersebut meminta kita untuk membuat script bash yang dapat melakukan backup file syslog setiap 2 jam dengan format jam:menit tanggal:bulan:tahun dalam format .txt. Isi dari file backup tersebut harus dienkripsi dengan sistem cipher yang disesuaikan dengan jam dilakukannya backup. Sistem cipher yang digunakan adalah menggeser setiap huruf sebanyak nilai jam saat backup dilakukan. Contohnya, jika pada pukul 12, huruf b diganti dengan huruf alfabet yang memiliki urutan ke-14. Jika setelah penggeseran huruf tersebut melebihi urutan huruf z, maka akan kembali ke huruf a. Selain itu, kita juga diminta untuk membuat script dekripsi untuk isi file backup yang sudah dienkripsi.

## Cara pengerjaan soal 4 :
#### Pertama kita perlu membuat file Shell untuk enkripsinya 
```bash
nano log_encrypt.sh
```

```bash
#!/bin/bash
jam=$(date +'%H')
syslog=$(cat /var/log/syslog)
```
```jam=$(date +'%H')``` fungsi tersebut untuk mengambil jam saat backup dilakukan menggunakan perintah date dan menyimpannya ke dalam variabel jam.  Fungsi ```syslog=$(cat /var/log/syslog)``` untuk membaca isi file /var/log/syslog menggunakan perintah cat dan menyimpannya ke dalam variabel syslog.

```bash 
encrypted=""
echo ${#syslog}
for ((i=0; i<${#syslog}; i++)); do
    char="${syslog:$i:1}"
    if [[ "$char" =~ [A-Za-z] ]]; then
        if [[ "$char" =~ [A-Z] ]]; then
            charcode=$(( ( $(printf '%d' "'$char") - 65 + $jam) % 26 + 65 ))
            char=$(printf \\$(printf '%03o' "$charcode"))
        else
            charcode=$(( ( $(printf '%d' "'$char") - 97 + $jam) % 26 + 97 ))
            char=$(printf \\$(printf '%03o' "$charcode"))
        fi
    fi
    encrypted="${encrypted}${char}"
done
```
Fungsi ```encrypted=""``` untuk melakukan inisialisasi variabel encrypted sebagai string kosong untuk menampung hasil enkripsi. Kita perlu melakukan ```for ((i=0; i<${#syslog}; i++))``` untuk mengiterasi setiap karakter pada variabel syslog. ```char="${syslog:$i:1}"``` digunakan menyimpan karakter pada indeks i ke dalam variabel char. Diperlukan fungsi ```if [[ "$char" =~ [A-Za-z] ]]``` untuk memeriksa apakah karakter tersebut adalah huruf (A-Z atau a-z). Selanjutnya ```if [[ "$char" =~ [A-Z] ]]``` kondisi untuk memeriksa apakah huruf tersebut adalah huruf besar (A-Z). ```charcode=$(( ( $(printf '%d' "'$char") - 65 + $jam) % 26 + 65 ))``` digunakan untuk menghitung kode ASCII dari huruf tersebut, menambahkan nilai variabel jam, dan mengubahnya kembali ke huruf sesuai dengan kode ASCII. ```char=$(printf \\$(printf '%03o' "$charcode"))``` mengubah kode ASCII menjadi karakter yang sesuai dan menyimpannya ke dalam variabel char. Dan untuk kondisi ``else``` jika huruf tersebut bukan huruf besar, maka dilakukan perhitungan yang sama seperti di atas, tapi untuk huruf kecil.Setelah kondisi ```if``` telah selesai selanjutnya ```encrypted="${encrypted}${char}"``` untuk menambahkan karakter yang telah dienkripsi ke dalam variabel encrypted.
```bash
filename=$(date +'%H:%M %d:%m:%Y').txt
echo -e "$encrypted" > "$filename"
```
```filename=$(date +'%H:%M %d:%m:%Y').txt``` perintah ini untuk  menginisialisasi variabel filename dengan format timestamp tersebut adalah "Jam:Menit Tanggal:Bulan:Tahun" dan ditambahkan ekstensi ".txt".``` echo -e "$encrypted" > "$filename"``` untuk menulis isi dari variabel ```$encrypted``` ke file dengan nama yang ditentukan dalam variabel ```$filename```.

##### Selanjutnya untuk file dekripsinya kita perlu melakukan file shell lagi
```bash
nano log_decrypt.sh
```
```bash
!/bin/bash

filename="$1"
jam="${filename:0:2}"

encrypted=$(cat "$filename")
```
Fungsi diatas untuk mengambil jam dari nama file backup dan membuka file backup dan simpan isinya pada variabel. ```filename="$1"```  menginisialisasi variabel filename dengan argumen pertama yang diberikan saat menjalankan script. Argumen pertama tersebut dianggap sebagai nama file yang akan didekripsi. ```jam="${filename:0:2}"```untuk menginisialisasi variabel jam dengan dua karakter pertama dari filename, yang berisi jam pada saat file tersebut dienkripsi.```encrypted=$(cat "$filename")``` membaca isi file yang akan didekripsi dan menyimpannya dalam variabel encrypted.

```bash 
decrypted=""
for ((i=0; i<${#encrypted}; i++)); do
    char="${encrypted:$i:1}"
    if [[ "$char" =~ [A-Za-z] ]]; then
        if [[ "$char" =~ [A-Z] ]]; then
            charcode=$(( ( $(printf '%d' "'$char") - 65 - $jam + 26) % 26 + 65 ))
            char=$(printf \\$(printf '%03o' "$charcode"))
        else
            charcode=$(( ( $(printf '%d' "'$char") - 97 - $jam + 26) % 26 + 97 ))
            char=$(printf \\$(printf '%03o' "$charcode"))
        fi
    fi
    decrypted="${decrypted}${char}"
done
```
Kode di atas untuk mendekripsi setiap huruf pada variabel encrypted.Fungsi ```decrypted=""``` untuk menginisialisasi variabel decrypted sebagai string kosong. Nanti variabel ini akan diisi dengan hasil dekripsi.Digunakan ```for ((i=0; i<${#encrypted}; i++))``` untuk mengiterasi setiap karakter pada variabel encrypted. Lalu di dalam foor loop perlu ```char="${encrypted:$i:1}"``` untuk menginisialisasi variabel char dengan karakter ke-i dari variabel encrypted.```if [[ "$char" =~ [A-Za-z] ]]``` mengecek apakah karakter yang sedang diproses merupakan karakter alfabet.```if [[ "$char" =~ [A-Z] ]] ``` mengecek apakah karakter yang sedang diproses merupakan huruf kapital. ```charcode=$(( ( $(printf '%d' "'$char") - 65 - $jam + 26) % 26 + 65 ))``` perintah ini menghitung kode ASCII dari karakter yang sedang diproses setelah didekripsi dengan menggunakan rumus Caesar Cipher. Variabel ```$jam``` menyimpan kunci, yang merupakan jumlah posisi alfabet yang digeser. Mengurangkan 65 dari kode karakter ASCII ```($(printf '%d' "'$char") - 65)``` menormalkan kode ke indeks berbasis nol.Variabel ```$jam``` kemudian dikurangi untuk menggeser karakter kembali ke posisi semula sebelum dienkripsi. ```26``` ditambahkan untuk memastikan bahwa hasilnya positif. Operator ```modulo % 26 ```memastikan bahwa karakter tetap berada dalam jangkauan alfabet. ```65``` ditambahkan kembali untuk mengembalikan kode ASCII ke kisaran aslinya (yaitu, 'A' menjadi 65, 'B' menjadi 66, dan seterusnya). ```char=$(printf \\$(printf '%03o' "$charcode"))``` untuk mengonversi kode ASCII yang dihitung sebelumnya menjadi karakter yang sesuai dan menyimpannya dalam variabel char. ```else``` dieksekusi jika karakter yang sedang diproses merupakan huruf kecil. Setelah kondisi ```if``` telah selesai selanjutnya ```decrypted="${decrypted}${char}"``` untuk menambahkan karakter yang telah didekripsi ke dalam variabel decrypted.

```bash 
decrypted_filename="${filename%.*}_decrypted.txt"
echo -e "$decrypted" > "$decrypted_filename"
```
Simpan hasil dekripsi pada file baru dengan format "jam:menit tanggal:bulan:tahun_decrypted.txt"
Untuk menjalankan programnya perlu disertai dengan jam, contohnya :
```bash 
bash log_decrypt.sh 06:36\ 28:02:2023.txt
```

#### Backup file syslog setiap 2 jam
```bash 
crontab -e
```
```bash 
*/2 * * * * /Desktop/log_encrypt.sh
```
```*/2```menunjukkan interval waktu backup setiap 2 jam. Angka lainnya menunjukkan interval waktu dalam menit, jam, atau hari.File backup script memiliki izin eksekusi dengan menggunakan perintah ```chmod +x log_encrypt.sh``` 

## Source Code
#### log_encrypt.sh
```bash
#!/bin/bash
jam=$(date +'%H')
syslog=$(cat /var/log/syslog)

encrypted=""
echo ${#syslog}
for ((i=0; i<${#syslog}; i++)); do
    char="${syslog:$i:1}"
    if [[ "$char" =~ [A-Za-z] ]]; then
        if [[ "$char" =~ [A-Z] ]]; then
            charcode=$(( ( $(printf '%d' "'$char") - 65 + $jam) % 26 + 65 ))
            char=$(printf \\$(printf '%03o' "$charcode"))
        else
            charcode=$(( ( $(printf '%d' "'$char") - 97 + $jam) % 26 + 97 ))
            char=$(printf \\$(printf '%03o' "$charcode"))
        fi
    fi
    encrypted="${encrypted}${char}"
done

filename=$(date +'%H:%M %d:%m:%Y').txt
echo -e "$encrypted" > "$filename"
```
#### log_decrypt.sh
```bash
#!/bin/bash

filename="$1"
jam="${filename:0:2}"

encrypted=$(cat "$filename")

decrypted=""
for ((i=0; i<${#encrypted}; i++)); do
    char="${encrypted:$i:1}"
    if [[ "$char" =~ [A-Za-z] ]]; then
        if [[ "$char" =~ [A-Z] ]]; then
            charcode=$(( ( $(printf '%d' "'$char") - 65 - $jam + 26) % 26 + 65 ))
            char=$(printf \\$(printf '%03o' "$charcode"))
        else
            charcode=$(( ( $(printf '%d' "'$char") - 97 - $jam + 26) % 26 + 97 ))
            char=$(printf \\$(printf '%03o' "$charcode"))
        fi
    fi
    decrypted="${decrypted}${char}"
done

decrypted_filename="${filename%.*}_decrypted.txt"
echo -e "$decrypted" > "$decrypted_filename"
```
## Test Output

![enkripsi](https://user-images.githubusercontent.com/100351038/222177319-9271884c-27f0-4057-bbe8-756d572559d6.png)
![dekripsi](https://user-images.githubusercontent.com/100351038/222177413-305bcae6-baaf-4e98-950e-84d8f8eeae65.png)
