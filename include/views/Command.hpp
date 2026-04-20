#pragma once
#include <string>
#include <vector>

using namespace std;

// Forward declaration biar ga compile 
class IGameAction;

// Setiap aksi pengguna direpresentasikan sebagai objek Command
// execute() dipanggil oleh Game lewat CommandHandler
class Command {
public:
    virtual ~Command() {}
    virtual void execute(IGameAction& game) = 0;
    virtual string getName() const = 0;
};