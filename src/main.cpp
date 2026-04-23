#include "core/Game.hpp"
#include "views/CLIRenderer.hpp"
#include "views/commands/CommandHandler.hpp"
#include <iostream>

int main() {
    std::cout << "=======================================\n";
    std::cout << "  TEST ENGINE NIMONSPOLI (CLI MODE)    \n";
    std::cout << "=======================================\n";

    // 1. Inisialisasi Game (Ini akan mengaktifkan logger dan set gameActive = true)
    Game game;
    game.initialize(); 

    // 2. Buat Dummy Renderer
    CLIRenderer renderer;

    // 3. Pasang Command Handler sebagai jembatan
    CommandHandler handler(game, renderer);

    std::cout << "\nKetik BANTUAN untuk melihat daftar perintah.\n";
    
    // 4. Mulai looping input dari user!
    handler.listen();

    std::cout << "\n=======================================\n";
    std::cout << "  PERMAINAN SELESAI. SAMPAI JUMPA!     \n";
    std::cout << "=======================================\n";

    return 0;
}