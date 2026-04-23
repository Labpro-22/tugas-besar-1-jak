#include "TransactionLogger.hpp"
#include <iostream>

void TransactionLogger::addLog(const std::string& message) {
    logs.push_back(message);
}

void TransactionLogger::printLog(int limit) const {
    if (logs.empty()) {
        std::cout << "Belum ada log.\n";
        return;
    }

    // Nentuin dari indeks mana mulai dicetak
    int startIdx = 0;
    if (limit > 0 && limit < (int)logs.size()) {
        startIdx = (int)logs.size() - limit;
        std::cout << "=== Log Transaksi (" << limit << " Terakhir) ===\n";
    } else {
        std::cout << "=== Log Transaksi Penuh ===\n";
    }

    for (int i = startIdx; i < (int)logs.size(); i++) {
        std::cout << logs[i] << "\n";
    }
}

void TransactionLogger::clearLog() {
    logs.clear();
}

std::string TransactionLogger::exportState() const {
    std::string result = std::to_string(logs.size()) + "\n";
    for (const std::string& log : logs) {
        result = result + log + "\n";
    }
    return result;
}