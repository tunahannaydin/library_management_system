#include "library.h" //header dosyasını içe aktarır

Book books[MAX_BOOKS]; //kitaplar için dizi
User users[MAX_USERS]; //kullanıcılar için dizi
LogEntry logs[MAX_LOG_ENTRIES]; //log kayıtları için dizi
int numBooks = 0; //toplam kitap sayısını tutar
int numUsers = 0; //toplam kullanıcı sayısını tutar
int numLogs = 0; //toplam log kaydı sayısını tutar
User* currentUser = NULL; //giriş yapmış kullanıcıyı tutar

 // Türkçe karakter desteği için gerekli ayarları yapar
void initTurkish() {
    setlocale(LC_ALL, "Turkish"); // Türkçe dil desteğini etkinleştirir
    SetConsoleOutputCP(1254); // Konsol çıktısı için Türkçe karakter setini ayarlar
    SetConsoleCP(1254); // Konsol karakter setini Türkçe yapar
}

//DJB2 algoritmasıyla şifreler için basit hashleme yaptım
void simpleHash(const char* input, char* output) {
    unsigned long hash = 5381; //sihirli sayı
    int c;
    // DJB2 algoritması her karakter için hash değerini günceller böylece farklı stringler farklı hash üretir
    while ((c = *input++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    sprintf(output, "%lx", hash); //hash sonucunu hex string olarak outputa yazar
}

//giriş ekranına güzel bir görüntü için ASCIIArt yazdırma fonksiyonu
void printAsciiArt() {
    system("cls");
    printf(COLOR_DARK_BLUE);
    printf("\n");
    printf(" _   ___   _ _____ _   _______ _   _   ___   _   _  _____ \n");
    printf("| | / / | | |_   _| | | | ___ \\ | | | / _ \\ | \\ | ||  ___|\n");
    printf("| |/ /| | | | | | | | | | |_/ / |_| |/ /_\\ \\|  \\| || |__  \n");
    printf("|    \\| | | | | | | | | |  __/|  _  ||  _  || . ` ||  __| \n");
    printf("| |\\  \\ |_| | | | | |_| | |   | | | || | | || |\\  || |___ \n");
    printf("\\_| \\_/\\___/  \\_/  \\___/\\_|   \\_| |_/\\_| |_/\\_| \\_/\\____/ \n");
    printf(COLOR_RESET);
    printf("\n==============================================================\n\n");
    printf(COLOR_LIGHT_BLUE "Kutuphane Yonetim Sistemine Hosgeldiniz!\n" COLOR_RESET);
}

// kulanıcı akt. hem dosyaya hem de bellekteki log dizisine kaydeder
void logActivity(const char* activity) {
    FILE* logFile = fopen("logs.txt", "a");
    if (logFile) {
        time_t now = time(NULL);
        char* timeStr = ctime(&now);
        timeStr[strlen(timeStr)-1] = '\0';
        fprintf(logFile, "[%s] %s - %s\n", timeStr, currentUser->username, activity);
        fclose(logFile);
    }

    if (numLogs < MAX_LOG_ENTRIES) {
        logs[numLogs].timestamp = time(NULL);
        strncpy(logs[numLogs].username, currentUser->username, 50);
        strncpy(logs[numLogs].activity, activity, 200);
        numLogs++;
    }
}

//kitapların bilgilerini görüntülemek için kullanılan fonksiyon
void displayBook(Book book) {
    printf(COLOR_CYAN "\nID: %d\n" COLOR_RESET, book.id);
    printf(COLOR_SOFT_WHITE "Ad: %s\n" COLOR_RESET, book.title);
    printf(COLOR_SOFT_WHITE "Yazar: %s\n" COLOR_RESET, book.author);
    printf(COLOR_LIGHT_BLUE "Kategori: %s\n" COLOR_RESET, book.category);
    printf(COLOR_LIGHT_BLUE "Durum: %s\n" COLOR_RESET, book.available ? "Rafta" : "Odunc Verildi");

    if (!book.available) {
        char dateStr[20];
        strftime(dateStr, sizeof(dateStr), "%d.%m.%Y", localtime(&book.dueDate));
        printf(COLOR_ORANGE "Iade Tarihi: %s\n" COLOR_RESET, dateStr);
    }

    printf(COLOR_LIGHT_BLUE "Odunc Alma Sayisi: %d\n" COLOR_RESET, book.borrowCount);
    printf(COLOR_LIGHT_BLUE "Puan: %.1f/5\n" COLOR_RESET, book.rating);

    if (book.commentCount > 0) {
        printf(COLOR_PURPLE "\nYorumlar:\n" COLOR_RESET);
        for (int i = 0; i < book.commentCount; i++) {
            printf(COLOR_SOFT_WHITE "- %s " COLOR_ORANGE "(Yorum yapan: %s)\n" COLOR_RESET,
                   book.comments[i], book.commentAuthors[i]);
        }
    }
}

//kullanıcıların bilgilerini görüntülemek için kullanılan fonksiyon
void displayUser(User user) {
    printf(COLOR_CYAN "\nID: %d\n" COLOR_RESET, user.id);
    printf(COLOR_LIGHT_BLUE "Kullanici Adi: %s\n" COLOR_RESET, user.username);
    printf(COLOR_LIGHT_BLUE "Rol: %s\n" COLOR_RESET, user.role);

    printf(COLOR_PURPLE "\nSU ANDA ODUNC ALINAN KITAPLAR (%d):\n" COLOR_RESET, user.numBorrowed);
    for (int i = 0; i < user.numBorrowed; i++) {
        for (int j = 0; j < numBooks; j++) {
            if (books[j].id == user.borrowedBooks[i]) {
                printf(COLOR_SOFT_WHITE "- Kitap Adi: %s\n" COLOR_RESET, books[j].title);
                char borrowDateStr[20];
                strftime(borrowDateStr, sizeof(borrowDateStr), "%d.%m.%Y", localtime(&user.borrowDates[i]));
                printf(COLOR_LIGHT_BLUE "  Odunc Alma Tarihi: %s\n" COLOR_RESET, borrowDateStr);

                if (user.returnDates[i] == 0) {
                    char dueDateStr[20];
                    strftime(dueDateStr, sizeof(dueDateStr), "%d.%m.%Y", localtime(&books[j].dueDate));
                    printf(COLOR_ORANGE "  Iade Tarihi: %s (Bekleniyor)\n" COLOR_RESET, dueDateStr);
                }
                break;
            }
        }
    }

    printf(COLOR_PURPLE "\nTUM ODUNC ALMA GECMISI (%d):\n" COLOR_RESET, user.totalBorrowed);
    for (int i = 0; i < user.totalBorrowed; i++) {
        for (int j = 0; j < numBooks; j++) {
            if (books[j].id == user.allBorrowedBooks[i]) {
                printf(COLOR_SOFT_WHITE "- Kitap Adi: %s\n" COLOR_RESET, books[j].title);
                char borrowDateStr[20], returnDateStr[20];
                strftime(borrowDateStr, sizeof(borrowDateStr), "%d.%m.%Y", localtime(&user.allBorrowDates[i]));
                printf(COLOR_LIGHT_BLUE "  Odunc Alma Tarihi: %s\n" COLOR_RESET, borrowDateStr);

                if (user.allReturnDates[i] != 0) {
                    strftime(returnDateStr, sizeof(returnDateStr), "%d.%m.%Y", localtime(&user.allReturnDates[i]));
                    printf(COLOR_LIGHT_BLUE "  Iade Tarihi: %s\n" COLOR_RESET, returnDateStr);
                } else {
                    printf(COLOR_ORANGE "  Iade Tarihi: Henuz iade edilmedi\n" COLOR_RESET);
                }
                break;
            }
        }
    }
}

//dataları ve logları dosyadan yükler
void loadData() {
    FILE* file = fopen("data.dat", "rb");
    if (file) {
        fread(&numBooks, sizeof(int), 1, file);
        fread(&numUsers, sizeof(int), 1, file);
        if (numBooks > 0) fread(books, sizeof(Book), numBooks, file);
        if (numUsers > 0) fread(users, sizeof(User), numUsers, file);
        fclose(file);
    }

    FILE* logFile = fopen("logs.txt", "r");
    if (logFile) {
        char line[300];
        while (fgets(line, sizeof(line), logFile) && numLogs < MAX_LOG_ENTRIES) {
            strncpy(logs[numLogs].activity, line, 200);
            numLogs++;
        }
        fclose(logFile);
    }
}

//verileri ve logları dosyaya kaydeder
void saveData() {
    FILE* file = fopen("data.dat", "wb");
    if (file) {
        fwrite(&numBooks, sizeof(int), 1, file);
        fwrite(&numUsers, sizeof(int), 1, file);
        if (numBooks > 0) fwrite(books, sizeof(Book), numBooks, file);
        if (numUsers > 0) fwrite(users, sizeof(User), numUsers, file);
        fclose(file);
    }
}

//csv formatında veri aktarma menüsünü gösterir ve seçilen verileri belirttiğiniz dosyaya aktarır
//csv olarak aktarmanın amacı verileri kolayca görüp analiz etmeyi sağlamaktır
int exportToCSV(const char* filename, int dataType) {
    int choice;
    do {
        system("cls");
        printf(COLOR_PURPLE "\n=== CSV AKTARIM MENUSU ===\n" COLOR_RESET);
        printf("1. Kitaplari Aktar\n");
        printf("2. Kullanicilari Aktar\n");
        printf("3. Loglari Aktar\n");
        printf("0. Ana Menuye Don\n");
        printf("Seciminiz: ");
        
        while(scanf("%d", &choice) != 1) {
            printf(ERROR_COLOR "Hata: Sayi girmelisiniz!\n" COLOR_RESET);
            while(getchar() != '\n');
            printf("Seciminiz: ");
        }

        if (choice == 0) return 0;

        if (choice < 1 || choice > 3) {
            printf(ERROR_COLOR "Gecersiz secim! Tekrar deneyin.\n" COLOR_RESET);
            Sleep(1033);
            continue;
        }

        char filename[50];
        printf("Dosya adi (ornek: veriler.csv): ");
        scanf("%49s", filename);

        FILE* file = fopen(filename, "w");
        if (!file) {
            printf(ERROR_COLOR "Dosya acilamadi veya yazma izni yok!\n" COLOR_RESET);
            return -1;
        }

        switch (choice) {
            case 1:
                fprintf(file, "ID,Baslik,Yazar,Kategori,Durum,Iade Tarihi,Puan,Odunc Alma Sayisi,Yorum Sayisi\n");
                for (int i = 0; i < numBooks; i++) {
                    char dateStr[20] = "N/A";
                    if (books[i].dueDate != 0) {
                        strftime(dateStr, sizeof(dateStr), "%d.%m.%Y", localtime(&books[i].dueDate));
                    }
                    fprintf(file, "%d,\"%s\",\"%s\",\"%s\",%s,%s,%.1f,%d,%d\n",
                           books[i].id, books[i].title, books[i].author, books[i].category,
                           books[i].available ? "Rafta" : "Odunc", dateStr,
                           books[i].rating, books[i].borrowCount, books[i].commentCount);
                }
                break;

            case 2:
                fprintf(file, "ID,Kullanici Adi,Rol,Odunc Alinan Kitap Sayisi\n");
                for (int i = 0; i < numUsers; i++) {
                    fprintf(file, "%d,%s,%s,%d\n",
                           users[i].id, users[i].username, users[i].role, users[i].numBorrowed);
                }
                break;

            case 3:
                fprintf(file, "Zaman Damgasi,Kullanici,Aktivite\n");
                for (int i = 0; i < numLogs; i++) {
                    char timeStr[30];
                    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localtime(&logs[i].timestamp));
                    fprintf(file, "\"%s\",%s,\"%s\"\n", timeStr, logs[i].username, logs[i].activity);
                }
                break;
        }

        fclose(file);
        printf(SUCCESS_COLOR "Veriler basariyla '%s' dosyasina aktarildi.\n" COLOR_RESET, filename);
        logActivity("CSV aktarimi yapildi");
        Sleep(2033);
        return 0;
    } while (1);
}

//giriş yaparken kullanıcı adı ve şifreyi kontrol eder eşleşirse kullanıcı indeksi eşleşmeşse -1 döner
int checkCredentials(const char* username, const char* password) {
    for (int i = 0; i < numUsers; i++) {
        if (strcmp(users[i].username, username) == 0 &&
            strcmp(users[i].password, password) == 0) {
            return i;
        }
    }
    return -1;
}

//kullanıcı ekleme fonksiyonu
void addUser() {
    if (numUsers >= MAX_USERS) {
        printf(ERROR_COLOR "\nMaksimum kullanici sayisina ulasildi!\n" COLOR_RESET);
        Sleep(1033);
        return;
    }

    User newUser;
    memset(&newUser, 0, sizeof(User));

    system("cls");
    printf(COLOR_PURPLE "\n=== YENI KULLANICI EKLE ===\n" COLOR_RESET);
    printf("(Cikmak icin 0 girin)\n");

    char username[50];
    printf("Kullanici Adi: ");
    scanf("%49s", username);

    if (strcmp(username, "0") == 0) return;

    for (int i = 0; i < numUsers; i++) {
        if (strcmp(users[i].username, username) == 0) {
            printf(ERROR_COLOR "\nBu kullanici adi zaten alinmis!\n" COLOR_RESET);
            Sleep(1533);
            return;
        }
    }
    strcpy(newUser.username, username);
    //şifre girişini gizli şekilde alır her tuşu "*" sembolüyle sansürler ve şifreyi string olarak kaydeder
    char password[50];
    printf("Sifre: ");
    int i = 0;
    int ch;
    while (1) {
        ch = getch();
        if (ch == 13) { //enter tuşu
            password[i] = '\0';
            break;
        } else if (ch == 8) { //backspace tuşu
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else {
            password[i] = ch;
            i++;
            printf("*");
        }
    }
    //şifre hashleme işlemi
    printf("\n");
    simpleHash(password, newUser.password);

    // Rol seçimi -hatayı düzelttim-
    int roleChoice;
    printf("\nRol Secimi:\n");
    printf("1. Admin\n");
    printf("2. Personel\n");
    printf("3. Ogrenci\n");
    printf("Seciminiz (1-3): ");
    
    while(1) {
        if(scanf("%d", &roleChoice) != 1) {
            printf(ERROR_COLOR "Hata: Sayi girmelisiniz!\n" COLOR_RESET);
            while(getchar() != '\n');
            printf("Seciminiz (1-3): ");
            continue;
        }
        
        if(roleChoice < 1 || roleChoice > 3) {
            printf(ERROR_COLOR "Hata: 1-3 arasi bir sayi girmelisiniz!\n" COLOR_RESET);
            printf("Seciminiz (1-3): ");
            continue;
        }
        break;
    }

    switch(roleChoice) {
        case 1: strcpy(newUser.role, "admin"); break;
        case 2: strcpy(newUser.role, "personel"); break;
        case 3: strcpy(newUser.role, "ogrenci"); break;
    }

    newUser.id = (numUsers == 0) ? 1 : users[numUsers-1].id + 1;

    newUser.numBorrowed = 0;
    newUser.totalBorrowed = 0;
    memset(newUser.borrowedBooks, 0, sizeof(newUser.borrowedBooks));
    memset(newUser.borrowDates, 0, sizeof(newUser.borrowDates));
    memset(newUser.returnDates, 0, sizeof(newUser.returnDates));
    memset(newUser.allBorrowedBooks, 0, sizeof(newUser.allBorrowedBooks));
    memset(newUser.allBorrowDates, 0, sizeof(newUser.allBorrowDates));
    memset(newUser.allReturnDates, 0, sizeof(newUser.allReturnDates));

    users[numUsers++] = newUser;
    saveData();

    printf(SUCCESS_COLOR "\nKullanici basariyla eklendi! Rol: %s\n" COLOR_RESET, newUser.role);
    logActivity("Yeni kullanici eklendi");
    Sleep(1533);
}
//kullanıcı silme fonksiyonu
void deleteUser() {
    system("cls");
    listUsers();

    printf(COLOR_PURPLE "\n=== KULLANICI SIL ===\n" COLOR_RESET);
    printf("\nSilmek istediginiz kullanicinin ID'sini girin: ");
    printf(COLOR_GREEN "\nCikmak icin 0 girin\n" COLOR_RESET);
    //-bug düzeltildi-
    int userId;
    while(scanf("%d", &userId) != 1) {
        printf(ERROR_COLOR "Hata: Sayi girmelisiniz!\n" COLOR_RESET);
        while(getchar() != '\n');
        printf("Kullanici ID'si girin: ");
    }

    if (userId == 0) return;

    int userIndex = -1;
    for (int i = 0; i < numUsers; i++) {
        if (users[i].id == userId) {
            userIndex = i;
            break;
        }
    }

    if (userIndex == -1) {
        printf(ERROR_COLOR "\nGecersiz kullanici ID'si!\n" COLOR_RESET);
        Sleep(1033);
        return;
    }
    //kendini silme hatasını düzelttim
    if (strcmp(users[userIndex].username, currentUser->username) == 0) {
        printf(ERROR_COLOR "\nKendinizi silemezsiniz!\n" COLOR_RESET);
        Sleep(1033);
        return;
    }

    for (int i = userIndex; i < numUsers - 1; i++) {
        users[i] = users[i + 1];
    }
    numUsers--;

    saveData();
    printf(SUCCESS_COLOR "\nKullanici basariyla silindi.\n" COLOR_RESET);
    logActivity("Kullanici silindi");
    Sleep(1533);
}

// kullanıcı güncelleme fonksiyonu
void updateUser() {
    system("cls");
    listUsers();

    printf(COLOR_PURPLE "\n=== KULLANICI GUNCELLE ===\n" COLOR_RESET);
    printf("\nGuncellemek istediginiz kullanicinin ID'sini girin: ");
    printf(COLOR_GREEN "\nCikmak icin 0 girin\n" COLOR_RESET);
    
    int userId;
    int i;
    int ch;
    while(scanf("%d", &userId) != 1) {
        printf(ERROR_COLOR "Hata: Sayi girmelisiniz!\n" COLOR_RESET);
        while(getchar() != '\n');
        printf("Kullanici ID'si girin: ");
    }

    if (userId == 0) return;

    int userIndex = -1;
    for (int i = 0; i < numUsers; i++) {
        if (users[i].id == userId) {
            userIndex = i;
            break;
        }
    }

    if (userIndex == -1) {
        printf(ERROR_COLOR "\nGecersiz kullanici ID'si!\n" COLOR_RESET);
        Sleep(1033);
        return;
    }

    printf("\nYeni bilgileri girin (degistirmek istemiyorsaniz enter'a basin):\n");

    //kullanıcı adı daha önce alınmış mı kontrolü
    char newUsername[50];
    printf("Yeni kullanici adi (%s): ", users[userIndex].username);
    getchar();
    fgets(newUsername, 50, stdin);
    newUsername[strcspn(newUsername, "\n")] = 0;
    if (strlen(newUsername) > 0 && strcmp(newUsername, "0") != 0) {
        for (int i = 0; i < numUsers; i++) {
            if (i != userIndex && strcmp(users[i].username, newUsername) == 0) {
                printf(ERROR_COLOR "\nBu kullanici adi zaten alinmis!\n" COLOR_RESET);
                Sleep(1533);
                return;
            }
        }
        strcpy(users[userIndex].username, newUsername);
    } else if (strcmp(newUsername, "0") == 0) {
        return;
    }

    char newPassword[50];
    printf("Yeni sifre (degistirmek istemiyorsaniz bos birakin): ");
    i = 0;
    while (1) {
        ch = getch();
        if (ch == 13) {
            newPassword[i] = '\0';
            break;
        } else if (ch == 8) {
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else {
            newPassword[i] = ch;
            i++;
            printf("*");
        }
    }
    printf("\n");
    if (strlen(newPassword) > 0) {
        if (strcmp(newPassword, "0") == 0) return;
        simpleHash(newPassword, users[userIndex].password);
    }

    // Rol güncelleme kısmını düzelttim
    printf("\nYeni rol (%s - Degistirmek icin 1-3 arasi sayi girin, degistirmek istemiyorsaniz 0 girin):\n", users[userIndex].role);
    printf("1. Admin\n");
    printf("2. Personel\n");
    printf("3. Ogrenci\n");
    printf("Seciminiz (0-3): ");
    
    int roleChoice;
    while(1) {
        if(scanf("%d", &roleChoice) != 1) {
            printf(ERROR_COLOR "Hata: Sayi girmelisiniz!\n" COLOR_RESET);
            while(getchar() != '\n');
            printf("Seciminiz (0-3): ");
            continue;
        }
        
        if(roleChoice < 0 || roleChoice > 3) {
            printf(ERROR_COLOR "Hata: 0-3 arasi bir sayi girmelisiniz!\n" COLOR_RESET);
            printf("Seciminiz (0-3): ");
            continue;
        }
        break;
    }

    if(roleChoice != 0) {
        switch(roleChoice) {
            case 1: strcpy(users[userIndex].role, "admin"); break;
            case 2: strcpy(users[userIndex].role, "personel"); break;
            case 3: strcpy(users[userIndex].role, "ogrenci"); break;
        }
    }

    saveData();
    printf(SUCCESS_COLOR "\nKullanici bilgileri basariyla guncellendi.\n" COLOR_RESET);
    logActivity("Kullanici guncellendi");
    Sleep(1533);
}

//tüm kullanıcıları listeleme fonksiyonu
void listUsers() {
    system("cls");
    printf(COLOR_PURPLE "\n=== TUM KULLANICILAR ===\n" COLOR_RESET);

    //döngüyle tüm kullanıcıları listeler
    for (int i = 0; i < numUsers; i++) {
        displayUser(users[i]);
        printf("\n----------------------------\n");
    }

    printf("\nListeleme tamamlandi. Devam etmek icin bir tusa basin...");
    getchar(); getchar();
}
//kitap ekleme fonksiyonu
void addBook() {
    if (numBooks >= MAX_BOOKS) {
        printf(ERROR_COLOR "\nMaksimum kitap sayisina ulasildi!\n" COLOR_RESET);
        Sleep(1033);
        return;
    }

    Book newBook;
    system("cls");
    printf(COLOR_PURPLE "\n=== YENI KITAP EKLE ===\n" COLOR_RESET);
    printf("(Cikmak icin kitap adina 0 girin)\n");

    newBook.id = (numBooks == 0) ? 1 : books[numBooks-1].id + 1;

    printf("Kitap Adi: ");
    getchar();
    fgets(newBook.title, 100, stdin);
    newBook.title[strcspn(newBook.title, "\n")] = 0;

    if (strcmp(newBook.title, "0") == 0) return;

    printf("Yazar: ");
    fgets(newBook.author, 50, stdin);
    newBook.author[strcspn(newBook.author, "\n")] = 0;

    printf("Kategori: ");
    fgets(newBook.category, 30, stdin);
    newBook.category[strcspn(newBook.category, "\n")] = 0;

    newBook.available = 1;
    newBook.dueDate = 0;
    newBook.rating = 0.0;
    newBook.borrowCount = 0;
    newBook.commentCount = 0;
    memset(newBook.commentAuthors, 0, sizeof(newBook.commentAuthors));

    books[numBooks++] = newBook;
    saveData();

    printf(SUCCESS_COLOR "\nKitap basariyla eklendi! ID: %d\n" COLOR_RESET, newBook.id);
    logActivity("Yeni kitap eklendi");
    Sleep(1533);
}

//kitap silme fonksiyonu
void deleteBook() {
    system("cls");
    listBooks();
    printf(COLOR_PURPLE "\n=== KITAP SIL ===\n" COLOR_RESET);

    printf("\nSilmek istediginiz kitabin ID'sini girin: ");
    printf(COLOR_GREEN "\nCikmak icin 0 girin\n" COLOR_RESET);
    
    int bookId;
    while(scanf("%d", &bookId) != 1) {
        printf(ERROR_COLOR "Hata: Sayi girmelisiniz!\n" COLOR_RESET);
        while(getchar() != '\n');
        printf("Kitap ID'si girin: ");
    }

    if (bookId == 0) return;

    int bookIndex = -1;
    for (int i = 0; i < numBooks; i++) {
        if (books[i].id == bookId) {
            bookIndex = i;
            break;
        }
    }

    if (bookIndex == -1) {
        printf(ERROR_COLOR "\nGecersiz kitap ID'si!\n" COLOR_RESET);
        Sleep(1033);
        return;
    }

    if (!books[bookIndex].available) {
        printf(ERROR_COLOR "\nBu kitap su anda odunc alinmis durumda. Once iade edilmesi gerekir!\n" COLOR_RESET);
        Sleep(1533);
        return;
    }

    for (int i = bookIndex; i < numBooks - 1; i++) {
        books[i] = books[i + 1];
    }
    numBooks--;

    saveData();
    printf(SUCCESS_COLOR "\nKitap basariyla silindi.\n" COLOR_RESET);
    logActivity("Kitap silindi");
    Sleep(1533);
}

//kitap güncelleme fonksiyonu
void updateBook() {
    system("cls");
    listBooks();

    printf(COLOR_PURPLE "\n=== KITAP GUNCELLE ===\n" COLOR_RESET);
    printf("\nGuncellemek istediginiz kitabin ID'sini girin: ");
    printf(COLOR_GREEN "\nCikmak icin 0 girin\n" COLOR_RESET);
    
    int bookId;
    while(scanf("%d", &bookId) != 1) {
        printf(ERROR_COLOR "Hata: Sayi girmelisiniz!\n" COLOR_RESET);
        while(getchar() != '\n');
        printf("Kitap ID'si girin: ");
    }

    if (bookId == 0) return;

    int bookIndex = -1;
    for (int i = 0; i < numBooks; i++) {
        if (books[i].id == bookId) {
            bookIndex = i;
            break;
        }
    }

    if (bookIndex == -1) {
        printf(ERROR_COLOR "\nGecersiz kitap ID'si!\n" COLOR_RESET);
        Sleep(1033);
        return;
    }

    printf("\nYeni bilgileri girin (degistirmek istemiyorsaniz enter'a basin):\n");

    char newTitle[100];
    printf("Yeni kitap adi (%s): ", books[bookIndex].title);
    getchar();
    fgets(newTitle, 100, stdin);
    newTitle[strcspn(newTitle, "\n")] = 0;
    if (strlen(newTitle) > 0) {
        if (strcmp(newTitle, "0") == 0) return;
        strcpy(books[bookIndex].title, newTitle);
    }

    char newAuthor[50];
    printf("Yeni yazar (%s): ", books[bookIndex].author);
    fgets(newAuthor, 50, stdin);
    newAuthor[strcspn(newAuthor, "\n")] = 0;
    if (strlen(newAuthor) > 0) {
        if (strcmp(newAuthor, "0") == 0) return;
        strcpy(books[bookIndex].author, newAuthor);
    }

    char newCategory[30];
    printf("Yeni kategori (%s): ", books[bookIndex].category);
    fgets(newCategory, 30, stdin);
    newCategory[strcspn(newCategory, "\n")] = 0;
    if (strlen(newCategory) > 0) {
        if (strcmp(newCategory, "0") == 0) return;
        strcpy(books[bookIndex].category, newCategory);
    }

    saveData();
    printf(SUCCESS_COLOR "\nKitap bilgileri basariyla guncellendi.\n" COLOR_RESET);
    logActivity("Kitap guncellendi");
    Sleep(1533);
}

//tüm kitapları listeleme fonksiyonu
void listBooks() {
    system("cls");
    printf(COLOR_PURPLE "\n=== KITAP LISTESI ===\n" COLOR_RESET);

    if(numBooks == 0) {
        printf("\nKayitli kitap bulunamadi.\n");
    } else {
        for(int i = 0; i < numBooks; i++) {
            displayBook(books[i]);
            printf("\n-------------------\n");
        }
    }

    printf("\nListeleme tamamlandi. Devam etmek icin bir tusa basin...");
    while(getchar() != '\n');
    getchar();
}
//arama yaparken büyük- küçük harf fark etmeden arayan fonksiyon
int caseInsensitiveCompare(const char *str1, const char *str2) {
    while (*str1 && *str2) {
        if (tolower(*str1) != tolower(*str2)) {
            return 0;
        }
        str1++;
        str2++;
    }
    return (*str1 == '\0' && *str2 == '\0');
}

//kitap arama fonksiyonu
void searchBook() {
    int choice;
    char searchTerm[100];

    do {
        system("cls");
        printf(COLOR_PURPLE "\n=== KITAP ARAMA ===\n" COLOR_RESET);
        printf("Arama turu:\n");
        printf("1. Kitap Adina Gore\n");
        printf("2. Yazara Gore\n");
        printf("3. Kategoriye Gore\n");
        printf("4. ID'ye Gore\n");
        printf("0. Ana Menuye Don\n");
        printf("Seciminiz: ");

        while(scanf("%d", &choice) != 1) {
            printf(ERROR_COLOR "Hata: Sayi girmelisiniz!\n" COLOR_RESET);
            while(getchar() != '\n');
            printf("Seciminiz: ");
        }

        if (choice == 0) return;

        if (choice < 1 || choice > 4) {
            printf(ERROR_COLOR "Gecersiz secim! Tekrar deneyin.\n" COLOR_RESET);
            Sleep(1033);
            continue;
        }

        printf("Arama terimini girin (Ana menuye donmek icin 0 girin): ");
        getchar();
        fgets(searchTerm, 100, stdin);
        searchTerm[strcspn(searchTerm, "\n")] = 0;

        if (strcmp(searchTerm, "0") == 0) continue;

        system("cls");
        printf(COLOR_PURPLE "\n=== ARAMA SONUCLARI ===\n" COLOR_RESET);
        int found = 0;

        for (int i = 0; i < numBooks; i++) {
            int match = 0;
            char lowerTitle[100], lowerAuthor[100], lowerCategory[100];

            strcpy(lowerTitle, books[i].title);
            for (int j = 0; lowerTitle[j]; j++) lowerTitle[j] = tolower(lowerTitle[j]);

            strcpy(lowerAuthor, books[i].author);
            for (int j = 0; lowerAuthor[j]; j++) lowerAuthor[j] = tolower(lowerAuthor[j]);

            strcpy(lowerCategory, books[i].category);
            for (int j = 0; lowerCategory[j]; j++) lowerCategory[j] = tolower(lowerCategory[j]);

            char lowerTerm[100];
            strcpy(lowerTerm, searchTerm);
            for (int j = 0; lowerTerm[j]; j++) lowerTerm[j] = tolower(lowerTerm[j]);

            switch (choice) {
                case 1:
                    match = (strstr(lowerTitle, lowerTerm) != NULL);
                    break;
                case 2:
                    match = (strstr(lowerAuthor, lowerTerm) != NULL);
                    break;
                case 3:
                    match = (strstr(lowerCategory, lowerTerm) != NULL);
                    break;
                case 4:
                    match = (books[i].id == atoi(searchTerm));
                    break;
            }

            if (match) {
                displayBook(books[i]);
                printf("\n----------------------------\n");
                found = 1;
            }
        }

        if (!found) {
            printf(ERROR_COLOR "Arama kriterlerine uygun kitap bulunamadi.\n" COLOR_RESET);
        }

        printf("\nArama tamamlandi. Devam etmek icin bir tusa basin...");
        getchar();
    } while (1);
}

//kitap ödünç alma fonksiyonu
void borrowBook() {
    system("cls");
    listBooks();
    printf(COLOR_PURPLE "\n=== KITAP ODUNC AL ===\n" COLOR_RESET);
    printf("(Cikmak icin 0 girin)\n");

    time_t now = time(NULL);

    if (strcmp(currentUser->role, "admin") == 0) {
        printf(ERROR_COLOR "Adminler kitap odunc alamaz!\n" COLOR_RESET);
        Sleep(2033);
        return;
    }

    if (currentUser->numBorrowed >= MAX_BORROWED) {
        printf(ERROR_COLOR "Maksimum %d kitap odunc alabilirsiniz. Iade etmeden yeni kitap alamazsiniz.\n" COLOR_RESET, MAX_BORROWED);
        Sleep(2033);
        return;
    }

    printf("Odunc almak istediginiz kitabin ID'sini girin: ");
    int bookId;
    
    while(scanf("%d", &bookId) != 1) {
        printf(ERROR_COLOR "Hata: Sayi girmelisiniz!\n" COLOR_RESET);
        while(getchar() != '\n');
        printf("Kitap ID'si girin: ");
    }

    if (bookId == 0) return;

    int bookIndex = -1;
    for (int i = 0; i < numBooks; i++) {
        if (books[i].id == bookId) {
            bookIndex = i;
            break;
        }
    }

    if (bookIndex == -1) {
        printf(ERROR_COLOR "Gecersiz kitap ID'si!\n" COLOR_RESET);
        Sleep(1033);
        return;
    }

    if (!books[bookIndex].available) {
        printf(ERROR_COLOR "Bu kitap su anda odunc verilmis durumda.\n" COLOR_RESET);
        Sleep(2033);
        return;
    }

    books[bookIndex].available = 0;
    books[bookIndex].borrowCount++;

    currentUser->allBorrowedBooks[currentUser->totalBorrowed] = bookId;
    currentUser->allBorrowDates[currentUser->totalBorrowed] = now;
    currentUser->allReturnDates[currentUser->totalBorrowed] = 0;
    currentUser->totalBorrowed++;

    books[bookIndex].dueDate = now + (LOAN_PERIOD * 24 * 60 * 60);

    currentUser->borrowedBooks[currentUser->numBorrowed] = bookId;
    currentUser->borrowDates[currentUser->numBorrowed] = now;
    currentUser->returnDates[currentUser->numBorrowed] = 0;
    currentUser->numBorrowed++;

    saveData();

    char dateStr[20];
    strftime(dateStr, sizeof(dateStr), "%d.%m.%Y", localtime(&books[bookIndex].dueDate));
    printf(SUCCESS_COLOR "\nKitap basariyla odunc alindi. Iade tarihi: %s\n" COLOR_RESET, dateStr);
    logActivity("Kitap odunc alindi");
    Sleep(2033);
}

//kütüphaneye kitap iade etme fonksiyonu
void returnBook() {
    system("cls");
    viewBorrowed();
    printf(COLOR_PURPLE "\n=== KITAP IADE ET ===\n" COLOR_RESET);
    printf("(Cikmak icin 0 girin)\n");
    time_t now = time(NULL);

    if (currentUser->numBorrowed == 0) {
        printf(ERROR_COLOR "Odunc alinmis kitabiniz bulunmamaktadir.\n" COLOR_RESET);
        Sleep(1033);
        return;
    }

    printf("Iade etmek istediginiz kitabin ID'sini girin: ");
    int bookId;
    
    while(scanf("%d", &bookId) != 1) {
        printf(ERROR_COLOR "Hata: Sayi girmelisiniz!\n" COLOR_RESET);
        while(getchar() != '\n');
        printf("Kitap ID'si girin: ");
    }

    if (bookId == 0) return;

    int userBookIndex = -1;
    for (int i = 0; i < currentUser->numBorrowed; i++) {
        if (currentUser->borrowedBooks[i] == bookId) {
            userBookIndex = i;
            break;
        }
    }

    if (userBookIndex == -1) {
        printf(ERROR_COLOR "Bu kitap sizin tarafinizdan odunc alinmamis.\n" COLOR_RESET);
        Sleep(1033);
        return;
    }

    int bookIndex = -1;
    for (int i = 0; i < numBooks; i++) {
        if (books[i].id == bookId) {
            bookIndex = i;
            break;
        }
    }

    if (bookIndex == -1) {
        printf(ERROR_COLOR "Hata: Kitap bulunamadi!\n" COLOR_RESET);
        Sleep(1033);
        return;
    }

    books[bookIndex].available = 1;
    currentUser->returnDates[userBookIndex] = time(NULL);

    for (int i = 0; i < currentUser->totalBorrowed; i++) {
        if (currentUser->allBorrowedBooks[i] == bookId && currentUser->allReturnDates[i] == 0) {
            currentUser->allReturnDates[i] = now;
            break;
        }
    }

    for (int i = userBookIndex; i < currentUser->numBorrowed - 1; i++) {
        currentUser->borrowedBooks[i] = currentUser->borrowedBooks[i+1];
        currentUser->borrowDates[i] = currentUser->borrowDates[i+1];
        currentUser->returnDates[i] = currentUser->returnDates[i+1];
    }
    currentUser->numBorrowed--;

    if (now > books[bookIndex].dueDate) {
        int daysLate = (now - books[bookIndex].dueDate) / (24 * 60 * 60);
        printf(ERROR_COLOR "\nKitap %d gun gecikmeyle iade edildi. Lutfen kutuphane gorevlisinden gec iade dilekcesi isteyin!\n" COLOR_RESET, daysLate);
    } else {
        printf(SUCCESS_COLOR "\nKitap basariyla iade edildi.\n" COLOR_RESET);
    }

    //ödeve kendimden de bir şeyler katmak için kitap puanlama özelliği eklemek istedim
    printf("\nKitabi puanlamak ister misiniz? (e/h): ");
    char choice = getch();
    printf("\n");
    
    if (choice == 'e' || choice == 'E') {
        while(1) {
            printf("\nBu Kitaba Kac Puan Verirsiniz (1-5 arasi, cikmak icin 0 girin): ");
            float rating;
            
            while(scanf("%f", &rating) != 1) {
                printf(ERROR_COLOR "Hata: Sayi girmelisiniz!\n" COLOR_RESET);
                while(getchar() != '\n');
                printf("Puan girin: ");
            }
            
            if(rating == 0) {
                break;
            }
            
            if (rating >= 1 && rating <= 5) {
                if (books[bookIndex].borrowCount == 1) {
                    books[bookIndex].rating = rating;
                } else {
                    books[bookIndex].rating = 
                        (books[bookIndex].rating * (books[bookIndex].borrowCount - 1) + rating) 
                        / books[bookIndex].borrowCount;
                }
                
                printf("\nYorumunuz (max 200 karakter, bos birakmak icin enter'a basin): ");
                getchar();
                char comment[200];
                fgets(comment, 200, stdin);
                comment[strcspn(comment, "\n")] = 0;

                if (strlen(comment) > 0 && books[bookIndex].commentCount < 10) {
                    strcpy(books[bookIndex].comments[books[bookIndex].commentCount], comment);
                    strcpy(books[bookIndex].commentAuthors[books[bookIndex].commentCount], currentUser->username);
                    books[bookIndex].commentCount++;
                }
                
                printf(SUCCESS_COLOR "Kitap puaniniz basariyla kaydedildi!\n" COLOR_RESET);
                break;
            } else {
                printf(ERROR_COLOR "Gecersiz puan! 1-5 arasi bir deger girin.\n" COLOR_RESET);
            }
        }
        Sleep(1533);
    }
    saveData();
    logActivity("Kitap iade edildi");
    Sleep(2033);
}
//kullanıcının şuan elindeki kitapları görüntüleme fonksiyonu
void viewBorrowed() {
    system("cls");
    printf(COLOR_PURPLE "\n=== ODUNC ALDIGIM KITAPLAR ===\n" COLOR_RESET);

    if (currentUser->numBorrowed == 0) {
        printf("Odunc alinmis kitabiniz bulunmamaktadir.\n");
    } else {
        time_t now = time(NULL);

        for (int i = 0; i < currentUser->numBorrowed; i++) {
            for (int j = 0; j < numBooks; j++) {
                if (books[j].id == currentUser->borrowedBooks[i]) {
                    displayBook(books[j]);

                    char borrowDateStr[20], dueDateStr[20];
                    strftime(borrowDateStr, sizeof(borrowDateStr), "%d.%m.%Y", localtime(&currentUser->borrowDates[i]));
                    strftime(dueDateStr, sizeof(dueDateStr), "%d.%m.%Y", localtime(&books[j].dueDate));

                    printf("\nOdunc Alma Tarihi: %s\n", borrowDateStr);
                    printf("Iade Tarihi: %s\n", dueDateStr);
                    //gecikme kontrolü
                    if (now > books[j].dueDate) {
                        int daysLate = (now - books[j].dueDate) / (24 * 60 * 60);
                        printf(ERROR_COLOR "!!! %d GUN GECIKME !!! Bir an once iade et...\n" COLOR_RESET, daysLate);
                    }

                    printf("\n----------------------------\n");
                    break;
                }
            }
        }
    }

    printf("\nDevam etmek icin bir tusa basin...");
    getchar(); getchar();
}

//admin için kütüphane istatistiklerini görüntüleme fonksiyonu
void showStatistics() {
    system("cls");
    printf(COLOR_PURPLE "\n=== ISTATISTIKLER ===\n" COLOR_RESET);

    printf("\nEn Cok Odunc Alinan 5 Kitap:\n");

    for (int i = 0; i < numBooks - 1; i++) {
        for (int j = 0; j < numBooks - i - 1; j++) {
            if (books[j].borrowCount < books[j+1].borrowCount) {
                Book temp = books[j];
                books[j] = books[j+1];
                books[j+1] = temp;
            }
        }
    }

    int limit = (numBooks < 5) ? numBooks : 5;
    for (int i = 0; i < limit; i++) {
        printf("%d. %s (ID: %d) - %d kez\n",
               i+1, books[i].title, books[i].id, books[i].borrowCount);
    }

    printf("\nGenel Istatistikler:\n");
    printf("Toplam Kitap Sayisi: %d\n", numBooks);
    printf("Toplam Kullanici Sayisi: %d\n", numUsers);

    int availableBooks = 0;
    for (int i = 0; i < numBooks; i++) {
        if (books[i].available) availableBooks++;
    }
    printf("Raftaki Kitap Sayisi: %d\n", availableBooks);
    printf("Odunc Verilen Kitap Sayisi: %d\n", numBooks - availableBooks);

    printf("\nDevam etmek icin bir tusa basin...");
    getchar(); getchar();

    for (int i = 0; i < numBooks - 1; i++) {
        for (int j = 0; j < numBooks - i - 1; j++) {
            if (books[j].id > books[j+1].id) {
                Book temp = books[j];
                books[j] = books[j+1];
                books[j+1] = temp;
            }
        }
    }

    logActivity("Istatistikler goruntulendi");
}

//admin için gecikmiş kitapları kontrol etme fonksiyonu
void checkOverdueBooks() {
    system("cls");
    printf(COLOR_PURPLE "\n=== GECIKMIS KITAPLAR ===\n" COLOR_RESET);
    
    time_t now = time(NULL);
    int found = 0;
    
    for (int i = 0; i < numUsers; i++) {
        for (int j = 0; j < users[i].numBorrowed; j++) {
            for (int k = 0; k < numBooks; k++) {
                if (books[k].id == users[i].borrowedBooks[j] && !books[k].available) {
                    if (now > books[k].dueDate) {
                        found = 1;
                        printf("\nKullanici: %s", users[i].username);
                        printf("\nKitap: %s (ID: %d)", books[k].title, books[k].id);
                        
                        char dueDateStr[20];
                        strftime(dueDateStr, sizeof(dueDateStr), "%d.%m.%Y", localtime(&books[k].dueDate));
                        printf("\nIade Tarihi: %s", dueDateStr);
                        
                        int daysLate = (now - books[k].dueDate) / (24 * 60 * 60);
                        printf(ERROR_COLOR "\nGecikme: %d gun\n" COLOR_RESET, daysLate);
                        printf("-------------------\n");
                    }
                }
            }
        }
    }
    
    if (!found) {
        printf("\nGecikmis kitap bulunamadi.\n");
    }
    
    printf("\nDevam etmek icin bir tusa basin...");
    getchar(); getchar();
}
//admin paneli
void adminMenu() {
    int choice;
    do {
        system("cls");
        printf(COLOR_DARK_BLUE "\n=== ADMIN PANELI ===\n" COLOR_RESET);
        printf("1. Kitap Ekle\n");
        printf("2. Kitap Listele\n");
        printf("3. Kitap Sil\n");
        printf("4. Kitap Guncelle\n");
        printf("5. Kitap Ara\n");
        printf("6. Kullanici Ekle\n");
        printf("7. Kullanici Listele\n");
        printf("8. Kullanici Sil\n");
        printf("9. Kullanici Guncelle\n");
        printf("10. Istatistikleri Goruntule\n");
        printf("11. Gecikmis Kitaplari Listele\n");
        printf("12. CSV Olarak Aktar\n");
        printf("0. Cikis\n");
        printf("Seciminiz: ");
        
        while(scanf("%d", &choice) != 1) {
            printf(ERROR_COLOR "Hata: Sayi girmelisiniz!\n" COLOR_RESET);
            while(getchar() != '\n');
            printf("Seciminiz: ");
        }

        switch (choice) {
            case 1: addBook(); break;
            case 2: listBooks(); break;
            case 3: deleteBook(); break;
            case 4: updateBook(); break;
            case 5: searchBook(); break;
            case 6: addUser(); break;
            case 7: listUsers(); break;
            case 8: deleteUser(); break;
            case 9: updateUser(); break;
            case 10: showStatistics(); break;
            case 11: checkOverdueBooks(); break;
            case 12: exportToCSV("", 0); break;
            case 0: break;
            default: printf(ERROR_COLOR "Gecersiz secim!\n" COLOR_RESET); Sleep(1033);
        }
    } while (choice != 0);
}
//personel paneli
void staffMenu() {
    int choice;
    do {
        system("cls");
        printf(COLOR_PURPLE "\n=== PERSONEL PANELI ===\n" COLOR_RESET);
        printf("1. Kitap Ara\n");
        printf("2. Kitaplari Listele\n");
        printf("3. Kitap Odunc Al\n");
        printf("4. Kitap Iade Et\n");
        printf("5. Odunc Aldigim Kitaplar\n");
        printf("0. Cikis\n");
        printf("Seciminiz: ");
        
        while(scanf("%d", &choice) != 1) {
            printf(ERROR_COLOR "Hata: Sayi girmelisiniz!\n" COLOR_RESET);
            while(getchar() != '\n');
            printf("Seciminiz: ");
        }

        switch (choice) {
            case 1: searchBook(); break;
            case 2: listBooks(); break;
            case 3: borrowBook(); break;
            case 4: returnBook(); break;
            case 5: viewBorrowed(); break;
            case 0: break;
            default: printf(ERROR_COLOR "Gecersiz secim!\n" COLOR_RESET); Sleep(1033);
        }
    } while (choice != 0);
}
//öğrenci paneli
void studentMenu() {
    int choice;
    do {
        system("cls");
        printf(COLOR_PURPLE "\n=== OGRENCI PANELI ===\n" COLOR_RESET);
        printf("1. Kitap Ara\n");
        printf("2. Kitaplari Listele\n");
        printf("3. Kitap Odunc Al\n");
        printf("4. Kitap Iade Et\n");
        printf("5. Odunc Aldigim Kitaplar\n");
        printf("0. Cikis\n");
        printf("Seciminiz: ");
        
        while(scanf("%d", &choice) != 1) {
            printf(ERROR_COLOR "Hata: Sayi girmelisiniz!\n" COLOR_RESET);
            while(getchar() != '\n');
            printf("Seciminiz: ");
        }

        switch (choice) {
            case 1: searchBook(); break;
            case 2: listBooks(); break;
            case 3: borrowBook(); break;
            case 4: returnBook(); break;
            case 5: viewBorrowed(); break;
            case 0: break;
            default: printf(ERROR_COLOR "Gecersiz secim!\n" COLOR_RESET); Sleep(1033);
        }
    } while (choice != 0);
}
//giriş paneli
void loginMenu() {
    int attempts = 0;
    char username[50];
    char password[50];
    char hashedPassword[50];
    int ch;

    while (attempts < 3) {
        system("cls");
        printAsciiArt();
        printf(COLOR_PURPLE "\n=== KULLANICI GIRISI ===\n" COLOR_RESET);
        printf("(Cikmak icin kullanici adi yerine 0 girin)\n");
        printf("Kullanici Adi: ");
        scanf("%49s", username);
        
        if (strcmp(username, "0") == 0) {
            printf("\nProgramdan cikiliyor...\n");
            exit(0);
        }

        printf("Sifre: ");
        int i = 0;
        while (1) {
            ch = getch();
            if (ch == 13) {
                password[i] = '\0';
                break;
            } else if (ch == 8) {
                if (i > 0) {
                    i--;
                    printf("\b \b");
                }
            } else {
                password[i] = ch;
                i++;
                printf("*");
            }
        }
        printf("\n");

        simpleHash(password, hashedPassword);

        int userIndex = checkCredentials(username, hashedPassword);
        if (userIndex != -1) {
            currentUser = &users[userIndex];
            printf(SUCCESS_COLOR "\nGiris basarili! Hos geldiniz, %s!\n" COLOR_RESET, currentUser->username);
            Sleep(1033);

            if (strcmp(currentUser->role, "admin") == 0) {
                adminMenu();
            } else if (strcmp(currentUser->role, "personel") == 0) {
                staffMenu();
            } else {
                studentMenu();
            }
            return;
        } else {
            attempts++; //3 hatalı giriş denemesinden sonra program kapatılsın
            printf(ERROR_COLOR "\nHatali kullanici adi veya sifre! Kalan deneme hakkiniz: %d\n" COLOR_RESET, 3 - attempts);
            Sleep(1533);
        }
    }

    printf(ERROR_COLOR "\nCok fazla basarisiz giris denemesi. Program kapatiliyor.\n" COLOR_RESET);
    exit(0);
}

// kullanıcı verilerini ve kitapları başlatma fonksiyonu
void initSystem() {
    srand(time(NULL));
    if (numUsers == 0) {
        User admin;
        admin.id = 1;
        strcpy(admin.username, "admin");
        simpleHash("admin123", admin.password);
        strcpy(admin.role, "admin");
        admin.numBorrowed = 0;
        memset(admin.borrowDates, 0, sizeof(admin.borrowDates));
        memset(admin.returnDates, 0, sizeof(admin.returnDates));
        users[numUsers++] = admin;
        saveData();
    }
}