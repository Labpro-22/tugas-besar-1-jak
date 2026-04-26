#include "core/Game.hpp"
#include "views/CLIRenderer.hpp"
#include "views/commands/CommandHandler.hpp"
#include <iostream>
#include <string>
#include <vector>

int main() {
    Game game;
    CLIRenderer renderer;

    // Menu awal
    renderer.printMainMenu();

    // Loop awal sebelum game dimulai
    bool gameStarted = false;
    while (!gameStarted) {
        std::string input;
        std::getline(std::cin, input);

        // New Game
        if (input == "1") {
            game.initialize();
            game.setRenderer(&renderer);

            int jumlahPemain = 0;
            while (jumlahPemain < 2 || jumlahPemain > 4) {
                std::cout << "Jumlah pemain (2-4): ";
                std::string inp;
                std::getline(std::cin, inp);
                try {
                    jumlahPemain = std::stoi(inp);
                    if (jumlahPemain < 2 || jumlahPemain > 4)
                        renderer.printError("Jumlah pemain harus antara 2-4.");
                } catch (...) {
                    renderer.printError("Input tidak valid.");
                }
            }

            std::vector<std::string> playerNames;
            for (int i = 0; i < jumlahPemain; i++) {
                std::string name;
                bool valid = false;
                while (!valid) {
                    std::cout << "Username pemain " << (i + 1) << ": ";
                    std::getline(std::cin, name);
                    if (name.empty()) {
                        renderer.printError("Username tidak boleh kosong.");
                    } else {
                        bool duplicate = false;
                        for (const auto& existing : playerNames) {
                            if (existing == name) { duplicate = true; break; }
                        }
                        if (duplicate) renderer.printError("Username sudah dipakai.");
                        else valid = true;
                    }
                }
                playerNames.push_back(name);
            }

            game.startNewGame(playerNames);
            gameStarted = true;
        } else if (input == "2") {
            std::cout << "Nama file save: ";
            std::string filename;
            std::getline(std::cin, filename);
            renderer.printInfo("Memuat permainan...");
            game.setRenderer(&renderer);
            game.initialize();
            try {
                game.loadGame(filename);
                renderer.printInfo("Permainan berhasil dimuat. Melanjutkan giliran " + game.getCurrentPlayerName() + "...");
                gameStarted = true;
            } catch (const std::exception& e) {
                renderer.printError(e.what());
                renderer.printInfo("Silakan pilih opsi lagi.");
                renderer.printMainMenu();
            }
        } else {
            renderer.printError("Pilihan tidak valid. Masukkan 1 atau 2.");
        }
    }

    // ===== Loop Utama =====
    CommandHandler handler(game, renderer);

    std::cout << "\nKetik BANTUAN untuk melihat daftar perintah.\n\n";
    handler.listen();

    std::cout << "\n=======================================\n";
    std::cout << "   PERMAINAN SELESAI. SAMPAI JUMPA!   \n";
    std::cout << "=======================================\n";

    return 0;
}