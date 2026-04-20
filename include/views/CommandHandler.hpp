#pragma once
#include "CommandParser.hpp"
#include "../../include/core/IGameAction.hpp"
#include "../../include/utils/NimonspoliException.hpp"

#include <string>
#include <iostream>

using namespace std;

// Forward declaration
class CLIRenderer;

// Gunanya CommandHandler sebagai listener, jadi cuman sebagai jembatan antara input user dengan Game (lewat CommandParser)
// Input --> CommandHandler --> CommandParser --> Game
class CommandHandler {
    private:
    IGameAction& game_;
    CLIRenderer& renderer_;

    public:
    CommandHandler(IGameAction& game, CLIRenderer& renderer) : game_(game), renderer_(renderer) {}

    // Mintain input dan dia dipake sama Game tiap turn dimulai
    void listen() {
        string rawInput;
        while (game_.isGameActive()) {
            cout << "> ";
            if (!getline(cin, rawInput)) {
                break;
            }
            handleInput(rawInput);
        }
    }

    // Handle satu baris input
    void handleInput(const string& rawInput) {
        try {
            // Lempar ke CommandParser buat dapet objek Command
            auto command = CommandParser::parse(rawInput);

            // Pass ke Game buat game panggil fungsi ini
            command->execute(game_);
        } catch (const NimonspoliException& e) { // Error handling
            cerr << "[Error] " << e.what() << "\n";
        }
    }

    // Minta input konfirmasi (buat Game minta jawaban y/n atau pilihan dari menu)
    string promptInput(const string& prompt) {
        cout << prompt;
        string input;
        getline(cin, input);
        return input;
    }
};