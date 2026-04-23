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

    bool execute(IGameAction& game) override {
        game.cetakLog(limit);
        return false;
    }
    
    std::string getName() const override { 
        return "CETAK_LOG"; 
    }
};