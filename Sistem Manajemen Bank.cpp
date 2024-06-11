#include <iostream>
#include <vector>
#include <sstream>
#include <ctime>
#include <fstream>
#include <queue>
#include <stack>

#ifdef _WIN32
#define CLEAR_SCREEN "cls"
#else
#define CLEAR_SCREEN "clear"
#endif

using namespace std;

class RekeningBank {
private:
    string nama;
    int nomorRekening;
    double saldo;
    vector<string> transaksi;
public:
    RekeningBank(string n, int nr, double s) : nama(n), nomorRekening(nr), saldo(s) {}

    RekeningBank() : nama(""), nomorRekening(0), saldo(0.0) {}

    void setNama(string n) {
        nama = n;
    }
    void setSaldo(double newSaldo) {
        saldo = newSaldo;
    }
    string getNama() {
        return nama;
    }
    int getNomorRekening() {
        return nomorRekening;
    }
    double getSaldo() {
        return saldo;
    }

    void simpanTransaksi(string tipeTransaksi, const double uang) {
        time_t now;
        time(&now);
        ostringstream oss;
        oss << "Waktu: " << ctime(&now) << " | " << "Jenis Transaksi: " << tipeTransaksi << " | " << "Jumlah Uang: " << uang << " | " << "Saldo: " << saldo;
        string transaksi = oss.str();
        this->transaksi.push_back(transaksi);
    }

    void tampilkanTransaksi() {
        if (!transaksi.empty()) {
            cout << "\t\t******* Info Transaksi *******\n";
            cout << "\t\tNomor Rekening: " << nomorRekening << endl;
            for (int i = 0; i < transaksi.size(); i++) {
                cout << "\t\tNo " << i + 1 << " ==> " << transaksi[i] << endl;
            }
            cout << "\t\t*******************************\n";
        } else {
            cout << "Tidak ada transaksi yang ditemukan\n";
        }
    }

    void setorUang(double uang) {
        saldo += uang;
        cout << " Setoran Berhasil. Saldo Baru: Rp. " << saldo << endl;
        this->simpanTransaksi("SETOR", uang);
    }

    void tarikUang(double uang) {
        if (uang <= saldo) {
            saldo -= uang;
            this->simpanTransaksi("TARIK", uang);
            cout << " Penarikan Berhasil. Saldo Baru: Rp. " << saldo << endl;
        } else {
            cout << "Saldo Tidak Cukup.\n";
        }
    }

    void lihatSaldo() {
        cout << "Saldo saat ini: Rp. " << saldo << endl;
    }

    vector<string> getTransaksi() {
        return transaksi;
    }

    void simpanKeStream(ofstream &file) {
        file << nama << endl;
        file << nomorRekening << endl;
        file << saldo << endl;
        file << transaksi.size() << endl;
        for (string &t : transaksi) {
            file << t << endl;
        }
    }

    void muatDariStream(ifstream &file) {
        file >> ws; 
        getline(file, nama);
        file >> nomorRekening;
        file >> saldo;
        int jumlahTransaksi;
        file >> jumlahTransaksi;
        file.ignore(); 
        transaksi.clear();
        for (int i = 0; i < jumlahTransaksi; ++i) {
            string t;
            getline(file, t);
            transaksi.push_back(t);
        }
    }
};

class ManajemenBank {
private:
    vector<RekeningBank> akun;
    queue<string> transaksiTertunda;
    stack<string> transaksiStack; 

public:
    void tambahAkun(string nama, int nomorRekening, double saldo) {
        akun.push_back(RekeningBank(nama, nomorRekening, saldo));
    }

    void tampilkanAkun(int i) {
        cout << "Nama: " << akun[i].getNama() << "     Nomor Rekening: " << akun[i].getNomorRekening() 
             << "  Saldo: Rp. " << akun[i].getSaldo() << endl;
        cout << "\t\t*****INFO*****" << endl;
        cout << "\t\tNama: " << akun[i].getNama() << endl;
        cout << "\t\tNomor Rekening: " << akun[i].getNomorRekening() << endl;
        cout << "\t\tSaldo: Rp. " << akun[i].getSaldo() << endl;
        cout << "\t\t**************" << endl;
    }

    void tampilkanSemuaAkun() {
        cout << "\t\tSemua Pemegang Akun" << endl;
        for (int i = 0; i < akun.size(); i++)
            tampilkanAkun(i);
    }

    int cariAkun(int nomorRekening) {
        for (int i = 0; i < akun.size(); i++) {
            if (akun[i].getNomorRekening() == nomorRekening)
                return i;
        }
        return -1;
    }

    RekeningBank* temukanAkun(int nomorRekening) {
        int ditemukan = this->cariAkun(nomorRekening);
        if (ditemukan >= 0) {
            return &akun[ditemukan];
        }
        return NULL;
    }

    void transferDana(int dariRekening, int keRekening, double jumlah) {
        RekeningBank* dariAkun = temukanAkun(dariRekening);
        RekeningBank* keAkun = temukanAkun(keRekening);
        if (dariAkun && keAkun && dariAkun->getSaldo() >= jumlah) {
            dariAkun->tarikUang(jumlah);
            keAkun->setorUang(jumlah);
            cout << "Transfer berhasil. Saldo baru: Rp. " << dariAkun->getSaldo() << endl;
            string transaksi = "Transfer: " + to_string(jumlah) + " dari " + to_string(dariRekening) + " ke " + to_string(keRekening);
            transaksiTertunda.push(transaksi);
            transaksiStack.push(transaksi); 
        } else {
            cout << "Transfer gagal. Periksa saldo atau nomor rekening.\n";
        }
    }

