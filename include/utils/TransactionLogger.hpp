#pragma once
#include <string>
#include <vector>

class TransactionLogger {
    private:
    std::vector<std::string> logs; 

    public:
    TransactionLogger() {};
    ~TransactionLogger() {};

    // Tambah entri log baru
    void addLog(const std::string& message);

    // Mencetak semua log, atau N entri terakhir jika limit > 0
    void printLog(int limit = -1) const;

    // Hapus semua log
    void clearLog();

    // Mengeluarkan full log ke format string buat disimpen ke save file
    std::string exportState() const;
};