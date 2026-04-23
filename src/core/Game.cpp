#include "core/Game.hpp"
#include "utils/TransactionLogger.hpp"
#include "models/Board.hpp"
#include "models/Player.hpp"
#include "models/Dice.hpp"
#include "models/Tile.hpp"
#include "models/PropertyTile.hpp"
#include "models/ActionTile.hpp"
#include "models/SpecialTile.hpp"
#include "core/BankruptcyManager.hpp"
#include "models/LiquidationOption.hpp"
#include "core/AuctionManager.hpp"
#include "models/Card.hpp"
#include "models/CardDeck.hpp"
#include "models/ActionCard.hpp"
#include "models/SkillCard.hpp"
#include "utils/ConfigLoader.hpp"
#include "utils/NimonspoliException.hpp"
#include "views/CLIRenderer.hpp"
#include <iostream>
#include <algorithm>
#include <random>

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
    board = std::make_unique<Board>();
    board->initializeBoard("config/");

    // Simpen nilai config
    auto special = ConfigLoader::loadSpecial("config/");
    goSalary = 200;
    if (special.count("GO_SALARY")) goSalary = special["GO_SALARY"];
    jailFine = 50;
    if (special.count("JAIL_FINE")) jailFine = special["JAIL_FINE"];

    auto tax = ConfigLoader::loadTax("config/");
    pphFlat = 150;
    if (tax.count("PPH_FLAT")) pphFlat = tax["PPH_FLAT"];
    pphPersen = 10;
    if (tax.count("PPH_PERSENTASE")) pphPersen = tax["PPH_PERSENTASE"];
    pbmFlat = 200;
    if (tax.count("PBM_FLAT")) pbmFlat = tax["PBM_FLAT"];

    // Deck kartu
    // Inisialisasi deck Kesempatan
    chanceDeck = std::make_unique<CardDeck<ActionCard>>();
    chanceDeck->addCardToDeck(new GoToNearestStationCard());
    chanceDeck->addCardToDeck(new MoveBackCard());
    chanceDeck->addCardToDeck(new GoToJailCard());

    // Inisialisasi deck Dana Umum
    generalFundsDeck = std::make_unique<CardDeck<ActionCard>>();
    generalFundsDeck->addCardToDeck(new BirthdayCard());
    generalFundsDeck->addCardToDeck(new DoctorFeeCard());
    generalFundsDeck->addCardToDeck(new NyalegCard());

    // Inisialisasi deck Skill
    skillCardDeck = std::make_unique<CardDeck<SkillCard>>();
    skillCardDeck->addCardToDeck(new MoveCard(3));
    skillCardDeck->addCardToDeck(new MoveCard(4));
    skillCardDeck->addCardToDeck(new MoveCard(5));
    skillCardDeck->addCardToDeck(new MoveCard(6));
    skillCardDeck->addCardToDeck(new DiscountCard(10, 1));
    skillCardDeck->addCardToDeck(new DiscountCard(20, 1));
    skillCardDeck->addCardToDeck(new DiscountCard(30, 1));
    skillCardDeck->addCardToDeck(new ShieldCard(1));
    skillCardDeck->addCardToDeck(new ShieldCard(1));
    skillCardDeck->addCardToDeck(new TeleportCard());
    skillCardDeck->addCardToDeck(new TeleportCard());
    skillCardDeck->addCardToDeck(new LassoCard());
    skillCardDeck->addCardToDeck(new LassoCard());
    skillCardDeck->addCardToDeck(new DemolitionCard());
    skillCardDeck->addCardToDeck(new DemolitionCard());

    dice = std::make_unique<Dice>();
    logger = std::make_unique<TransactionLogger>();
    bankruptcyManager = std::make_unique<BankruptcyManager>();
    auctionManager = std::make_unique<AuctionManager>(this);
    gameActive = true;
    logger->addLog("Permainan Nimonspoli telah dimulai!");
}

