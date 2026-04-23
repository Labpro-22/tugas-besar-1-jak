#include "core/Game.hpp"
#include "utils/TransactionLogger.hpp"
#include "models/Board.hpp"
#include "models/Player.hpp"
#include "models/Dice.hpp"
#include "models/BankruptcyManager.hpp"
#include "models/AuctionManager.hpp"
#include "models/CardDeck.hpp"
#include "models/ActionCard.hpp"
#include "models/SkillCard.hpp"
#include <iostream>

Game::Game()
    : gameActive(false),
      currentPhase(GamePhase::NORMAL),
      currentPlayerIndex(0),
      turnsPlayed(0),
      isAuctionActive(false),
      currentAuctionProperty(nullptr),
      currentHighestBid(0),
      currentHighestBidder(nullptr),
      renderer(nullptr),
      turnsInJailForCurrentPlayer(0),
      waitingForJailChoice(false)
{
}

Game::~Game() = default;

// Inisialisasi
void Game::initialize()
{
    logger = std::make_unique<TransactionLogger>();
    gameActive = true;
    logger->addLog("Permainan Nimonspoli telah dimulai!");
}

void Game::initializeFromSave(const std::string& currentSaveFile)
{
    logger = std::make_unique<TransactionLogger>();
    logger->addLog("Memuat permainan dari file: " + currentSaveFile);
    gameActive = true;
    // TODO: Implementation
}

bool Game::isGameActive() const
{
    std::cout << "[Game] isGameActive(), returning: " << gameActive << std::endl;
    return gameActive;
}

GamePhase Game::getCurrentPhase() const
{
    std::cout << "[Game] getCurrentPhase()" << std::endl;
    return currentPhase;
}

void Game::setGameActive(bool active)
{
    std::cout << "[Game] setGameActive() with value: " << active << std::endl;
    gameActive = active;
}

// Dadu
void Game::rollDice()
{
    std::cout << "[Game] rollDice()" << std::endl;
    logger->addLog("Pemain melempar dadu.");
    // TODO: Implementation
}

void Game::setDice(int x, int y)
{
    std::cout << "[Game] setDice() with values: " << x << ", " << y << std::endl;
    logger->addLog("Cheat Aktif: Dadu diatur ke " + std::to_string(x) + " dan " + std::to_string(y));
    // TODO: Implementation
}

// Property
void Game::buyCurrentProperty()
{
    std::cout << "[Game] buyCurrentProperty()" << std::endl;
    logger->addLog("Pemain mencoba membeli properti di petak saat ini.");
    // TODO: Implementation
}

void Game::mortgageProperty(const std::string &code)
{
    std::cout << "[Game] mortgageProperty() for property code: " << code << std::endl;
    logger->addLog("Pemain menggadaikan properti: " + code);
    // TODO: Implementation
}

void Game::redeemProperty(const std::string &tileCode)
{
    std::cout << "[Game] redeemProperty() for tile code: " << tileCode << std::endl;
    logger->addLog("Pemain menebus properti: " + tileCode);
    // TODO: Implementation
}

void Game::buildOnProperty(const std::string &tileCode)
{
    std::cout << "[Game] buildOnProperty() for tile code: " << tileCode << std::endl;
    logger->addLog("Pemain membangun di properti: " + tileCode);
    // TODO: Implementation
}

void Game::sellBuildingOnProperty(const std::string &tileCode)
{
    std::cout << "[Game] sellBuildingOnProperty() for tile code: " << tileCode << std::endl;
    logger->addLog("Pemain menjual bangunan di properti: " + tileCode);
    // TODO: Implementation
}

// Auction
void Game::placeBid(int amount)
{
    std::cout << "[Game] placeBid() with amount: " << amount << std::endl;
    logger->addLog("Pemain melakukan penawaran lelang sebesar: " + std::to_string(amount));
    // TODO: Implementation
}

void Game::passAuction()
{
    std::cout << "[Game] passAuction()" << std::endl;
    logger->addLog("Pemain melewati giliran lelang.");
    // TODO: Implementation
}

// SkillCard
void Game::useSkillCard(int cardIndex)
{
    std::cout << "[Game] useSkillCard() with card index: " << cardIndex << std::endl;
    logger->addLog("Pemain menggunakan kartu skill index ke-" + std::to_string(cardIndex));
    // TODO: Implementation
}

// Jail
void Game::payJailFine()
{
    std::cout << "[Game] payJailFine()" << std::endl;
    logger->addLog("Pemain membayar denda penjara.");
    // TODO: Implementation
}

void Game::useJailFreeCard()
{
    std::cout << "[Game] useJailFreeCard()" << std::endl;
    logger->addLog("Pemain menggunakan kartu bebas penjara.");
    // TODO: Implementation
}

// SaveLoad
void Game::saveGame(const std::string &filename)
{
    std::cout << "[Game] saveGame() with filename: " << filename << std::endl;
    logger->addLog("Menyimpan progres game ke: " + filename);
    // TODO: Implementation
}

void Game::loadGame(const std::string &filename)
{
    std::cout << "[Game] loadGame() with filename: " << filename << std::endl;
    logger->addLog("Mencoba memuat game dari: " + filename);
    // TODO: Implementation
}

// Display
void Game::printBoard()
{
    std::cout << "[Game] printBoard()" << std::endl;
    logger->addLog("Menampilkan papan permainan.");
    // TODO: Implementation
}

void Game::printDeed(const std::string &tileCode)
{
    std::cout << "[Game] printDeed() for tile code: " << tileCode << std::endl;
    logger->addLog("Menampilkan detail akta: " + tileCode);
    // TODO: Implementation
}

void Game::printPropertyInventory()
{
    std::cout << "[Game] printPropertyInventory()" << std::endl;
    logger->addLog("Menampilkan daftar properti pemain.");
    // TODO: Implementation
}

