#pragma once
#include "CommandParser.hpp"
#include "../../include/core/IGameAction.hpp"
#include "../../include/utils/NimonspoliException.hpp"

#include <string>
#include <iostream>

// Forward declaration
class CLIRenderer;

// Gunanya CommandHandler sebagai listener, jadi cuman sebagai jembatan antara input user dengan Game (lewat CommandParser)
// Input --> CommandHandler --> CommandParser --> Game
class CommandHandler {
    private:
    IGameAction& game;
    CLIRenderer& renderer;

    public:
    CommandHandler(IGameAction& game, CLIRenderer& renderer) : game(game), renderer(renderer) {}

    // Mintain input dan dia dipake sama Game tiap turn dimulai
    void listen() {
        std::string rawInput;
        while (game.isGameActive()) {
            cout << "> ";
            if (!std::getline(std::cin, rawInput)) {
                break;
            }
            handleInput(rawInput);
        }
    }

    // Handle satu baris input
    void handleInput(const std::string& rawInput) {
        try {
            // Lempar ke CommandParser buat dapet objek Command
            auto command = CommandParser::parse(rawInput);

            // Pass ke Game buat game panggil fungsi ini
            command->execute(game);
        } catch (const NimonspoliException& e) { // Error handling
            std::cerr << "[Error] " << e.what() << "\n";
        }
    }

    // Minta input konfirmasi (buat Game minta jawaban y/n atau pilihan dari menu)
    std::string promptInput(const std::string& prompt) {
        std::cout << prompt;
        std::string input;
        std::getline(cin, input);
        return input;
    }
};