// Mulai game baru
void Game::startNewGame(const std::vector<std::string>& playerNames) {
    // Baca config misc untuk saldo awal dan max turn
    auto misc = ConfigLoader::loadMisc("config/");
    int saldoAwal = 1000;
    if (misc.count("SALDO_AWAL")) {
        saldoAwal = misc["SALDO_AWAL"];
    }
    maxTurn = 15;
    if (misc.count("MAX_TURN")) {
        maxTurn = misc["MAX_TURN"];
    }

    // Buat player
    players.clear();
    for (const auto& name : playerNames) {
        auto p = std::make_unique<Player>(name);
        p->setCash(saldoAwal);
        p->setPosition(board->getStartTileIndex());
        players.push_back(std::move(p));
    }

    // Acak urutan giliran
    turnOrder.clear();
    for (int i = 0; i < (int)players.size(); i++) {
        turnOrder.push_back(i);
    }
    std::shuffle(turnOrder.begin(), turnOrder.end(), std::mt19937(std::random_device{}()));
    currentPlayerIndex = 0;
    turnsPlayed = 1;

    logger->addLog("New game dimulai dengan " + std::to_string(players.size()) + " pemain.");
}

// Ambil pemain aktif saat ini
Player *Game::getCurrentPlayer() const
{
    if (players.empty()) {
        return nullptr;
    }
    return players[turnOrder[currentPlayerIndex]].get();
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
void Game::rollDice() {
    Player* player = getCurrentPlayer();
    if (!player) return;

    int result = dice->roll();
    int d1 = dice->getDice1();
    int d2 = dice->getDice2();

    logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " +
                   player->getUsername() + " | DADU | Lempar: " +
                   std::to_string(d1) + "+" + std::to_string(d2) + "=" +
                   std::to_string(result));

    if (dice->isDouble()) {
        player->setConsecutiveDoublesDice(player->getConsecutiveDoublesDice() + 1);
        if (player->getConsecutiveDoublesDice() >= 3) {
            // Double 3x berturut — langsung ke penjara
            player->setConsecutiveDoublesDice(0);
            player->goToJail();
            logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " +
                           player->getUsername() + " | PENJARA | Double 3x, masuk penjara!");
            return;
        }
    } else {
        player->setConsecutiveDoublesDice(0);
    }

    movePlayer(*player, result);
}

void Game::setDice(int x, int y) {
    Player* player = getCurrentPlayer();
    if (!player) return;

    dice->setDice(x, y);
    int result = dice->getTotal();

    logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " +
                   player->getUsername() + " | DADU | Diatur manual: " +
                   std::to_string(x) + "+" + std::to_string(y) + "=" +
                   std::to_string(result));

    movePlayer(*player, result);
}

void Game::movePlayer(Player& player, int steps) {
    int startPos = player.getPosition();
    int boardSize = board->getTileCount();
    int newPos = (startPos + steps) % boardSize;

    // Cek apakah melewati GO
    if (newPos < startPos || (startPos + steps) >= boardSize) {
        auto misc = ConfigLoader::loadMisc("config/");
        int goSalary = 200;
        if (misc.count("GO_SALARY")) goSalary = misc["GO_SALARY"];
        // Tapi GO_SALARY ada di special.txt, bukan misc.txt
        // Ambil dari special config
        auto special = ConfigLoader::loadSpecial("config/");
        goSalary = 200;
        if (special.count("GO_SALARY")) goSalary = special["GO_SALARY"];

        player += goSalary;
        logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " +
                       player.getUsername() + " | GO | Melewati GO, dapat M" +
                       std::to_string(goSalary));
    }

    player.setPosition(newPos);
    Tile* tile = board->getTile(newPos);
    std::string tileName = tile ? tile->getCode() : "???";

    logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " +
                   player.getUsername() + " | GERAK | Mendarat di " + tileName);

    processTileLanding(player, newPos);
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

