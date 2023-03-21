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

# Soal 2
# Analisa Soal

Diminta untuk membuat script dimana sebuah gambar akan didownload sebanyak n kali selama 10 jam, dengan n adalah jam saat script dijalankan. Contoh: script dijalankan pukul 13:20, maka dilakukan download gambar sebanyak 13 kali. Selain itu diminta untuk melakukan zip terhadap gambar yang sudah didownload setiap 1 hari, atau setiap pukul 00:00 dengan format yang ditentukan.

## Cara pengerjaan

Inisialisasi jam saat ini dan set interval menjadi 10 jam
```shell
#!/bin/bash
HOUR=$(date + %H)

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
0 0 * * * /home/reyhan/Documents/shift1/kobeni_liburan.sh zip
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
Terdapat kendala pada script untuk melakukan zip file, beberapa kali script membuat file zip yang tidak ada isinya.

## Soal 3
## Analisa soal:
Membuat sistem login dan register yang mana password dari user memiliki beberapa ketentuan yang harus di ikuti, dan Setiap kejadian login dan register di simpan ke log dengan format tertentu
## Cara pengerjaan soal 3:
Membuat fungsi untuk membuat folder dan file user.txt
```sh
#!/bin/bash
mkdir users
touch users/users.txt 
```

Selanjutnya membuat fungsi untuk memvalidasi password pada saat register
```sh
is_valid_password() {
    local PASSWORD=$1
    local USERNAME=$2

    # Validasi minimal 8 karakter dan alphanumeric
    if ! [[ $PASSWORD =~ ^[[:alnum:]]{8,}$ ]]; then
        echo "Password harus terdiri dari minimal 8 karakter dan alphanumeric"
        return 1
    fi

    # Validasi minimal 1 huruf kapital dan 1 huruf kecil
    if ! [[ $PASSWORD =~ [A-Z] ]] || ! [[ $PASSWORD =~ [a-z] ]]; then
        echo "Password harus memiliki minimal 1 huruf kapital dan 1 huruf kecil"
        return 1
    fi

    # Validasi tidak sama dengan username
    if [[ $PASSWORD == $USERNAME ]]; then
        echo "Password tidak boleh sama dengan username"
        return 1
    fi

    # Validasi tidak boleh menggunakan kata chicken atau ernie
    if [[ $PASSWORD =~ chicken|ernie ]]; then
        echo "Password tidak boleh menggunakan kata chicken atau ernie"
        return 1
    fi

    # Jika password lolos semua validasi, return 0
    return 0
}
```
Setelah membuat fungsi untuk validasi, Kita membuat fungsi untuk mencatat log ke pada file log
```sh
log() {
    local MESSAGE=$1
    echo "$(date +"%y/%m/%d %H:%M:%S") $MESSAGE" >> log.txt
}
```
Selanjutnya kita perlu membuat fungsi register 
```sh
register() {
    local USERNAME=$1
    local PASSWORD=""

    # Loop untuk input password yang valid
    while true; do
        read -s -p "Masukkan password untuk $USERNAME: " PASSWORD
        echo
        if is_valid_password $PASSWORD $USERNAME; then
            break
        fi
    done


    # Cek apakah username sudah terdaftar
    if grep -q "^$USERNAME:" users/users.txt; then
        log "REGISTER: ERROR User $USERNAME already exists"
        echo "Username sudah terdaftar"
    else
        # Simpan data user ke file
        echo "$USERNAME:$PASSWORD" >> users/users.txt
        log "REGISTER: INFO User $USERNAME registered successfully"
        echo "Registrasi berhasil dilakukan untuk $USERNAME"
    fi
}
```
Setelah itu kita membuat fungsi login dan menu utama
```sh
login() {
    local USERNAME=$1
    local PASSWORD=""

    # Loop untuk input password yang valid
    while true; do
        read -s -p "Masukkan password untuk $USERNAME: " PASSWORD
        echo
        if is_valid_password $PASSWORD $USERNAME; then
            break
        fi
    done 

    # Cek apakah username dan password sudah terdaftar di file
    if grep -q "^$USERNAME:$PASSWORD$" users/users.txt; then
        log "LOGIN: INFO User $USERNAME logged in"
        echo "Login berhasil dilakukan untuk $USERNAME"
    else
        log "LOGIN: ERROR Failed login attempt on user $USERNAME"
        echo "Username atau password salah"
    fi
}

