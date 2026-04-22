#pragma once
#include <string>

// Forward declaration
class IGameAction;

// Setiap aksi pengguna direpresentasikan sebagai objek Command
// execute() dipanggil oleh Game lewat CommandHandler
class Command {
public:
    virtual ~Command() {}

    // True kalau giliran habis (ganti pemain)
    virtual bool execute(IGameAction& game) = 0;

    // Bisa dipake TransactionLogger buat nyatet nama perintah
    virtual std::string getName() const = 0;
};