void Game::finalizeAuction(Player* winner, PropertyTile* property, int winningBid) {
    if (winner == nullptr) {
        logger->addLog("Lelang gagal: " + property->getCode() + " tidak terjual.");
        return;
    }

    *winner -= winningBid;
    property->changeOwner(winner);
    winner->addProperty(property);
    logger->addLog(winner->getUsername() + " memenangkan lelang " + property->getCode() + " dengan harga " + std::to_string(winningBid));
}

void Game::logAuctionEvent(const std::string& action, const std::string& detail) {
    logger->addLog("[AUCTION] " + action + ": " + detail);
}

void Game::triggerAuction(PropertyTile& property) {
    // TODO: mulai lelang via auctionManager
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

// Player
int Game::countActivePlayers() const {
    int count = 0;
    for (const auto& p : players) {
        if (p && p->getStatus() != "BANKRUPT") count++;
    }
    return count;
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

// Private methods
void Game::processTileLanding(Player& player, int tileIndex) {
    Tile* tile = board->getTile(tileIndex);
    if (!tile) return;

    // Street
    StreetTile* street = dynamic_cast<StreetTile*>(tile);
    if (street) {
        handleStreetLanding(player, *street);
        return;
    }

    // Railroad
    RailroadTile* railroad = dynamic_cast<RailroadTile*>(tile);
    if (railroad) {
        handleRailroadLanding(player, *railroad);
        return;
    }

    // Utility
    UtilityTile* utility = dynamic_cast<UtilityTile*>(tile);
    if (utility) {
        handleUtilityLanding(player, *utility);
        return;
    }

    // Tax
    TaxTile* tax = dynamic_cast<TaxTile*>(tile);
    if (tax) {
        handleTaxLanding(player, *tax);
        return;
    }

    // Festival
    FestivalTile* festival = dynamic_cast<FestivalTile*>(tile);
    if (festival) {
        handleFestivalLanding(player);
        return;
    }

    // Card (Kesempatan/Dana Umum)
    CardTile* card = dynamic_cast<CardTile*>(tile);
    if (card) {
        handleCardLanding(player, *card);
        return;
    }

    // Spesial (GO, Penjara, Bebas Parkir, Pergi ke Penjara)
    // sudah dihandle via onLanded() di masing-masing SpecialTile
    tile->onLanded(player, *this);
}

// handleStreetLanding
void Game::handleStreetLanding(Player& player, StreetTile& tile) {
    PropertyStatus status = tile.getStatus();

    // Sudah dimiliki pemain lain — bayar sewa
    if (status == PropertyStatus::OWNED && tile.getOwner() != &player) {
        applyRent(player, tile);
        return;
    }

    // Digadaikan — tidak ada sewa
    if (status == PropertyStatus::MORTGAGED) {
        renderer->printInfo("Kamu mendarat di " + tile.getName() + " (" + tile.getCode() + "), milik " + tile.getOwner()->getUsername() + ".");
        renderer->printInfo("Properti ini sedang digadaikan [M]. Tidak ada sewa yang dikenakan.");
        return;
    }

    // Milik sendiri — tidak ada aksi
    if (status == PropertyStatus::OWNED && tile.getOwner() == &player) {
        renderer->printInfo("Kamu mendarat di properti milikmu sendiri: " + tile.getName());
        return;
    }

    // Status BANK — tawarkan pembelian
    if (status == PropertyStatus::BANK) {
        renderer->printInfo("Kamu mendarat di " + tile.getName() + " (" + tile.getCode() + ")!");
        renderer->printAkta(tile);

        renderer->printInfo("Uang kamu saat ini: M" + std::to_string(player.getCash()));
        std::cout << "Apakah kamu ingin membeli properti ini seharga M" << tile.getPrice() << "? (y/n): ";

        std::string input;
        std::getline(std::cin, input);

        if (input == "y" || input == "Y") {
            if (player.getCash() < tile.getPrice()) {
                renderer->printInfo("Uang tidak cukup! Properti masuk ke sistem lelang...");
                startAuctionForProperty(tile);
                return;
            }
            player -= tile.getPrice();
            tile.changeOwner(&player);
            player.addProperty(&tile);
            tile.redeem();
            logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " +
                           player.getUsername() + " | BELI | Beli " +
                           tile.getName() + " (" + tile.getCode() + ") seharga M" +
                           std::to_string(tile.getPrice()));
            renderer->printInfo(tile.getName() + " kini menjadi milikmu!");
            renderer->printInfo("Uang tersisa: M" + std::to_string(player.getCash()));
        } else {
            renderer->printInfo("Properti ini akan masuk ke sistem lelang...");
            startAuctionForProperty(tile);
        }
    }
}

// Helper applyRent
void Game::applyRent(Player& player, PropertyTile& tile) {
    if (player.isShieldActive()) {
        renderer->printInfo("[SHIELD ACTIVE] Kamu terlindungi dari sewa!");
        return;
    }

    int rent = tile.calculateRent(dice->getTotal());
    Player* owner = tile.getOwner();

    renderer->printInfo("Kamu mendarat di " + tile.getName() +
                        " (" + tile.getCode() + "), milik " + owner->getUsername() + "!");
    renderer->printInfo("Sewa: M" + std::to_string(rent));

    try {
        player.payRent(rent, *owner);
        logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " +
                       player.getUsername() + " | SEWA | Bayar M" +
                       std::to_string(rent) + " ke " + owner->getUsername() +
                       " (" + tile.getCode() + ")");
        renderer->printInfo("Uang kamu: M" + std::to_string(player.getCash()));
        renderer->printInfo("Uang " + owner->getUsername() + ": M" + std::to_string(owner->getCash()));
    } catch (const NimonspoliException&) {
        // Tidak cukup bayar — masuk alur bankruptcy
        bankruptcyManager->handle(player, owner, rent, *this,
            [this](const std::vector<LiquidationOption>& options) -> int {
                renderer->printInfo("\n=== Panel Likuidasi ===");
                for (int i = 0; i < (int)options.size(); i++) {
                    renderer->printInfo(std::to_string(i + 1) + ". " + options[i].description);
                }
                renderer->printInfo("0. Selesai");
                std::cout << "Pilih aksi: ";
                std::string input;
                std::getline(std::cin, input);
                try {
                    int choice = std::stoi(input) - 1;
                    return choice;
                } catch (...) {
                    return -1;
                }
            });
    }
}