    void konfirmasiTransaksi() {
        if (!transaksiTertunda.empty()) {
            string transaksi = transaksiTertunda.front();
            transaksiTertunda.pop();
            cout << "Transaksi dikonfirmasi: " << transaksi << endl;
        } else {
            cout << "Tidak ada transaksi yang perlu dikonfirmasi.\n";
        }
    }

    void simpanAkunKeFile() {
        ofstream file("akun.txt");
        if (file.is_open()) {
            file << akun.size() << endl;
            for (RekeningBank& account : akun) {
                account.simpanKeStream(file);
            }
            file.close();
            cout << "Data akun disimpan ke file.\n";
        } else {
            cout << "Gagal menyimpan ke file.\n";
        }
    }

    void muatAkunDariFile() {
        ifstream file("akun.txt");
        if (file.is_open()) {
            int jumlahAkun;
            file >> jumlahAkun;
            for (int i = 0; i < jumlahAkun; ++i) {
                RekeningBank account;
                account.muatDariStream(file);
                akun.push_back(account);
            }
            file.close();
        } else {
            cout << "Gagal membuka file.\n";
        }
    }
};

int main() {
    ManajemenBank bank;
    bank.muatAkunDariFile();
    int pilihan;
    char opsi;

    do {
        system(CLEAR_SCREEN);
        cout << "\t\tSistem Manajemen Bank" << endl;
        cout << "\t\t---- Menu Utama ----" << endl;
        cout << "\t\t1. Buat Akun Baru " << endl;
        cout << "\t\t2. Tampilkan Semua Akun " << endl;
        cout << "\t\t3. Cari Akun " << endl;
        cout << "\t\t4. Setor Uang" << endl;
        cout << "\t\t5. Tarik Uang" << endl;
        cout << "\t\t6. Tampilkan Transaksi" << endl;
        cout << "\t\t7. Transfer Dana" << endl;
        cout << "\t\t8. Konfirmasi Transaksi" << endl;
        cout << "\t\t9. Keluar" << endl;
        cout << "---------------------------------------------" << endl;
        cout << "\tMasukkan Pilihan Anda: ";
        cin >> pilihan;

        switch (pilihan) {
            case 1: {
                string nama;
                int nomorRekening;
                double saldo;
                cout << "\t\tMasukkan Nama: ";
                cin >> nama;
                cout << "\t\tMasukkan Nomor Rekening: ";
                cin >> nomorRekening;
                cout << "\t\tMasukkan Saldo Awal: Rp. ";
                cin >> saldo;
                bank.tambahAkun(nama, nomorRekening, saldo);
                cout << "\t\t Akun Berhasil Dibuat.\n";
                break;
            }
            case 2: {
                bank.tampilkanSemuaAkun();
                break;
            }
            case 3: {
                int num;
                int ditemukan;
                cout << "\t\tMasukkan Nomor Rekening: ";
                cin >> num;
                ditemukan = bank.cariAkun(num);
                if (ditemukan >= 0)
                    bank.tampilkanAkun(ditemukan);
                else
                    cout << "Akun tidak ditemukan.\n";
                break;
            }
            case 4: {
                int num;
                double uang;
                cout << "\t\tMasukkan Nomor Rekening: ";
                cin >> num;
                RekeningBank* akun = bank.temukanAkun(num);
                if (akun != NULL) {
                    cout << "\t\tMasukkan Jumlah Uang untuk Disetor: Rp. ";
                    cin >> uang;
                    akun->setorUang(uang);
                } else {
                    cout << "Akun tidak ditemukan.\n";
                }
                break;
            }
            case 5: {
                int num;
                double uang;
                cout << "\t\tMasukkan Nomor Rekening: ";
                cin >> num;
                RekeningBank* akun = bank.temukanAkun(num);
                if (akun != NULL) {
                    cout << "\t\tMasukkan Jumlah Uang untuk Ditarik: Rp. ";
                    cin >> uang;
                    akun->tarikUang(uang);
                } else {
                    cout << "Akun tidak ditemukan.\n";
                }
                break;
            }
            case 6: {
                int num;
                cout << "\t\tMasukkan Nomor Rekening: ";
                cin >> num;
                RekeningBank* akun = bank.temukanAkun(num);
                if (akun != NULL) {
                    akun->tampilkanTransaksi();
                } else {
                    cout << "Akun tidak ditemukan.\n";
                }
                break;
            }
            case 7: {
                int dariRekening, keRekening;
                double jumlah;
                cout << "\t\tMasukkan Nomor Rekening Anda: ";
                cin >> dariRekening;
                cout << "\t\tMasukkan Nomor Rekening Tujuan: ";
                cin >> keRekening;
                cout << "\t\tMasukkan Jumlah Uang untuk Ditransfer: Rp: ";
                cin >> jumlah;
                bank.transferDana(dariRekening, keRekening, jumlah);
                break;
            }
            case 8: {
                bank.konfirmasiTransaksi();
                break;
            }
            case 9: {
                opsi = 'n';
                bank.simpanAkunKeFile();
                break;
            }
        }

        if (pilihan != 9) {
            cout << "\t\t Apakah Anda ingin melanjutkan [Y/T]? ";
            cin >> opsi;
        }

    } while (opsi == 'y' || opsi == 'Y');

    return 0;
}

