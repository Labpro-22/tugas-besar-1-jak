#pragma once
#include "CommandParser.hpp"
#include "core/IGameAction.hpp"
#include "utils/NimonspoliException.hpp"
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
            std::cout << "[" << game.getCurrentPlayerName() << "] > ";
            if (!std::getline(std::cin, rawInput)) {
                break;
            }
            // Info giliran dah habis belum
            bool turnEnded = handleInput(rawInput);

            // Kalau habis (true)
            if (turnEnded) {
                break;
            }
        }
    }

    // Handle satu baris input
    bool handleInput(const std::string& rawInput) {
        bool isTurnEnded = false;
        try {
            // Lempar ke CommandParser buat dapet objek Command
            Command* command = CommandParser::parse(rawInput);

            if (command != nullptr) {
                // Kasih ke Game dan simpan status boolean-nya
                isTurnEnded = command->execute(game);

                // Hapus memory
                delete command;
            }
        } catch (const NimonspoliException& e) { // Error handling
            std::cerr << "[Error] " << e.what() << "\n";
        } catch (const std::exception& e) {
            std::cerr << "[Error Sistem] " << e.what() << "\n";
        }
        return isTurnEnded;
    }

    // Minta input konfirmasi (buat Game minta jawaban y/n atau pilihan dari menu)
    std::string promptInput(const std::string& prompt) {
        std::cout << prompt;
        std::string input;
        std::getline(std::cin, input);
        return input;
    }
};