#include <iostream>
#include <string>
#include <ctime>
#include <stdlib.h>
#include <conio.h>
#include <bits/stdc++.h>
using namespace std;

const int MAX_LAUNDRY_ORDERS = 100;
const int MAX_PURCHASES = 100;

struct LaundryOrder { // untuk menyimpan pesanan
    string username;
    string serviceType;
    int weight;
    string orderDate;
    string orderTime;
    string status;
};

struct User { // untuk menyimpan data user
    string username;
    string password;
    User *next;
};

struct Purchase { // untuk menyimpan detail pembelian/pembayaran
    string username;
    string item;
    double price;
    string purchaseDate;
    string purchaseTime;
};

struct LaundryService { // untuk menyimpan servis dan harga
    string serviceType;
    double pricePerKilo;
};

struct Node { // linked list dari pesanan
    LaundryOrder order;
    Node* next;
};

LaundryService laundryServices[100] = { // servis dan harga yang tersedia di awal
    {"Cuci Biasa", 10.0},
    {"Cuci Kering", 15.0},
    {"Setrika", 5.0},
};

User *usersDatabase = NULL;
Node* laundryOrderList = NULL;
Node* laundryfinish = NULL;
Purchase purchases[MAX_PURCHASES];
int NUM_LAUNDRY_SERVICES = 3;

string currentUser = "";
const string ADMIN_USERNAME = "admin";
const string ADMIN_PASSWORD = "admin";

int userCount = 0;
int purchaseCount = 0;
int orderCount = 0;
int messageCount = 0;
bool adminlogin = false;
bool userlogin = false;

void clear() { //bersihkan terminal
    system("cls");
}

// Fungsi untuk swap dua node dalam linked list
void swapNodes(Node* a, Node* b) {
    LaundryOrder temp = a->order;
    a->order = b->order;
    b->order = temp;
}

string findname(User *head, int idx){ // bagian dari jump search
    for (int i = 0 ; i < idx ; i++){
        head = head->next;
    }
    return head->username;
}

int length(User *head){ // bagian dari jump search
    int count = 0;
    User *temp = head;
    while (temp != NULL) {
        count++;
        temp = temp->next;
    }
    return count;
}

int jumpSearchname(User *head, string x, int n){ // fungsi untuk formula/metode jump search
    if (head == NULL) {
        clear();
        return -1;
    }
    int step = sqrt(n);
    int prev = 0;
    int idx = min(step, n) - 1;
    while (findname(head, idx) < x){
        prev = step;
        step += sqrt(n);
        idx = min(step, n) - 1;
        if (prev >= n) {
            return -1;
        }
    }
    while (findname(head, prev) < x){
        prev++;
    }
    cout << endl;
    if (findname(head, prev) == x){
        return prev;
    }
    return -1;
}

// Fungsi untuk mengurutkan pesanan berdasarkan tanggal pesanan menggunakan algoritma merge Sort
// Fungsi untuk mengurutkan linked list dengan algoritma merge Sort
User *SortedMergeName(User *a, User *b){
    User *result = NULL;
    /* Base cases */
    if (a == NULL){
        return (b);
    }
    else if (b == NULL){
        return (a);
    }
    /* Pick either a or b, and recur */
    if (a->username <= b->username){
        result = a;
        result->next = SortedMergeName(a->next, b);
    }
    else{
        result = b;
        result->next = SortedMergeName(a, b->next);
    }
    return (result);
}

void FrontBackSplit(User *source,User **frontRef, User **backRef){ // bagian dari merge sort
    User *fast;
    User *slow;
    slow = source;
    fast = source->next;
    /* Advance 'fast' two Users, and advance 'slow' one User */
    while (fast != NULL){
        fast = fast->next;
        if (fast != NULL){
            slow = slow->next;
            fast = fast->next;
        }
    }
    /* 'slow' is before the midpoint in the list, so split it in two
    at that point. */
    *frontRef = source;
    *backRef = slow->next;
    slow->next = NULL;
}

