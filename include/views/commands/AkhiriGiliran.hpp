#pragma once
#include "Command.hpp"
#include "IGameAction.hpp"
#include <string>

class AkhiriGiliranCommand : public Command {
public:
    bool execute(IGameAction& game) override {
        game.akhiriGiliran();
        return true; // True karena buat ganti giliran pemain
    }
    
    std::string getName() const override { 
        return "AKHIRI_GILIRAN"; 
    }
};