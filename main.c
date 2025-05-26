#include "library.h"    //header dosyasını içe aktarır

//Tunahan Aydın
//240229029

//main fonksiyonu
int main() {
    initTurkish(); // Türkçe karakter desteği için gerekli ayarları yapar
    system("color 0A"); // Konsol rengini yeşil yapar

    //veri dosyası yoksa admin personel ve öğrenci kullanıcıları ve demo kitapları oluşturur ve data.dat dosyasına kaydeder
    if (fopen("data.dat", "rb") == NULL) {
        User admin = {1, "admin", "", "admin", {0}, {0}, {0}, 0};
        simpleHash("admin", admin.password);
        users[numUsers++] = admin;

        User staff = {2, "personel", "", "personel", {0}, {0}, {0}, 0};
        simpleHash("personel", staff.password);
        users[numUsers++] = staff;

        User student = {3, "ogrenci", "", "ogrenci", {0}, {0}, {0}, 0};
        simpleHash("ogrenci", student.password);
        users[numUsers++] = student;

        Book b1 = {1, "Sefiller", "Victor Hugo", "Roman", 1, 0, 0, 0, {""}, {""}, 0};
        Book b2 = {2, "Kumarbaz", "Dostoyevski", "Roman", 1, 0, 0, 0, {""}, {""}, 0};
        books[numBooks++] = b1;
        books[numBooks++] = b2;

        saveData();
    }

    loadData();     // Verileri ve logları dosyadan yükler
    printAsciiArt(); // Giriş ekranına ASCII art ile hoş bir görüntü ekler
    loginMenu();  //giriş menüsünü gösterir

    return 0;
}