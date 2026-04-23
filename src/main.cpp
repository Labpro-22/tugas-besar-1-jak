// #include "core/Game.hpp"
// #include "views/CLIRenderer.hpp"
// #include "views/commands/CommandHandler.hpp"
// #include <iostream>

// int main() {
//     std::cout << "=======================================\n";
//     std::cout << "          TESTING NIMONSPOLI          \n";
//     std::cout << "=======================================\n\n";

//     // 1. Inisialisasi Game
//     Game game;
//     game.initialize();

//     // 2. Buat Renderer
//     CLIRenderer renderer;

//     // 3. Tampilkan menu awal
//     renderer.printMainMenu();
//     std::string pilihan;
//     std::getline(std::cin, pilihan);

//     if (pilihan == "1") {
//         // New Game
//         int jumlahPemain = 0;
//         while (jumlahPemain < 2 || jumlahPemain > 4) {
//             std::cout << "Jumlah pemain (2-4): ";
//             std::string input;
//             std::getline(std::cin, input);
//             try {
//                 jumlahPemain = std::stoi(input);
//             } catch (...) {
//                 renderer.printError("Input tidak valid.");
//             }
//         }

//         std::vector<std::string> playerNames;
//         for (int i = 0; i < jumlahPemain; i++) {
//             std::cout << "Username pemain " << (i + 1) << ": ";
//             std::string name;
//             std::getline(std::cin, name);
//             playerNames.push_back(name);
//         }

//         // TODO: game.startNewGame(playerNames) setelah diimplementasi

//     } else if (pilihan == "2") {
//         // Load Game
//         std::cout << "Nama file save: ";
//         std::string filename;
//         std::getline(std::cin, filename);
//         game.loadGame(filename);
//     } else {
//         renderer.printError("Pilihan tidak valid.");
//         return 1;
//     }

//     // 4. Pasang CommandHandler dan mulai loop
//     CommandHandler handler(game, renderer);

//     std::cout << "\nKetik BANTUAN untuk melihat daftar perintah.\n";
//     handler.listen();

//     std::cout << "\n=======================================\n";
//     std::cout << "   PERMAINAN SELESAI. SAMPAI JUMPA!   \n";
//     std::cout << "=======================================\n";

//     return 0;
// }