void MergeSortName(User **headRef){ // fungsi merge sort
    User *head = *headRef;
    User *a;
    User *b;
    /* Base case -- length 0 or 1 */
    if ((head == NULL) || (head->next == NULL)){
        return;
    }
    /* Split head into 'a' and 'b' sublists */
    FrontBackSplit(head, &a, &b);
    /* Recursively sort the sublists */
    MergeSortName(&a);
    MergeSortName(&b);
    /* answer = merge the two sorted lists together */
    *headRef = SortedMergeName(a, b);
}

void transfer(int orderIndex) { //pemindahan data laundry dari orderlist ke finish
    Node* current = laundryOrderList;
    Node* previous = NULL;
    int currentIndex = 1;

    while (current != NULL) {
        if (currentIndex == orderIndex) {
            Node* newNode = new Node;
            newNode->order = current->order;
            newNode->next = laundryfinish;
            laundryfinish = newNode;
            if (previous != NULL) {
                previous->next = current->next;
            } else {
                laundryOrderList = current->next;
            }
            delete current;
            return;
        }
        previous = current;
        current = current->next;
        currentIndex++;
    }
}

void lihatDanUbahStatusPesanan() { //untuk admin dapat mengubah status pesanan
    if (!adminlogin) {
    	clear();
    	cout << "=====================================================================" << endl;
        cout << "||		 Anda tidak memiliki akses untuk mengubah status pesanan	||" << endl;
        cout << "=====================================================================" << endl;
        cout << "Press Enter to Back...";
        getche();
        return;
    }

    Node* current = laundryOrderList;
    int orderIndex = 1;
    if (current == NULL){
        clear();
        cout << "Pesanan kosong" << endl;
        cout << "Press Enter to Back...";
        getche();
        return;
    }
	
	clear();
    cout << "Daftar Pesanan:" << endl;
    while (current != NULL) {
        LaundryOrder& order = current->order;
        cout << "Nomor Pesanan " << orderIndex << "\n" " - Username: " << order.username << "\n" " - Status: " << order.status << "\n" << endl;
        current = current->next;
        orderIndex++;
    }

    int nomorPesanan = 1;

    if (nomorPesanan == 0) {
        return;
    }

    current = laundryOrderList;
    orderIndex = 1;

    while (current != NULL) {
        if (orderIndex == nomorPesanan) {
            while (true) {
                LaundryOrder& order = current->order;
                int newStatus;
                cout << "Pilih status untuk pesanan No. 1 " << endl;
                cout << "1. Menunggu." << endl;
                cout << "2. Diproses." << endl;
                cout << "3. Selesai." << endl;
                cout << "4. Kembali." << endl;
                cout << "Pilihan [1-4]: ";
                cin >> newStatus;
                if (cin.fail()) {
                    cin.clear();
                    cin.ignore();
                    newStatus = -1;
                }
                else if (newStatus == 1) {
                    clear();
                    order.status = "Menunggu"; // Perbarui status pesanan dalam linked list
                    cout << "Status pesanan berhasil diubah." << endl;
                    cout << "Press Enter to Next...";
                    getche();
                    return;
                }
                else if (newStatus == 2) {
                    clear();
                    order.status = "Diproses"; // Perbarui status pesanan dalam linked list
                    cout << "Status pesanan berhasil diubah." << endl;
                    cout << "Press Enter to Next...";
                    getche();
                    return;
                }
                else if (newStatus == 3) {
                    clear();
                    order.status = "Selesai"; // Perbarui status pesanan dalam linked list
                    cout << "Status pesanan berhasil diubah." << endl;
                    cout << "Press Enter to Next...";
                    getche();
                    transfer(orderIndex); //pindah ke laundryfinish
                    return;
                }
                else if (newStatus == 4) {
                    clear();
                    return;
                }
                else {
                    clear();
                    cout << "Input tidak valid." << endl;
                    getche();
                }
            }
        }
        current = current->next;
        orderIndex++;
    }
}