void Game::handleRailroadLanding(Player& player, RailroadTile& tile) {
    if (tile.getStatus() == PropertyStatus::BANK) {
        tile.changeOwner(&player);
        tile.redeem();
        player.addProperty(&tile);
        renderer->printInfo("Kamu mendarat di " + tile.getName() + "!");
        renderer->printInfo("Belum ada yang menginjaknya duluan, stasiun ini kini menjadi milikmu!");
        logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " +
                       player.getUsername() + " | RAILROAD | " +
                       tile.getCode() + " kini milik " + player.getUsername() + " (otomatis)");
        return;
    }

    if (tile.getStatus() == PropertyStatus::OWNED && tile.getOwner() != &player) {
        applyRent(player, tile);
        return;
    }

    if (tile.getStatus() == PropertyStatus::MORTGAGED) {
        renderer->printInfo("Stasiun ini sedang digadaikan [M]. Tidak ada sewa.");
    }
}

void Game::handleUtilityLanding(Player& player, UtilityTile& tile) {
    if (tile.getStatus() == PropertyStatus::BANK) {
        tile.changeOwner(&player);
        tile.redeem();
        player.addProperty(&tile);
        renderer->printInfo("Kamu mendarat di " + tile.getName() + "!");
        renderer->printInfo("Belum ada yang menginjaknya duluan, " + tile.getName() + " kini menjadi milikmu!");
        logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " +
                       player.getUsername() + " | UTILITY | " +
                       tile.getCode() + " kini milik " + player.getUsername() + " (otomatis)");
        return;
    }

    if (tile.getStatus() == PropertyStatus::OWNED && tile.getOwner() != &player) {
        applyRent(player, tile);
        return;
    }

    if (tile.getStatus() == PropertyStatus::MORTGAGED) {
        renderer->printInfo("Utilitas ini sedang digadaikan [M]. Tidak ada sewa.");
    }
}