void Game::printLog(int limit)
{
    std::cout << "[Game] printLog() with limit: " << limit << std::endl;
    if (logger) {
        logger->printLog(limit);
    }
    // TODO: Implementation
}

void Game::printHelp() { 
    std::cout << "=================================================================\n";
    std::cout << "                 DAFTAR PERINTAH NIMONSPOLI                      \n";
    std::cout << "=================================================================\n\n";

    std::cout << "[AKSI GILIRAN]\n";
    std::cout << "  LEMPAR_DADU    : Melempar dadu untuk bergerak ke petak baru.\n";
    std::cout << "  AKHIRI_GILIRAN : Mengakhiri giliranmu dan lanjut ke pemain berikutnya.\n\n";

    std::cout << "[PROPERTI & BANGUNAN]\n";
    std::cout << "  BELI               : Membeli properti di petak tempatmu berada.\n";
    std::cout << "  BANGUN <petak>     : Membangun rumah/hotel di properti milikmu.\n";
    std::cout << "  JUAL_BANGUNAN <ptk>: Menjual bangunan yang ada di properti milikmu.\n\n";

    std::cout << "[MANAJEMEN ASET & LELANG]\n";
    std::cout << "  GADAI <petak> : Menggadaikan properti untuk mendapatkan uang.\n";
    std::cout << "  TEBUS <petak> : Menebus properti yang sedang digadaikan.\n";
    std::cout << "  TAWAR <harga> : Mengajukan harga saat sesi lelang properti.\n";
    std::cout << "  LEPAS         : Mundur dari sesi lelang saat ini.\n";
    std::cout << "  BANGKRUT      : Menyerah dan keluar dari permainan karena kehabisan uang.\n\n";

    std::cout << "[PENJARA & KARTU]\n";
    std::cout << "  BAYAR_DENDA                 : Membayar denda agar bisa keluar dari penjara.\n";
    std::cout << "  GUNAKAN_KARTU_BEBAS         : Menggunakan kartu khusus untuk bebas.\n";
    std::cout << "  GUNAKAN_KEMAMPUAN <index>   : Menggunakan kartu skill (kemampuan) yang dimiliki.\n\n";

    std::cout << "[INFORMASI]\n";
    std::cout << "  CETAK_STATUS   : Melihat info saldo, posisi, dan statusmu saat ini.\n";
    std::cout << "  CETAK_PAPAN    : Menampilkan kondisi papan Nimonspoli saat ini.\n";
    std::cout << "  CETAK_PROPERTI : Menampilkan daftar seluruh properti yang kamu miliki.\n";
    std::cout << "  CETAK_AKTA <p> : Melihat detail harga, biaya sewa, dan status sebuah properti.\n";
    std::cout << "  CETAK_LOG      : Melihat riwayat aksi yang sudah terjadi di dalam game.\n";
    std::cout << "  BANTUAN        : Menampilkan menu daftar perintah ini.\n\n";

    std::cout << "[SISTEM & CHEAT]\n";
    std::cout << "  SIMPAN <file>   : Menyimpan progres permainan (contoh: SIMPAN game1.txt).\n";
    std::cout << "  MUAT <file>     : Memuat progres permainan dari file penyimpanan.\n";
    std::cout << "  ATUR_DADU <x> <y>: [Cheat] Mengatur angka dadu secara manual untuk testing.\n";
    std::cout << "  QUIT            : Keluar dari aplikasi Nimonspoli sepenuhnya.\n\n";
    
    std::cout << "=================================================================\n";
}

void Game::printPlayerStatus() { 
    std::cout << "[Game] printPlayerStatus() - Menampilkan status pemain...\n"; 
    logger->addLog("Menampilkan status pemain saat ini.");
    // TODO: Implementation
}

// Flow Giliran
void Game::endTurn() { 
    std::cout << "[Game] endTurn()\n"; 
    logger->addLog("Giliran pemain berakhir.");
    // TODO: panggil nextTurn()
}

void Game::declareBankruptcy() { 
    std::cout << "[Game] declareBankruptcy()\n"; 
    logger->addLog("Pemain menyatakan bangkrut!");
    // TODO: panggil fungsi di BankruptcyManager
}

// Getter
CLIRenderer &Game::getRenderer()
{
    std::cout << "[Game] getRenderer()" << std::endl;
    return *renderer;
}

int Game::getCurrentPlayerBalance()
{
    std::cout << "[Game] getCurrentPlayerBalance() int version" << std::endl;
    // TODO: Implementation - return balance as int
    return 0;
}

std::string Game::getCurrentPlayerBalanceString()
{
    std::cout << "[Game] getCurrentPlayerBalanceString() " << std::endl;
    // TODO: Implementation - return balance as string
    return "Rp 0"; // Stub
}

Player *Game::getCurrentPlayer() const
{
    std::cout << "[Game] getCurrentPlayer()" << std::endl;
    // TODO: Implementation - return current player pointer
    return nullptr;
}

Board& Game::getBoard() const
{
    std::cout << "[Game] getBoard()" << std::endl;
    return *board;
}

TransactionLogger *Game::getLogger()
{
    std::cout << "[Game] getLogger()" << std::endl;
    return logger.get();
}

// Manajemen Aset Pemain
void Game::addSkillCardToPlayer(Player &player, std::unique_ptr<SkillCard> card)
{
    std::cout << "[Game] addSkillCardToPlayer()" << std::endl;
    // TODO: Implementation
}

void Game::addActionCardToPlayer(Player &player, std::unique_ptr<ActionCard> card)
{
    std::cout << "[Game] addActionCardToPlayer()" << std::endl;
    // TODO: Implementation
}