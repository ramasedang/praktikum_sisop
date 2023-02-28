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