void lihatPesananAnda() { //untuk melihat pesanan user yang sesuai
    clear();
    if (currentUser.empty()) {
        clear();
        cout << "Silakan login terlebih dahulu." << endl;
        getche();
        return;
    }
    bool show = false;
    Node* current = laundryOrderList;
    if (laundryOrderList == NULL && laundryfinish == NULL) {
    	cout << "==========================" << endl;
        cout << "|| Pesanan masih kosong ||" << endl;
        cout << "==========================" << endl;
        getche();
        return;
    }
    else if (laundryOrderList == NULL) {
        show = false;
    }
		cout << "=====================================================\n";
        cout << "||                  PESANAN LAUNDRY ANDA           ||\n";
        cout << "=====================================================\n\n";    
    
    int orderIndex = 1;
    if (show == false) {
        while (current != NULL) {
            if (current->order.username == currentUser) {
                cout << "Nomor Pesanan " << orderIndex << "\n" " - Jenis Layanan: " << current->order.serviceType
                    << "\n" " - Berat: " << current->order.weight << " kg " "\n"  " - Tanggal Pesanan: " << current->order.orderDate
                    <<"\n" " - Waktu Pesanan: " << current->order.orderTime << "\n" " - Status: " << current->order.status << endl;
                orderIndex++;
            }
            
            current = current->next;
        }
    }

    current = laundryfinish;
    orderIndex = 1;
    if (current == NULL) {
        cout << endl;
        cout << "Press Enter to Back Menu...";
        getche();
        return;
    }
    cout << endl;
    cout << "Pesanan selesai: " << endl;
    while (current != NULL) {
        // const LaundryOrder& order = current->order; gak perlu, cek aja langsung di current dari pada bikin variabel baru
        if (current->order.username == currentUser) {
            cout << "Nomor Pesanan " << orderIndex << "\n" " - Jenis Layanan: " << current->order.serviceType
                 << "\n" " - Berat: " << current->order.weight << " kg " "\n"  " - Tanggal Pesanan: " << current->order.orderDate
                 <<"\n" " - Waktu Pesanan: " << current->order.orderTime << "\n" " - Status: " << current->order.status << endl;
            orderIndex++;
        }
        
        current = current->next;
    }
    getche();
    clear();
    return;
}

void daftarPengguna(string username, string password) { // untuk tambah akun ke database pengguna
    clear();
    User* current = usersDatabase;
    User* previous = NULL;

    while (current != NULL) {
        if (current->username == username) {
            clear();
            cout << "===========================" << endl ;
            cout << "||  Username sudah ada!  ||" << endl;
            cout << "===========================" << endl;
			cin.clear();
            cin.ignore();
            cin.get();
            return;
        }
        previous = current;
        current = current->next;
    }

    User* newUser = new User;
    newUser->username = username;
    newUser->password = password;
    newUser->next = NULL;

    if (previous != NULL) {
        previous->next = newUser;
    } else {
        usersDatabase = newUser;
    }

    userCount++;

    clear();
    cout << "==========SELAMAT==========" << endl;
    cout << "||  Registrasi berhasil  ||" << endl;
    cout << "===========================" << endl;
    cout << "Press Enter to Next...";
	getche();
    return;
}

string periksa(string username, string password) { // pengecekan input yang dimasukkan dengan database user
    if (username == "admin" && password == "admin") {
        return "admin";
    }
    
    User* current = usersDatabase;
    bool usernameExists = false;

    while (current != NULL) {
        if (current->username == username) {
            usernameExists = true;
            if (current->password == password) {
                clear();
                cout << "======================" << endl;
                cout << "||  Login berhasil  ||" << endl;
                cout << "======================" << endl;
                cout << "Press Enter to Next...";
                getche();
                return username;
            } else {
                clear();
                cout << "======================" << endl;
                cout << "|| Kata sandi salah ||" << endl;
                cout << "======================" << endl;
                getche();
                return "";
            }
        }
        current = current->next;
    }

    clear();

    if (usernameExists) {
        cout << "======================" << endl;
        cout << "|| Kata sandi salah ||" << endl;
        cout << "======================" << endl;
    } else {
        cout << "================================================================" << endl;
        cout << "||	                      				      ||" << endl;
        cout << "||  Pengguna tidak ditemukan atau Registrasi terlebih dahulu  ||" << endl;
        cout << "||			      				      ||" << endl;
        cout << "================================================================" << endl;
    }

    getche();
    return "";
}

