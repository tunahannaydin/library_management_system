// library.h dosyasının tekrar tekrar include edilmesini engeller
#ifndef LIBRARY_H
#define LIBRARY_H

//Tunahan Aydın
//240229029


#include <stdio.h> //standart girdi/çıktı işlemleri için (örneğin: printf, scanf)
#include <stdlib.h> //standart kütüphane işlemleri için(örneğin: malloc, free)
#include <string.h> //string işlemleri için (örneğin: strcpy, strcmp)
#include <time.h> //zaman işlemleri için (örneğin: time, localtime)
#include <locale.h> //Türkçe karakter desteği için (örneğin: setlocale)
#include <windows.h> //windowsa özgü fonksiyonlar için (örneğin: Sleep, system)
#include <conio.h> //klavye girdi işlemleri için (örneğin: getch, kbhit)
#include <ctype.h> //karakter işlemleri için (örneğin: toupper, isdigit)

#define COLOR_RESET "\033[0m" //Renkleri resetler
#define COLOR_DARK_BLUE "\033[34m" //Koyu mavi renk
#define COLOR_LIGHT_BLUE "\033[36m" //Açık mavi renk
#define COLOR_CYAN "\033[96m"   //Cyan renk
#define COLOR_SOFT_WHITE "\033[97m" //Parlak beyaz renk
#define COLOR_GREEN "\033[92m" //Yeşil renk
#define COLOR_ORANGE "\033[33m" //Turuncu renk
#define COLOR_RED "\033[91m"    //Kırmızı renk
#define COLOR_PURPLE "\033[95m" //Mor renk

#define ERROR_COLOR COLOR_RED //hata mesajları için kırmızı
#define WARNING_COLOR COLOR_ORANGE //uyarılar için turuncu rengi
#define INFO_COLOR COLOR_LIGHT_BLUE //info için açık mavi
#define SUCCESS_COLOR COLOR_GREEN //başarıyla gerçekleşen işlemler için mavi rengi

#define MAX_BOOKS 500  //maksimum kitaplar için üst sınır
#define MAX_USERS 100 //maksimum kullanıcı sayısı için üst sınır
#define MAX_BORROWED 3 //kullanıcılar için en fazla ödünç alınabilecek kitap sayısı
#define LOAN_PERIOD 15 // ödünç için maksimum gün sayısı
#define MAX_LOG_ENTRIES 100 // maksimum log girişi sayısı (sistem performansını korumak için)

typedef struct {
    int id; // Kitabın ID.si
    char title[100]; //kitap adı
    char author[50]; //o kitabın yazarının adı
    char category[30]; //kitap kategorisi (bir nevi tür)
    int available; //rafta mı yoksa ödünç mü verildi
    time_t dueDate;  //son iade tarihi
    float rating; //kitabın oyu (0.0 - 5.0 arasında bir değer alıyor)
    int borrowCount; //kitap kaç kere ödünç alınmış
    char comments[10][200]; //kitabın yorumları (en fazla 10 yorum ve her yorum en fazla 200 karakter)
    char commentAuthors[10][50]; //o yorumu yapan kullanıcıların adları
    int commentCount; //yorum sayısı (en fazla 10 yorum)
} Book;

typedef struct {
    int id; //kullanıcıların ID.si
    char username[50]; //kullanıcı adı 
    char password[50]; //kullanıcı şifresi
    char role[20];  //kullanıcı rolü (admin, personel, öğrenci)
    int borrowedBooks[MAX_BORROWED];  //ödünç alınan kitapların ID.leri
    time_t borrowDates[MAX_BORROWED];   //ödünç alınan kitapların alınma tarihi
    time_t returnDates[MAX_BORROWED]; //ödünç alınan kitapların son iade tarihi
    int numBorrowed; //ödünç alınan kitap sayısı (üst sınır 3 kitap)
    int allBorrowedBooks[100]; //tüm ödünç alınan kitapların ID'leri
    time_t allBorrowDates[100]; //tüm ödünç alınan kitapların alınma tarihleri
    time_t allReturnDates[100]; //tüm ödünç alınan kitapların iade tarihleri
    int totalBorrowed; //tüm ödünç alınan kitap sayısı (all time)
} User;  

typedef struct {
    time_t timestamp; //log kaydının zaman etiketi
    char username[50]; //log kaydını yapan kullanıcı
    char activity[200]; //log kaydının açıklamsı (örnek: "Kitap eklendi" ...)
} LogEntry;

// Uygulamanın genelinde kullanılacak, ana veri yapılarının ve sayaçların harici (extern) tanımları.
// Bu değişkenler library.c dosyasında tanımlanmalı ve diğer dosyalardan erişim için burada bildirilmiştir.
extern Book books[MAX_BOOKS];
extern User users[MAX_USERS];
extern LogEntry logs[MAX_LOG_ENTRIES];
extern int numBooks;
extern int numUsers;
extern int numLogs;
extern User* currentUser;

void initTurkish(); // türkçe karakter desteği için gerekli
void simpleHash(const char* input, char* output); //kullanıcı şifrelerini DJB2 algoritmasıyla hashler güvenliği artırmak için tercih ettim
void printAsciiArt(); //ASCII art ile giriş ekranında görece estetik bir ekran oluşturmak için
void logActivity(const char* activity); //log kaydı oluşturmak için
void displayBook(Book book); //kitap bilgilerini ekrana yazdırmak için
void displayUser(User user);    //kullanıcı bilgilerini ekrana yazdırmak için
void loadData(); //veri dosyasından verileri yüklemek için
void saveData(); //veri dosyasına verileri kaydetmek için
int exportToCSV(const char* filename, int dataType); //verileri CSV dosyasına aktarmak için (verileri program dışında da erişmek için)
int checkCredentials(const char* username, const char* password); //kullanıcı adı ve şifre kontrolü yapmak için
void addUser(); //yeni bi kullanıcı eklemek için
void deleteUser(); //mevcut bir kullanıcıyı silmek için
void updateUser(); //mevcut olan bir kullanıcının bilgilerini günvellemek için
void listUsers(); //tüm kullanıcıları listelemek için
void addBook();     //yeni bir kitap eklemek için
void deleteBook();  //mevcut bir kitabı silmek için
void updateBook();  //mevcut olan bir kitabın bilgilerini güncellemek için
void listBooks(); //tüm kitapları listelemek için
int caseInsensitiveCompare(const char *str1, const char *str2); //kitap arama fonksiyonu için büyük-küçük harf farketmeden aramak için
void searchBook(); //kitap arama fonksiyonu (kitap adı, yazar adı,kategori ve ID.ye göre arama yapmak için)
void borrowBook(); //kitap ödünç almak için (personel ve öğrenci)
void returnBook(); //ödünç alınan kitabı iade etmek için
void viewBorrowed(); //ödünç alınan kitapları görüntülemek için
void showStatistics();  //istatistikleri görüntülemek için (admin)
void checkOverdueBooks(); //geciken kitapları kontrol etmek için (admin)
void adminMenu(); //admin paneli
void staffMenu(); //personel paneli
void studentMenu(); //öğrenci paneli
void loginMenu(); //giriş ekranı
void initSystem(); // Sistemi ilk başlatmada admin hesabı oluşturmak için

#endif