void Game::handleTaxLanding(Player& player, TaxTile& tile) {
    if (tile.getTaxType() == TaxType::PBM) {
        renderer->printInfo("Kamu mendarat di Pajak Barang Mewah (PBM)!");
        renderer->printInfo("Pajak sebesar M" + std::to_string(pbmFlat) + " langsung dipotong.");

        if (player.isShieldActive()) {
            renderer->printInfo("[SHIELD ACTIVE] Kamu terlindungi dari pajak!");
            return;
        }

        try {
            player -= pbmFlat;
            logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " +
                           player.getUsername() + " | PAJAK | Bayar PBM M" +
                           std::to_string(pbmFlat));
            renderer->printInfo("Uang kamu: M" + std::to_string(player.getCash()));
        } catch (const NimonspoliException&) {
            bankruptcyManager->handle(player, nullptr, pbmFlat, *this,
                [this](const std::vector<LiquidationOption>& options) -> int {
                    renderer->printInfo("\n=== Panel Likuidasi ===");
                    for (int i = 0; i < (int)options.size(); i++) {
                        renderer->printInfo(std::to_string(i + 1) + ". " + options[i].description);
                    }
                    renderer->printInfo("0. Selesai");
                    std::cout << "Pilih aksi: ";
                    std::string input;
                    std::getline(std::cin, input);
                    try {
                        return std::stoi(input) - 1;
                    } catch (...) {
                        return -1;
                    }
                });
        }
        return;
    }

    // PPH
    renderer->printInfo("Kamu mendarat di Pajak Penghasilan (PPH)!");
    renderer->printInfo("Pilih opsi pembayaran pajak:");
    renderer->printInfo("1. Bayar flat M" + std::to_string(pphFlat));
    renderer->printInfo("2. Bayar " + std::to_string(pphPersen) + "% dari total kekayaan");
    renderer->printInfo("(Pilih sebelum menghitung kekayaan!)");
    std::cout << "Pilihan (1/2): ";

    std::string input;
    std::getline(std::cin, input);

    if (player.isShieldActive()) {
        renderer->printInfo("[SHIELD ACTIVE] Kamu terlindungi dari pajak!");
        return;
    }

    int amount = 0;
    if (input == "1") {
        amount = pphFlat;
    } else {
        int totalWealth = player.getTotalWealth();
        amount = totalWealth * pphPersen / 100;
        renderer->printInfo("Total kekayaan: M" + std::to_string(totalWealth));
        renderer->printInfo("Pajak " + std::to_string(pphPersen) + "%: M" + std::to_string(amount));
    }

    try {
        player -= amount;
        logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " +
                       player.getUsername() + " | PAJAK | Bayar PPH M" +
                       std::to_string(amount));
        renderer->printInfo("Uang kamu: M" + std::to_string(player.getCash()));
    } catch (const NimonspoliException&) {
        bankruptcyManager->handle(player, nullptr, amount, *this,
            [this](const std::vector<LiquidationOption>& options) -> int {
                renderer->printInfo("\n=== Panel Likuidasi ===");
                for (int i = 0; i < (int)options.size(); i++) {
                    renderer->printInfo(std::to_string(i + 1) + ". " + options[i].description);
                }
                renderer->printInfo("0. Selesai");
                std::cout << "Pilih aksi: ";
                std::string input;
                std::getline(std::cin, input);
                try {
                    return std::stoi(input) - 1;
                } catch (...) {
                    return -1;
                }
            });
    }
}