void menuRegistrasi() {// menu membuat akun baru
    string username, password;
    	clear();
     	cout << "======================================================\n";
        cout << "||                  SELAMAT DATANG                  ||\n";
        cout << "||        SILAHKAN REGISTRASI TERLEBIH DAHULU       ||\n";
        cout << "======================================================\n\n";

        cout << "[Registrasi]\n\n";
        cout << "Username: ";
        cin >> username;
        cout << "Password: ";
        cin >> password;
    daftarPengguna(username, password);
}

void menuLogin() { // menu saat login/pengecekan akun
	string username, password;
    while (true) {
        clear();

        cout << "======================================================\n";
        cout << "||                  SELAMAT DATANG                  ||\n";
        cout << "||              DI JASA LAUNDRY COLLABS             ||\n";
        cout << "======================================================\n\n";
		
        cout << "[LOGIN]\n\n";
        cout << "Username: ";
    	cin >> username;
        cout << "Password: ";
        cin >> password;

        currentUser = periksa(username, password);
        if (username == ADMIN_USERNAME && password == ADMIN_PASSWORD) {
            clear();
            cout << "==========WELCOME ADMIN===========" << endl;
            cout << "||	Admin berhasil masuk!   ||" << endl;
            cout << "==================================" << endl;
        	cout << "Press Enter to Next...";
            currentUser = "admin";
            adminlogin = true;
            getche();
            return;
        } else if (!currentUser.empty()) {
            clear();
            cout << "=====================" << endl;
            cout << "|| Berhasil masuk! ||" << endl;
            cout << "=====================" << endl;
            cout << "Press Enter to Next...";
            userlogin = true;
            getche();
            return;
        } else {
            clear();
            cout << "		================================" << endl;
            cout << "		||   Silahkan Login Kembali!  ||" << endl;
            cout << "		================================" << endl;
            cout << "Press Enter to Back...";
            getche();
            return;
            }
	}
}