# Menu utama
while true; do
    echo "Silakan pilih opsi:"
    echo "1. Registrasi"
    echo "2. Login"
    read -p "Pilihan anda: " CHOICE

    case $CHOICE in
        1)
            read -p "Masukkan username baru: " USERNAME
            register $USERNAME
            ;;
       2)
        read -p "Masukkan username: " USERNAME
        login $USERNAME
        ;;
    *)
        echo "Pilihan tidak valid"
        ;;
esac

done
```
## Source code
```sh
#!/bin/bash

# Membuat direktori users dan file users.txt
mkdir users
touch users/users.txt

# Fungsi untuk memvalidasi password
is_valid_password() {
    local PASSWORD=$1
    local USERNAME=$2

    # Validasi minimal 8 karakter dan alphanumeric
    if ! [[ $PASSWORD =~ ^[[:alnum:]]{8,}$ ]]; then
        echo "Password harus terdiri dari minimal 8 karakter dan alphanumeric"
        return 1
    fi

    # Validasi minimal 1 huruf kapital dan 1 huruf kecil
    if ! [[ $PASSWORD =~ [A-Z] ]] || ! [[ $PASSWORD =~ [a-z] ]]; then
        echo "Password harus memiliki minimal 1 huruf kapital dan 1 huruf kecil"
        return 1
    fi

    # Validasi tidak sama dengan username
    if [[ $PASSWORD == $USERNAME ]]; then
        echo "Password tidak boleh sama dengan username"
        return 1
    fi

    # Validasi tidak boleh menggunakan kata chicken atau ernie
    if [[ $PASSWORD =~ chicken|ernie ]]; then
        echo "Password tidak boleh menggunakan kata chicken atau ernie"
        return 1
    fi

    # Jika password lolos semua validasi, return 0
    return 0
}

# Fungsi untuk mencatat log
log() {
    local MESSAGE=$1
    echo "$(date +"%y/%m/%d %H:%M:%S") $MESSAGE" >> log.txt
}

# Fungsi untuk registrasi user
register() {
    local USERNAME=$1
    local PASSWORD=""

    # Loop untuk input password yang valid
    while true; do
        read -s -p "Masukkan password untuk $USERNAME: " PASSWORD
        echo
        if is_valid_password $PASSWORD $USERNAME; then
            break
        fi
    done


    # Cek apakah username sudah terdaftar
    if grep -q "^$USERNAME:" users/users.txt; then
        log "REGISTER: ERROR User $USERNAME already exists"
        echo "Username sudah terdaftar"
    else
        # Simpan data user ke file
        echo "$USERNAME:$PASSWORD" >> users/users.txt
        log "REGISTER: INFO User $USERNAME registered successfully"
        echo "Registrasi berhasil dilakukan untuk $USERNAME"
    fi
}

# Fungsi untuk login user
login() {
    local USERNAME=$1
    local PASSWORD=""

    # Loop untuk input password yang valid
    while true; do
        read -s -p "Masukkan password untuk $USERNAME: " PASSWORD
        echo
        if is_valid_password $PASSWORD $USERNAME; then
            break
        fi
    done 

    # Cek apakah username dan password sudah terdaftar di file
    if grep -q "^$USERNAME:$PASSWORD$" users/users.txt; then
        log "LOGIN: INFO User $USERNAME logged in"
        echo "Login berhasil dilakukan untuk $USERNAME"
    else
        log "LOGIN: ERROR Failed login attempt on user $USERNAME"
        echo "Username atau password salah"
    fi
}