void Game::handleFestivalLanding(Player& player) {
    auto& ownedProps = player.getOwnedProperties();

    if (ownedProps.empty()) {
        renderer->printInfo("Kamu mendarat di petak Festival, tapi tidak punya properti!");
        return;
    }

    renderer->printInfo("Kamu mendarat di petak Festival!");
    renderer->printInfo("Daftar properti milikmu:");
    for (auto* prop : ownedProps) {
        renderer->printInfo("- " + prop->getCode() + " (" + prop->getName() + ")");
    }

    std::string kode;
    StreetTile* chosen = nullptr;
    while (!chosen) {
        std::cout << "Masukkan kode properti: ";
        std::getline(std::cin, kode);

        for (auto* prop : ownedProps) {
            StreetTile* st = dynamic_cast<StreetTile*>(prop);
            if (st && st->getCode() == kode) {
                chosen = st;
                break;
            }
        }

        if (!chosen) {
            // Cek apakah kode ada tapi bukan milik player
            Tile* tile = nullptr;
            for (int i = 0; i < board->getTileCount(); i++) {
                if (board->getTile(i)->getCode() == kode) {
                    tile = board->getTile(i);
                    break;
                }
            }
            if (!tile) {
                renderer->printError("Kode properti tidak valid!");
            } else {
                renderer->printError("Properti bukan milikmu!");
            }
        }
    }

    int sewaBefore = chosen->calculateRent(0);
    chosen->applyFestival();
    int sewaAfter = chosen->calculateRent(0);

    if (chosen->getFestivalMultiplier() > 8) {
        renderer->printInfo("Efek sudah maksimum (harga sewa sudah digandakan tiga kali)");
        renderer->printInfo("Durasi di-reset menjadi: 3 giliran");
    } else if (sewaBefore == sewaAfter) {
        renderer->printInfo("Efek diperkuat!");
        renderer->printInfo("Sewa sebelumnya: M" + std::to_string(sewaBefore));
        renderer->printInfo("Sewa sekarang: M" + std::to_string(sewaAfter));
        renderer->printInfo("Durasi di-reset menjadi: 3 giliran");
    } else {
        renderer->printInfo("Efek festival aktif!");
        renderer->printInfo("Sewa awal: M" + std::to_string(sewaBefore));
        renderer->printInfo("Sewa sekarang: M" + std::to_string(sewaAfter));
        renderer->printInfo("Durasi: 3 giliran");
    }

    logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " +
                   player.getUsername() + " | FESTIVAL | " +
                   chosen->getCode() + ": sewa M" + std::to_string(sewaBefore) +
                   " -> M" + std::to_string(sewaAfter) +
                   ", durasi 3 giliran");
}

void Game::handleCardLanding(Player& player, CardTile& tile) {
    if (tile.getDeckType() == DeckType::CHANCE) {
        card = chanceDeck->draw();
    } else {
        card = generalFundsDeck->draw();
    }
}

void Game::processTileLandingPublic(Player& player, int tileIndex) {
    processTileLanding(player, tileIndex);
}

std::vector<Player*> Game::getActivePlayers() const {
    std::vector<Player*> result;
    for (const auto& p : players) {
        if (p && p->getStatus() != "BANKRUPT") {
            result.push_back(p.get());
        }
    }
    return result;
}

