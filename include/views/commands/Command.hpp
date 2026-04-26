#pragma once
#include <string>

// Forward declaration
class IGameAction;

// Setiap aksi pengguna direpresentasikan sebagai objek Command
class Command {
public:
    virtual ~Command() {}

    // Dipanggil sama masing-masing command
    // True kalau game selesai
    virtual bool execute(IGameAction& game) = 0;

    // Bisa dipake TransactionLogger buat nyatet nama perintah
    virtual std::string getName() const = 0;
};