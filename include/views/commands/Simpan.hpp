#pragma once
#include "Command.hpp"
#include "IGameAction.hpp"
#include <string>

// SIMPAN <filename>
class SimpanCommand : public Command {
    private:
    std::string filename;
    
    public:
    explicit SimpanCommand(std::string filename) : filename(std::move(filename)) {}

    void execute(IGameAction& game) override {
        // TODO: panggil fungsi simpan game yang ada di kelas Game
        // Misal: game.simpanGame(filename);
    }
    
    std::string getName() const override { 
        return "SIMPAN"; 
    }
};