# Menu utama
while true; do
    echo "Silakan pilih opsi:"
    echo "1. Registrasi"
    echo "2. Login"
    read -p "Pilihan anda: " CHOICE

    case $CHOICE in
        1)
            read -p "Masukkan username baru: " USERNAME
            register $USERNAME
            ;;
       2)
        read -p "Masukkan username: " USERNAME
        login $USERNAME
        ;;
    *)
        echo "Pilihan tidak valid"
        ;;
esac

done
```
## Test output
saat di run

[![image](https://www.linkpicture.com/q/abdyusvdiua.png)](https://www.linkpicture.com/view.php?img=LPic63fd9267c7ed1870417813)

Error saat salah satu kondisi password tidak terpenuhi

[![image](https://www.linkpicture.com/q/Capture_885.png)](https://www.linkpicture.com/view.php?img=LPic63fd92be63d86825860864)

Setelah register, user akan ditambahkan ke users/users.txt

[![image](https://www.linkpicture.com/q/Capture_886.png)](https://www.linkpicture.com/view.php?img=LPic63fd9352e2a1f1325625848)

Begitupula dengan log, log register akan tercatat pada file log

[![image](https://www.linkpicture.com/q/Capture_887.png)](https://www.linkpicture.com/view.php?img=LPic63fd93e51ca54517175628)

Berikut menjalankan fungsi login

[![image](https://www.linkpicture.com/q/Capture_888.png)](https://www.linkpicture.com/view.php?img=LPic63fd96308a39c102323120)
## Kendala
Kendala yang dialamu mungkin penggunaan syntax yang belum familiar terhadap penggunaan bash sehingga kami harus menyesuaikan syntax dan harus memperlajari syntax baru, Penulisan syntax yang cukup rumit juga menjadi kendala kami



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
jam=${jam#0}
syslog=$(cat /var/log/syslog)
```
```jam=$(date +'%H')``` fungsi tersebut untuk mengambil jam saat backup dilakukan menggunakan perintah date dan menyimpannya ke dalam variabel jam. ```jam=${jam#0}``` digunakan untuk membuang angka nol di depan bilangan jam jika ada, sehingga nilai jam menjadi dua digit. Fungsi ```syslog=$(cat /var/log/syslog)``` untuk membaca isi file /var/log/syslog menggunakan perintah cat dan menyimpannya ke dalam variabel syslog.

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
echo -e "$encrypted" > "/home/kali/Desktop/$filename"
```
```filename=$(date +'%H:%M %d:%m:%Y').txt``` perintah ini untuk  menginisialisasi variabel filename dengan format timestamp tersebut adalah "Jam:Menit Tanggal:Bulan:Tahun" dan ditambahkan ekstensi ".txt".```echo -e "$encrypted" > "/home/kali/Desktop/$filename"``` untuk menulis isi dari variabel ```$encrypted``` ke file dengan nama yang ditentukan dalam variabel ```$filename``` pada folder Desktop.

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
0 */2 * * * /home/kali/Desktop/log_encrypt.sh
```
```0``` pada menit ke 0 ```*/2```menunjukkan interval waktu backup setiap 2 jam. Angka lainnya menunjukkan interval waktu dalam menit, jam, atau hari.File backup script memiliki izin eksekusi dengan menggunakan perintah ```chmod +x log_encrypt.sh``` 

## Source Code
#### log_encrypt.sh
```bash
#!/bin/bash
jam=$(date +'%H')
jam=${jam#0}
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
echo -e "$encrypted" > "/home/kali/Desktop/$filename"
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

## Kendala
Kesulitan untuk melakukan enkripsi dan dekripsi dengan sistem cipher dengan menyesuaikan dengan jam sekarang. Dan, syslog yang dienkiripsi dan didekripsi sangat banyak sehingga waktu yang diperlukan untuk menjalankannya lama, sehingga perlu menghapus syslognya.
