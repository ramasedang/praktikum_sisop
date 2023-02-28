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
