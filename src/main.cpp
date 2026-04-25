#include "core/Game.hpp"
#include "views/CLIRenderer.hpp"
#include "views/commands/CommandHandler.hpp"
#include <iostream>
#include <string>
#include <vector>

int main() {
    std::cout << "=======================================\n";
    std::cout << "          NIMONSPOLI                  \n";
    std::cout << "=======================================\n\n";

    Game game;
    CLIRenderer renderer;

    game.initialize();
    game.setRenderer(&renderer);

    // ===== Menu Awal =====
    renderer.printMainMenu();
    std::string pilihan;
    std::getline(std::cin, pilihan);

    if (pilihan == "1") {
        // ===== New Game =====
        int jumlahPemain = 0;
        while (jumlahPemain < 2 || jumlahPemain > 4) {
            std::cout << "Jumlah pemain (2-4): ";
            std::string input;
            std::getline(std::cin, input);
            try {
                jumlahPemain = std::stoi(input);
                if (jumlahPemain < 2 || jumlahPemain > 4) {
                    renderer.printError("Jumlah pemain harus antara 2-4.");
                }
            } catch (...) {
                renderer.printError("Input tidak valid, masukkan angka.");
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
                        if (existing == name) {
                            duplicate = true;
                            break;
                        }
                    }
                    if (duplicate) {
                        renderer.printError("Username sudah dipakai, pilih yang lain.");
                    } else {
                        valid = true;
                    }
                }
            }
            playerNames.push_back(name);
        }

        game.startNewGame(playerNames);

    } else if (pilihan == "2") {
        // ===== Load Game =====
        std::cout << "Nama file save: ";
        std::string filename;
        std::getline(std::cin, filename);

        try {
            game.initialize();
            game.setRenderer(&renderer);
            game.loadGame(filename);
        } catch (const std::exception& e) {
            renderer.printError(e.what());
            return 1;
        }
    } else {
        renderer.printError("Pilihan tidak valid.");
        return 1;
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