void buatPesananLaundry() { // bikin pesanan baru
    clear();
    if (currentUser.empty()) {
        cout << "Silakan login terlebih dahulu." << endl;
        getche();
        return;
    }

    cout << "Daftar Harga Laundry Per Kilo: \n" << endl;
    for (int i = 0; i < NUM_LAUNDRY_SERVICES; i++) {
        cout << i + 1 << ". " << laundryServices[i].serviceType << " - Rp" << laundryServices[i].pricePerKilo << " per kilo" << endl;
    }

    int selectedService;
    double totalHarga = 0.0;
    double berat;

    cout << "\nPilih jenis layanan laundry: ";
    cin >> selectedService;
    if (cin.fail()) {
            cin.clear();
            cin.ignore();
            selectedService = -1;
    }
    if (selectedService < 1 || selectedService > NUM_LAUNDRY_SERVICES) {
        cout << "Pilihan layanan tidak valid. Silakan pilih yang benar." << endl;
        return;
    }
	
    cout << "\nMasukkan berat (kg): ";
    cin >> berat;
    if (cin.fail()) {
            cin.clear();
            cin.ignore();
            berat = -1;
        }

    if (berat <= 0) {
        cout << "Berat tidak valid." << endl;
        return;
    }

    totalHarga = laundryServices[selectedService - 1].pricePerKilo * berat;

    if (orderCount >= MAX_LAUNDRY_ORDERS) {
        cout << "Basis data pesanan laundry penuh." << endl;
        return;
    }

    time_t currentTime = time(0);
    tm* localTime = localtime(&currentTime);

    char orderDate[11];
    char orderTime[9];
    strftime(orderDate, sizeof(orderDate), "%d/%m/%Y", localTime);
    strftime(orderTime, sizeof(orderTime), "%H:%M:%S", localTime);

    cout << "\nPesanan laundry berhasil dibuat dengan total harga Rp " << totalHarga << endl;

    while (true) {
        cout << "\nBeli pesanan laundry ini? (y/n): ";
        string buyOption;
        cin >> buyOption;
        if (cin.fail()) {
            cin.clear();
            cin.ignore();
            buyOption = "";
        }
        if (buyOption == "y" || buyOption == "Y") {
            // Menambahkan pesanan
            Node* newNode = new Node();
            newNode->order.username = currentUser;
            newNode->order.serviceType = laundryServices[selectedService - 1].serviceType;
            newNode->order.weight = berat;
            newNode->order.orderDate = orderDate;
            newNode->order.orderTime = orderTime;
            newNode->order.status = "Menunggu";
            newNode->next = laundryOrderList;
            laundryOrderList = newNode;
            orderCount++;

            // Menambahkan pembelian/pembayaran
            Purchase& purchase = purchases[purchaseCount];
            purchase.username = currentUser;
            purchase.item = "Laundry (" + laundryServices[selectedService - 1].serviceType + ")";
            purchase.price = totalHarga;
            purchase.purchaseDate = orderDate;
            purchase.purchaseTime = orderTime;
            purchaseCount++;
            clear();
            cout << "|==========================|";
            cout << "\n|Pembelian berhasil dibuat!|" << endl;
            cout << "|==========================|";
            cout << "\nPress Enter to Next...";
            getche();
            return;
        } else if (buyOption == "n" || buyOption == "N") {
            return;
        } else {
            cout << "\nPilihan tidak valid. Harap masukkan 'y' atau 'n'." << endl;
            getche();
        }
    }
}


void displayAllUsers() { // tampilkan semua akun yang dibuat
    clear();
    User* current = usersDatabase;

    if (current == NULL) {
    	cout << "==============================" << endl;
        cout << "||  Daftar pengguna kosong  ||" << endl;
        cout << "==============================" << endl;
        cout << "Press Enter to Back...";
        getche();
        clear();
        return;
    }

    cout << "Daftar Pengguna setelah sorting:" << endl;
    while (current != NULL) {
        cout << "Username: " << current->username << endl;
        current = current->next;
    }
    getche();
    return;
}

void searchmenu() { // menu pencarian
    clear();
    cout << "Masukkan nama user yang ingin di cari: ";
    string cari;
    cin >> cari;
    int result = jumpSearchname(usersDatabase, cari, length(usersDatabase));
    if (result == -1) {
        clear();
        cout << "============================" << endl;
        cout << "||  Nama tidak ditemukan  ||" << endl;
        cout << "============================" << endl;
        cout << "Press Enter to Back...";
        getche();
        clear();
        return;
    }
    else {
        clear();
        cout << "Username " << cari << " ditemukan di nomor " << result;
        getche();
        return;
    }
}

void menuPengelola() { // menu untuk melihat/urut/cari dari data akun
    while (true) {
        clear();
        cout << "Menu Pengelolaan Pengguna" << endl;
        cout << "1. Lihat Semua Pengguna\n2. Urutkan Pengguna\n3. Cari Pengguna\n4. Kembali ke Menu Utama" << endl;
        cout << "Pilihan [1-4]: ";
        int adminMenuPilihan;
        cin >> adminMenuPilihan;
        if (cin.fail()) {
            cin.clear();
            cin.ignore();
            adminMenuPilihan = -1;
        }
        switch (adminMenuPilihan) {
            case 1:
                displayAllUsers();
                break;
            case 2:
                clear();
                MergeSortName(&usersDatabase);
                cout << "Pengurutan pengguna selesai." << endl;
                getche();
                clear();
                break;
            case 3:
                searchmenu();
                break;
            case 4:
                return; // Kembali ke menu admin
            default:
                clear();
                cout << "Pilihan tidak valid!" << endl;
                getche();
                break;
        }
    }
}