void Game::endTurn() {
    Player* player = getCurrentPlayer();
    if (!player) return;

    // Reset flag skill per turn
    player->resetTurnFlags();
    player->setShieldActive(false);

    // Tick festival untuk semua properti milik player
    for (auto* prop : player->getOwnedProperties()) {
        StreetTile* st = dynamic_cast<StreetTile*>(prop);
        if (st && st->getFestivalDuration() > 0) {
            st->tickFestival();
        }
    }

    // Kalau double, giliran tambahan
    if (dice->isDouble() && player->getStatus() != "JAILED"
        && player->getConsecutiveDoublesDice() > 0) {
        renderer->printInfo("Kamu mendapat double! Giliran tambahan.");
        logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " +
                       player->getUsername() + " | DOUBLE | Giliran tambahan.");
        return; // tidak pindah ke pemain berikutnya
    }

    nextTurn();
}

void Game::nextTurn() {
    // Reset consecutive doubles
    Player* current = getCurrentPlayer();
    if (current) current->setConsecutiveDoublesDice(0);

    // Pindah ke pemain berikutnya
    currentPlayerIndex = (currentPlayerIndex + 1) % turnOrder.size();

    // Kalau sudah satu putaran penuh, naikkan turnsPlayed
    if (currentPlayerIndex == 0) {
        turnsPlayed++;

        // Cek max turn
        if (maxTurn > 0 && turnsPlayed > maxTurn) {
            gameActive = false;
            handleGameEnd();
            return;
        }
    }

    // Skip pemain bankrupt
    int attempts = 0;
    while (getCurrentPlayer() &&
           getCurrentPlayer()->getStatus() == "BANKRUPT" &&
           attempts < (int)players.size()) {
        currentPlayerIndex = (currentPlayerIndex + 1) % turnOrder.size();
        attempts++;
    }

    // Bagi kartu skill di awal giliran
    Player* next = getCurrentPlayer();
    if (next) {
        drawSkillCard(*next);
        logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " +
                       next->getUsername() + " | GILIRAN | Giliran dimulai.");
    }
}

void Game::handleGameEnd() {
    renderer->printInfo("\nPermainan selesai! (Batas giliran tercapai)");

    // Kumpulkan pemain aktif
    std::vector<Player*> active = getActivePlayers();

    // Sort berdasarkan kriteria menang sesuai spesifikasi:
    // 1. Uang terbanyak
    // 2. Jumlah properti terbanyak
    // 3. Jumlah kartu terbanyak
    std::sort(active.begin(), active.end(), [](Player* a, Player* b) {
        if (a->getCash() != b->getCash()) return a->getCash() > b->getCash();
        if (a->getOwnedProperties().size() != b->getOwnedProperties().size())
            return a->getOwnedProperties().size() > b->getOwnedProperties().size();
        return a->getOwnedSkillCards().size() > b->getOwnedSkillCards().size();
    });

    // Tampilkan rekap
    renderer->printInfo("\nRekap pemain:");
    for (Player* p : active) {
        renderer->printInfo(p->getUsername());
        renderer->printInfo("  Uang     : M" + std::to_string(p->getCash()));
        renderer->printInfo("  Properti : " + std::to_string(p->getOwnedProperties().size()));
        renderer->printInfo("  Kartu    : " + std::to_string(p->getOwnedSkillCards().size()));
    }

    // Tentukan pemenang (bisa lebih dari satu kalau seri terus)
    std::vector<Player*> winners;
    winners.push_back(active[0]);
    for (int i = 1; i < (int)active.size(); i++) {
        Player* a = active[0];
        Player* b = active[i];
        if (a->getCash() == b->getCash() &&
            a->getOwnedProperties().size() == b->getOwnedProperties().size() &&
            a->getOwnedSkillCards().size() == b->getOwnedSkillCards().size()) {
            winners.push_back(b);
        } else {
            break;
        }
    }

    if (winners.size() == 1) {
        renderer->printWinner(*winners[0], turnsPlayed);
    } else {
        renderer->printInfo("Pemenang (seri):");
        for (Player* w : winners) {
            renderer->printInfo("- " + w->getUsername());
        }
    }

    logger->addLog("Permainan selesai pada turn " + std::to_string(turnsPlayed));
    gameActive = false;
}