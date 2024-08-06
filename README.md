# salva
Versi 1.1

## Component
1. Arduino Nano (Otak dari program)
2. RTC (Khusus untuk menyimpan data jam, jadi saat dinyalakan jam tetap sesuai)
3. Layar Oled I2C (Untuk menampilkan layar yang dihasilkan dari kode program arduino nano)
4. SW-430 (Sensor getar, untuk mendeteksi adanya getaran. Dalam salva, diatur setiap 30 detik jika tidak ada getaran masuk ke mode sleep. Jika ada getaran masuk ke mode standby / jam)
5. Step Down 12V (Untuk menurunkan tegangan dari adaptor / charger yang ada di salva, agar bisa diterima tegangan normal dari arduino nano)
6. Box kecil (Menyimpan semua komponen ke dalam 1 box)
7. Adaptor 12V (Memberikan tegangan 12V ke step down)

## Mode Jam
![1](https://github.com/user-attachments/assets/d5b1029d-af39-4a1e-bcfc-290aed0bda91)

## Mode Sleep (Robot)
![2](https://github.com/user-attachments/assets/6f30e4d2-9c38-4be2-a025-f9c33f6272da)