void batalkanPesananLaundry() { //batal/hapus pesanan untuk user
    clear();
    if (currentUser.empty()) {
        cout << "Silakan login terlebih dahulu." << endl;
        getche();
        return;
    }

    cout << "Masukkan nomor pesanan yang ingin dibatalkan: ";
    int nomorPesanan;
    cin >> nomorPesanan;

    if (nomorPesanan < 1 || nomorPesanan > orderCount) {
        cout << "Nomor pesanan tidak valid." << endl;
        getche();
        return;
    }


    Node *temp = NULL;
    Node *current = laundryOrderList;
    if (current == NULL) {
        cout << "Pesanan kosong" << endl;
        getche();
        return;
    }
    int order = 1;
    while (current != NULL) {
        if (current->order.username == currentUser) {
            if (order == nomorPesanan) {
                if (current->order.status == "Menunggu") {
                    if (temp != NULL) {
                        temp->next = current->next;
                    } else {
                        laundryOrderList = current->next;
                    }
                    delete current;
                    orderCount--;
                    clear();
                    cout << "===================================";
                    cout << "\nPesanan dibatalkan dengan berhasil." << endl;
                    cout << "===================================";
                    cout << "\nPress Enter to Back...";
                    getche();
                    return;
                } else {
                    clear();
                    cout << "Tidak dapat membatalkan pesanan. Pesanan sudah dalam proses atau selesai." << endl;
                    getche();
                    return;
                }
            }
            order++;
        }
        temp = current;
        current = current->next;
    }

    cout << "Pesanan tidak ditemukan." << endl;
    getche();
    return;
}


void lihatMenuUser() { //menu utama user
	clear();
    while (true) {
    	clear();
        cout << "========== SELAMAT DATANG " << currentUser << " ============""\n";
        cout << "||   	  1. Buat Pesanan Laundry         ||" << endl;
        cout << "||	  2. Lihat Pesanan Anda		  ||" << endl;
        cout << "||	  3. Batalkan Pesanan Anda	  ||" << endl;
        cout << "||	  4. Keluar		          ||" << endl;
        cout << "============================================" << endl;
		cout << "Pilihan [1-4]: ";
        int pilihan;
        cin >> pilihan;
        if (cin.fail()) {
            cin.clear();
            cin.ignore();
            pilihan = -1;
        }
        switch (pilihan) {
            case 1:
                buatPesananLaundry();
                break;
            case 2:
                lihatPesananAnda();
                break;
            case 3:
                batalkanPesananLaundry();
                break;
            case 4:
                currentUser = "";
                cout << "Berhasil keluar." << endl;
                userlogin = false;
                return;
            default:
            	cout << "											"<< endl;
            	cout << "==========================================" << endl;
                cout << "|| Silahkan pilih sesuai nomor di atas! ||" << endl;
                cout << "==========================================" << endl;
                cout << "Enter..." << endl;
				getche();
                break;
        }
        
    }
}

