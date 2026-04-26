#pragma once
#include "Command.hpp"
#include "core/IGameAction.hpp"
#include <string>

// SIMPAN <filename>
class SimpanCommand : public Command {
    private:
    std::string filename;
    
    public:
    explicit SimpanCommand(std::string filename) : filename(std::move(filename)) {}

    bool execute(IGameAction& game) override {
        // Panggil fungsi simpan game yang ada di kelas Game
        game.saveGame(filename);
        return false;
    }
    
    std::string getName() const override { 
        return "SIMPAN"; 
    }
};