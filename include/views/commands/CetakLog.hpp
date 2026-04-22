#pragma once
#include "Command.hpp"
#include "IGameAction.hpp"
#include <string>

// CETAK_LOG [<limit>]
class CetakLogCommand : public Command {
    private:
    int limit;
    
    public:
    explicit CetakLogCommand(int limit = -1) : limit(limit) {}

    void execute(IGameAction& game) override {
        // TODO: panggil fungsi cetak log yang ada di kelas Game
        // Misal: game.cetakLog(limit);
    }
    
    std::string getName() const override { 
        return "CETAK_LOG"; 
    }
};