void enqueueLaundryOrder(LaundryOrder newOrder) { //tambah pesanan baru yang kosong
    Node* newNode = new Node();
    newNode->order = newOrder;
    newNode->next = NULL;

    if (laundryOrderList == NULL) {
        laundryOrderList = newNode;
    } else {
        Node* temp = laundryOrderList;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
}

void displayFinishedLaundryOrders() { //tampilkan semua order yang selesai oleh semua user
    clear();

    int orderIndex = 1;
    Node* current = laundryOrderList;

    if (laundryfinish == NULL) {
        cout << "Tidak ada pesanan dengan status 'Selesai'" << endl;
        return;
    }

    orderIndex = 1;
    current = laundryfinish;
    cout << "Pesanan dengan status Selesai : \n" << endl;
    while (current != NULL) {
        const LaundryOrder& order = current->order;
        cout << "Nomor Pesanan: " << orderIndex << "\n" " - Username: " << order.username << "\n" " - Jenis Layanan: "
             << order.serviceType << "\n" " - Berat: " << order.weight << " kg" "\n" "- Tanggal Pesanan: " << order.orderDate
             << "\n" " - Waktu Pesanan: " << order.orderTime << "\n" " - Status: " << order.status << endl;
        current = current->next;
        orderIndex++;
    }
    cout << endl;
    cout << "Press Enter to Next...";
    getche();
    return;
}

void dequeueLaundryOrder() { // untuk hapus data pesanan dari laundryfinish
    clear();
    if (laundryfinish == NULL) {
        cout << "Queue pesanan laundry kosong." << endl;
        getche();
        return;
    }

    int nomorPesanan;
    displayFinishedLaundryOrders();
    cout << "Masukkan nomor pesanan yang ingin dihapus: ";
    cin >> nomorPesanan;
    if (cin.fail()) {
        cin.clear();
        cin.ignore();
        nomorPesanan = -1;
    }

    if (nomorPesanan == 1) {
        Node* temp = laundryfinish;
        laundryfinish = laundryfinish->next;
        delete temp;
        clear();
        cout << "============================================";
        cout << "\nPesanan laundry berhasil dihapus dari queue." << endl;
        cout << "============================================";
        cout << "\nPress Enter to Back...";
        getche();
        return;
    } else if (nomorPesanan < 1) {
        clear();
        cout << "Nomor pesanan tidak valid." << endl;
        getche();
        return;
    } else {
        int index = 1;
        Node* current = laundryfinish;
        Node* previous = NULL;
        while (current != NULL && index != nomorPesanan) {
            previous = current;
            current = current->next;
            index++;
        }

        if (current == NULL) {
            cout << "Nomor pesanan tidak valid." << endl;
            getche();
            return;
        }

        previous->next = current->next;
        delete current;
        cout << "Pesanan laundry berhasil dihapus dari queue." << endl;
        getche();
        clear();
        return;
    }
}

void displayAllLaundryOrders() { //tampilkan semua order yang dibuat oleh semua user
    clear();
    bool show = false;
    int orderIndex = 1;
    Node* current = laundryOrderList;
    if (laundryOrderList == NULL && laundryfinish == NULL) {
        clear();
        cout << "Queue pesanan laundry kosong." << endl;
        getche();
        return;
    }
    else if (laundryOrderList == NULL) {
        show = false;
    }

    if (show == false) {
        cout << "Pesanan dengan status Menunggu/Diproses : \n" << endl;
        while (current != NULL) {
            const LaundryOrder& order = current->order;
            cout << "Nomor Pesanan " << orderIndex << "\n" " - Username: " << order.username << "\n" " - Jenis Layanan: "
                << order.serviceType << "\n" " - Berat: " << order.weight << " kg" "\n" "- Tanggal Pesanan: " << order.orderDate
                << "\n" " - Waktu Pesanan: " << order.orderTime << "\n" " - Status: " << order.status << "\n" << endl;
            current = current->next;
            orderIndex++;
        }
    }

    if (laundryfinish == NULL) {
        cout << "Tidak ada pesanan dengan status 'Selesai'" << endl;
        cout << "\nPress Enter to Next...";
        getche();
        return;
    }

    orderIndex = 1;
    current = laundryfinish;
    cout << endl;
    cout << "Pesanan dengan status Selesai : \n" << endl;
    while (current != NULL) {
        const LaundryOrder& order = current->order;
        cout << "Nomor Pesanan: " << orderIndex << "\n" " - Username: " << order.username << "\n" " - Jenis Layanan: "
             << order.serviceType << "\n" " - Berat: " << order.weight << " kg" "\n" "- Tanggal Pesanan: " << order.orderDate
             << "\n" " - Waktu Pesanan: " << order.orderTime << "\n" " - Status: " << order.status << "\n" << endl;
        current = current->next;
        orderIndex++;
    }
    cout << "\nPress Enter to Back Menu...";
    getche();
    clear();
    return;
}

void tambahservis() { // untuk tambah jenis dan harga servis baru
    clear();
    string newServiceType;
    double newPricePerKilo;
    int a;
    while (true) {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Masukkan nama jenis servis baru: ";
        getline(cin, newServiceType);
        cout << "Masukkan harga per kilo servis baru: ";
        cin >> a;
        newPricePerKilo = a;
        if (cin.fail()) {
            cout << "Mohon masukkan angka." << endl;
            cin.clear();
            cin.ignore();
        }
        if (newPricePerKilo >= 1 && newPricePerKilo <= 100) {
            break;
        }
        else {
            cout << "Harga servis tidak valid" << endl;
        }
    }
    while (true) {
        clear();
        cout << "Apakah anda yakin ingin menambah: " << endl;
        cout << "Jenis servis baru: " << newServiceType << endl;
        cout << "Harga per kilo servis: " << newPricePerKilo << endl;
        cout << "[Y/N] : ";
        string confirm;
        cin >> confirm;
        if (confirm == "y" || confirm == "Y") {
            laundryServices[NUM_LAUNDRY_SERVICES++] = {newServiceType, newPricePerKilo};
            return;
        }
        else if (confirm == "n" || confirm == "N") {
            return;
        }
        else{
            cout << "Pilihan tidak valid" << endl;
            getche();
        } 
    }
}

void lihatMenuAdmin() { //menu utama admin
    while (true) {
    	clear();
    	cout << "======================================================\n";
        cout << "||                  SELAMAT DATANG                  ||\n";
        cout << "||              ADMIN LAUNDRY COLLABS               ||\n";
        cout << "======================================================\n\n";
        cout << "1. Lihat Semua Pesanan\n2. Lihat dan Ubah Status Pesanan\n3. Hapus Pesanan dengan status selesai\n4. Menu Display User\n5. Tambah Akhir Jenis Servis \n6. Keluar" << endl;
        cout << "Pilihan [1-6]: ";
        int pilihan;
        cin >> pilihan;
        if (cin.fail()) {
            cin.clear();
            cin.ignore();
            pilihan = -1;
        }
        switch (pilihan) {
            case 1:
                displayAllLaundryOrders();
                break;
            case 2:
                lihatDanUbahStatusPesanan();
                break;
            case 3:
                dequeueLaundryOrder();
                break;
            case 4:
                menuPengelola();
                break;
            case 5:
                tambahservis();
                break;
            case 6:
                currentUser = "";
                cout << "Berhasil keluar." << endl;
                adminlogin = false;
                return;
            default:
				cout << "											"<< endl;
            	cout << "==========================================" << endl;
                cout << "|| Silahkan pilih sesuai nomor di atas! ||" << endl;
                cout << "==========================================" << endl;
				cout << "Enter..." << endl;
				getche();
                break;
        }
    }
}



int main() {
    while (true) {
        clear();
        cout << "====SELAMAT DATANG DI JASA LAUNDRY====" << endl;
        cout << "||	     1. Registrasi	    ||" << endl;
        cout << "||	     2. Login		    ||" << endl;
        cout << "||	     3. Keluar		    ||" << endl;
        cout << "======================================" << endl;
        cout << "Pilihan [1-3]: ";
        int pilihan;
        cin >> pilihan;
        if (cin.fail()) {
            cin.clear();
            cin.ignore();
            pilihan = -1;
        }
        switch (pilihan) {
            case 1:
                menuRegistrasi();
                break;
            case 2:
                menuLogin();
                if (userlogin) {
                    lihatMenuUser();
                } else if (adminlogin) {
                    lihatMenuAdmin();
                } else {
                    break;
                }
                break;
            case 3:
                return 0;
            default:
            	cout << "											"<< endl;
            	cout << "==========================================" << endl;
                cout << "|| Silahkan pilih sesuai nomor di atas! ||" << endl;
                cout << "==========================================" << endl;
				getche();
                break;
        }
    }

    return 0;
}
