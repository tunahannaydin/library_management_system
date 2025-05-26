# Kütüphane Yönetim Sistemi - Programlama II Projesi

**Öğrenci:** Tunahan Aydın  
**Numara:** 240229029  
**Ders:** Programlama II
**Dönem:** 2024-2025 Bahar 
## Proje Teslim Bilgileri
**Son Teslim Tarihi:** 26 Mayıs 2025 Saat 09.00
**Sunum Tarihi:** 26 Mayıs 2025 Saat 09.00-13.00

## Proje Özeti
C diliyle geliştirilen dosya tabanlı bir kütüphane yönetim sistemi. Admin, personel ve öğrenci rollerine göre farklı yetkiler sunar.

## Teknik Özellikler
- **Programlama Dili:** C
- **Veri Yapıları:** Struct'lar (Kitap, Kullanıcı, Log)
- **Bellek Yönetimi:** Pointer kullanımı
- **Dosya İşlemleri:** Verilerin kalıcı depolanması
- **Kullanıcı Yönetimi:** Admin, Personel ve Öğrenci rolleri
- **Güvenlik:** Şifre hashleme
- **Loglama:** Kullanıcı aktivitelerinin kaydı

## Özellikler
- Kitap ekleme, silme ve güncelleme
- Kullanıcı ekleme, silme ve güncelleme
- Kitap ödünç alma ve iade etme
- Kitap ve kullanıcı arama
- Ödünç alınan kitapları listeleme
- Temel istatistikleri görüntüleme (en çok ödünç alınan kitap gibi)

## Dosya ve Klasör Yapısı
lms-TunahanAydin-240229029/
├── main.c
├── library.h
├── library.c
├── giris-bilgileri.txt
└── README.md

## Nasıl Çalıştırılır?
1. Proje dosyalarını bir klasöre yerleştirin.
2. Terminal/Powershell ile Proje Klasörüne Gidin
3. Terminal ya da komut satırında aşağıdaki komutu kullanarak projeyi derleyin:

```bash
gcc main.c library.c -o kutuphane -Wall
```

4. Programı çalıştırmak için:

```bash
./kutuphane
```

## Test Hesapları

### 1. Admin Hesabı
- **Kullanıcı Adı:** `admin`
- **Şifre:** `admin`

### 2. Personel Hesabı
- **Kullanıcı Adı:** `personel`
- **Şifre:** `personel`


### 3. Öğrenci Hesapları
- **Kullanıcı Adı:** `ogrenci`
- **Şifre:** `ogrenci`

## Test Kitabı 1
- **ID** `1`
- **Kitap Adı** `Sefiller`
- **Yazar Adı** `Victor Hugo`
- **Kategorisi** `Roman`
## Test Kitabı 2
- **ID** `2`
- **Kitap Adı** `Kumarbaz`
- **Yazar Adı** `Dostoyevski`
- **Kategorisi** `Roman`

## Sistem Sınırlamaları
- Maksimum 500 kitap kaydı

- Maksimum 100 kullanıcı kaydı

- Bir kullanıcı en fazla 3 kitap ödünç alabilir

- Ödünç alma süresi 15 gün ile sınırlıdır