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
#include "utils/SaveLoadManager.hpp"
#include "views/CLIRenderer.hpp"
#include <iostream>
#include <algorithm>
#include <random>

// ===== INISIALISASI =====
Game::Game()
    : gameActive(false),
      currentPlayerIndex(0),
      turnsPlayed(0),
      maxTurn(15),
      goSalary(200),
      jailFine(50), 
      pphFlat(150), 
      pphPersen(10),
      pbmFlat(200),
      isAuctionActive(false),
      renderer(nullptr)
{
}

Game::~Game() = default;

// Inisialisasi awal
void Game::initialize()
{
    board = std::make_unique<Board>();
    board->initializeBoard("config/");
    dice = std::make_unique<Dice>();
    logger = std::make_unique<TransactionLogger>();
    bankruptcyManager = std::make_unique<BankruptcyManager>();
    auctionManager = std::make_unique<AuctionManager>(this);

    initializeDecks();
    initializeConfig();

    gameActive = true;
    logger->addLog("Permainan Nimonspoli telah dimulai!");
}

// Inisialisasi deck
void Game::initializeDecks() {
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

    // Shuffle kartunya
    chanceDeck->shuffle();
    generalFundsDeck->shuffle();
    skillCardDeck->shuffle();
}

// Inisialiasi config
void Game::initializeConfig() {
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
}

// Inisialisasi dari save
void Game::initializeFromSave(const std::string& saveFile) {
    board = std::make_unique<Board>();
    board->initializeBoard("config/");
    dice = std::make_unique<Dice>();
    logger = std::make_unique<TransactionLogger>();
    bankruptcyManager = std::make_unique<BankruptcyManager>();
    auctionManager = std::make_unique<AuctionManager>(this);

    initializeDecks();
    initializeConfig();

    // Load state dari file
    SaveLoadManager slm;
    std::vector<Player*> rawPlayers;
    slm.loadGame(saveFile, turnsPlayed, maxTurn, rawPlayers, turnOrder, currentPlayerIndex, *board, *logger);

    players.clear();
    for (Player* p : rawPlayers) {
        players.push_back(std::unique_ptr<Player>(p));
    }

    gameActive = true;
    logger->addLog("Permainan dimuat dari: " + saveFile);
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

// Buat looping utama (state game aktif)
bool Game::isGameActive() const
{
    return gameActive;
}

// Jadiin game aktif (kepake di command QUIT)
void Game::setGameActive(bool active)
{
    gameActive = active;
}

// ===== DADU =====
// Lempar dadu
void Game::rollDice() {
    Player* player = getCurrentPlayer();
    if (!player) return;

    if (player->hasRolledDiceThisTurn()) {
        renderer->printError("Kamu sudah melempar dadu! Ketik AKHIRI_GILIRAN untuk selesai.");
        return;
    }

    // Handle jail turn
    if (player->isJailed()) {
        int turnsLeft = player->getJailTurns();

        // Giliran ke-4 — wajib bayar denda dulu
        if (turnsLeft == 0) {
            renderer->printError("Kamu wajib keluar penjara! Gunakan BAYAR_DENDA atau GUNAKAN_KARTU_BEBAS.");
            return;
        }
    }

    int result = dice->roll();
    int d1 = dice->getDice1();
    int d2 = dice->getDice2();

    player->setRolledDiceThisTurn(true);

    renderer->printInfo("Mengocok dadu...");
    renderer->printInfo("Hasil: " + std::to_string(d1) + " + " + std::to_string(d2) + " = " + std::to_string(result));
    renderer->printInfo("Memajukan bidak " + player->getUsername() + " sebanyak " + std::to_string(result) + " petak...");

    logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " + player->getUsername() + " | DADU | Lempar: " + std::to_string(d1) + "+" + std::to_string(d2) + "=" + std::to_string(result));

    // Kalau d1 dan d2 sama
    if (dice->isDouble()) {
        player->setConsecutiveDoublesDice(player->getConsecutiveDoublesDice() + 1);
        if (player->getConsecutiveDoublesDice() >= 3) {
            // Double 3x berturut: langsung ke penjara
            player->setConsecutiveDoublesDice(0);
            player->goToJail();
            logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " + player->getUsername() + " | PENJARA | Double 3x, masuk penjara!");
            return;
        } else {
            player->setRolledDiceThisTurn(false);
            renderer->printInfo("Kamu mendapat DOUBLE! Kamu berhak melempar dadu lagi.");
        }
    } else {
        player->setConsecutiveDoublesDice(0);
    }

    movePlayer(*player, result);
}

// Atur Dadu (manual)
void Game::setDice(int x, int y) {
    Player* player = getCurrentPlayer();
    if (!player) return;

    if (player->hasRolledDiceThisTurn()) {
        renderer->printError("Kamu sudah melempar dadu! Ketik AKHIRI_GILIRAN untuk selesai.");
        return;
    }

    dice->setDice(x, y);
    int result = dice->getTotal();

    player->setRolledDiceThisTurn(true);

    logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " + player->getUsername() + " | DADU | Diatur manual: " + std::to_string(x) + "+" + std::to_string(y) + "=" + std::to_string(result));

    movePlayer(*player, result);
}

// ===== PROPERTY =====
// Gadai properti
void Game::mortgageProperty() {
    Player* player = getCurrentPlayer();
    if (!player) return;

    // Kumpulkan properti yang bisa digadaikan
    std::vector<PropertyTile*> mortgageable;
    for (auto* prop : player->getOwnedProperties()) {
        if (!prop->isMortgaged()) {
            mortgageable.push_back(prop);
        }
    }

    if (mortgageable.empty()) {
        renderer->printInfo("Tidak ada properti yang dapat digadaikan saat ini.");
        return;
    }

    // Tampilkan daftar
    renderer->printInfo("===== Properti yang Dapat Digadaikan =====");
    for (int i = 0; i < (int)mortgageable.size(); i++) {
        auto* prop = mortgageable[i];
        renderer->printInfo(std::to_string(i + 1) + ". " + prop->getName() + " (" + prop->getCode() + ") Nilai Gadai: M" + std::to_string(prop->getMortgageValue()));
    }

    // Prompt pilih nomor
    renderer->printInfo("Pilih nomor properti (0 untuk batal): ");
    int choice;
    while (true) {
        std::string input;
        std::getline(std::cin, input);
        try {
            choice = std::stoi(input);
            if (choice >= 0 && choice <= (int)mortgageable.size()) break;
        } catch (...) {}
        renderer->printInfo("Masukkan angka yang valid: ");
    }

    if (choice == 0) return;

    PropertyTile* target = mortgageable[choice - 1];

    // Cek bangunan di color group
    StreetTile* st = dynamic_cast<StreetTile*>(target);
    if (st) {
        auto group = board->getPropertiesByColor(st->getColorGroup());
        bool hasBangunan = false;
        for (auto* prop : group) {
            StreetTile* s = dynamic_cast<StreetTile*>(prop);
            if (s && s->getBuildingLevel() > 0) {
                hasBangunan = true;
                break;
            }
        }

        if (hasBangunan) {
            renderer->printInfo(target->getName() + " tidak dapat digadaikan!");
            renderer->printInfo("Masih terdapat bangunan di color group [" + st->getColorGroup() + "].");
            renderer->printInfo("Bangunan harus dijual terlebih dahulu.\n\n");

            // Tampilkan daftar bangunan di color group
            renderer->printInfo("Daftar bangunan di color group [" + st->getColorGroup() + "]:");
            for (auto* prop : group) {
                StreetTile* s = dynamic_cast<StreetTile*>(prop);
                if (s && s->getBuildingLevel() > 0) {
                    renderer->printInfo("- " + s->getName() + " - " +  std::to_string(s->getBuildingLevel()) + " rumah -> Nilai jual bangunan: M" +  std::to_string(s->getBuildingSaleValue()));
                }
            }

            renderer->printInfo("Jual semua bangunan color group [" + st->getColorGroup() + "]? (y/n): ");
            std::string input;
            while (true) {
                std::getline(std::cin, input);
                if (input == "y" || input == "Y") break;
                else if (input == "n" || input == "N") return;
                else renderer->printInfo("Masukkan y atau n: ");
            }

            // Jual semua bangunan
            for (auto* prop : group) {
                StreetTile* s = dynamic_cast<StreetTile*>(prop);
                if (s && s->getBuildingLevel() > 0) {
                    int saleVal = s->getBuildingSaleValue();
                    s->resetBuildings();
                    *player += saleVal;
                    renderer->printInfo("Bangunan " + s->getName() + " terjual. Kamu menerima M" + std::to_string(saleVal) + ".");
                }
            }

            // Konfirmasi lanjut gadai
            renderer->printInfo("Lanjut menggadaikan " + target->getName() + "? (y/n): ");
            while (true) {
                std::getline(std::cin, input);
                if (input == "y" || input == "Y") break;
                else if (input == "n" || input == "N") return;
                else renderer->printInfo("Masukkan y atau n: ");
            }
        }
    }

    // Gadai benerannya
    int mortgageVal = target->getMortgageValue();
    target->mortgage();
    *player += mortgageVal;

    renderer->printInfo(target->getName() + " berhasil digadaikan.");
    renderer->printInfo("Kamu menerima M" + std::to_string(mortgageVal) + " dari Bank.");
    renderer->printInfo("Uang kamu sekarang: M" + std::to_string(player->getCash()));
    renderer->printInfo("Catatan: Sewa tidak dapat dipungut dari properti yang digadaikan.");

    logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " +  player->getUsername() + " | GADAI | " +  target->getName() + " digadaikan, terima M" + std::to_string(mortgageVal));
}

// Tebus properti
void Game::redeemProperty(const std::string& tileCode) {
    Player* player = getCurrentPlayer();
    if (!player) return;

    PropertyTile* target = nullptr;
    for (auto* prop : player->getOwnedProperties()) {
        if (prop->getCode() == tileCode) {
            target = prop;
            break;
        }
    }

    if (!target) {
        renderer->printError("Properti " + tileCode + " tidak ditemukan atau bukan milikmu.");
        return;
    }

    if (!target->isMortgaged()) {
        renderer->printError("Properti " + tileCode + " tidak sedang digadaikan.");
        return;
    }

    int redeemPrice = target->getPrice();
    if (player->getCash() < redeemPrice) {
        renderer->printError("Uang tidak cukup untuk menebus " + target->getName() + ".");
        renderer->printInfo("Harga tebus: M" + std::to_string(redeemPrice) + " | Uang kamu: M" + std::to_string(player->getCash()));
        return;
    }

    *player -= redeemPrice;
    target->redeem();

    renderer->printInfo(target->getName() + " berhasil ditebus!");
    renderer->printInfo("Kamu membayar M" + std::to_string(redeemPrice) + " ke Bank.");
    renderer->printInfo("Uang kamu sekarang: M" + std::to_string(player->getCash()));

    logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " + player->getUsername() + " | TEBUS | " + tileCode + " ditebus seharga M" + std::to_string(redeemPrice));
}

// Bangun rumah atau upgrade ke hotel
void Game::buildOnProperty(const std::string& tileCode) {
    Player* player = getCurrentPlayer();
    if (!player) return;

    // Kumpulkan color group yang memenuhi syarat
    std::vector<std::string> eligibleGroups;
    for (auto* prop : player->getOwnedProperties()) {
        StreetTile* st = dynamic_cast<StreetTile*>(prop);
        if (!st) continue;
        if (st->isMortgaged()) continue;

        std::string color = st->getColorGroup();
        // Cek apakah sudah masuk list
        bool alreadyAdded = false;
        for (const auto& g : eligibleGroups) {
            if (g == color) { alreadyAdded = true; break; }
        }
        if (alreadyAdded) continue;

        // Cek monopoli
        if (st->isMonopolized()) {
            eligibleGroups.push_back(color);
        }
    }

    if (eligibleGroups.empty()) {
        renderer->printError("Tidak ada color group yang memenuhi syarat untuk dibangun.");
        return;
    }

    // Tampilkan pilihan color group
    renderer->printInfo("=== Color Group yang Memenuhi Syarat ===");
    for (int i = 0; i < (int)eligibleGroups.size(); i++) {
        renderer->printInfo(std::to_string(i + 1) + ". [" + eligibleGroups[i] + "]");
        auto group = board->getPropertiesByColor(eligibleGroups[i]);
        for (auto* prop : group) {
            StreetTile* st = dynamic_cast<StreetTile*>(prop);
            if (!st) continue;
            std::string level = st->getBuildingLevel() == 5 ? "Hotel" : std::to_string(st->getBuildingLevel()) + " rumah";
            renderer->printInfo("   - " + st->getName() + " (" + st->getCode() + ") : " + level);
        }
    }
    renderer->printInfo("Uang kamu saat ini: M" + std::to_string(player->getCash()));
    std::cout << "Pilih nomor color group (0 untuk batal): ";

    std::string input;
    std::getline(std::cin, input);
    int groupChoice = -1;
    try { groupChoice = std::stoi(input) - 1; } catch (...) { return; }
    if (groupChoice < 0 || groupChoice >= (int)eligibleGroups.size()) return;

    std::string chosenColor = eligibleGroups[groupChoice];
    auto group = board->getPropertiesByColor(chosenColor);

    // Tampilkan petak yang bisa dibangun
    renderer->printInfo("Color group [" + chosenColor + "]:");
    std::vector<StreetTile*> buildable;
    for (auto* prop : group) {
        StreetTile* st = dynamic_cast<StreetTile*>(prop);
        if (!st) continue;
        std::string level = st->getBuildingLevel() == 5 ? "Hotel" : std::to_string(st->getBuildingLevel()) + " rumah";
        std::string canBuild = st->canBuild() ? " <- dapat dibangun" : "";
        renderer->printInfo("   - " + st->getName() + " (" + st->getCode() + ") : " + level + canBuild);
        if (st->canBuild()) buildable.push_back(st);
    }

    if (buildable.empty()) {
        renderer->printError("Tidak ada petak yang bisa dibangun sekarang.");
        return;
    }

    std::cout << "Pilih petak (0 untuk batal): ";
    std::getline(std::cin, input);
    int tileChoice = -1;
    try { tileChoice = std::stoi(input) - 1; } catch (...) { return; }
    if (tileChoice < 0 || tileChoice >= (int)buildable.size()) return;

    StreetTile* chosen = buildable[tileChoice];
    int cost = chosen->getBuildingLevel() == 4 ? chosen->getHotelCost() : chosen->getHouseCost();

    if (player->getCash() < cost) {
        renderer->printError("Uang tidak cukup. Butuh M" + std::to_string(cost));
        return;
    }

    *player -= cost;
    chosen->build();

    std::string result = chosen->getBuildingLevel() == 5 ? "Hotel" : std::to_string(chosen->getBuildingLevel()) + " rumah";
    renderer->printInfo("Kamu membangun di " + chosen->getName() + ". Biaya: M" + std::to_string(cost));
    renderer->printInfo(chosen->getName() + " : " + result);
    renderer->printInfo("Uang tersisa: M" + std::to_string(player->getCash()));

    logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " + player->getUsername() + " | BANGUN | " + chosen->getCode() + " -> " + result + ", bayar M" + std::to_string(cost));
}

// ===== AUCTION =====
// Kasih tawaran
void Game::placeBid(int amount) {
    if (!isAuctionActive) {
        renderer->printError("Tidak ada lelang yang sedang berlangsung.");
        return;
    }

    if (amount <= 0) {
        renderer->printError("Jumlah tawaran harus lebih dari 0.");
        return;
    }

    Player* player = getCurrentPlayer();
    if (!player) return;

    try {
        auctionManager->processBid(player, amount);
        renderer->printInfo("Penawaran tertinggi: M" + std::to_string(auctionManager->getWinningBid()) + " (" + player->getUsername() + ")");

        if (auctionManager->isFinished()) {
            isAuctionActive = false;
        }
    } catch (const NimonspoliException& e) {
        renderer->printError(e.what());
    }
}

// Skip lelang
void Game::passAuction() {
    if (!isAuctionActive) {
        renderer->printError("Tidak ada lelang yang sedang berlangsung.");
        return;
    }

    Player* player = getCurrentPlayer();
    if (!player) return;

    try {
        auctionManager->processPass(player);

        if (auctionManager->isFinished()) {
            isAuctionActive = false;
        }
    } catch (const NimonspoliException& e) {
        renderer->printError(e.what());
    }
}

// Kondisi akhir lelang
void Game::finalizeAuction(Player* winner, PropertyTile* property, int winningBid) {
    if (!property) return;

    if (!winner) {
        renderer->printInfo("Lelang gagal: " + property->getCode() + " tidak terjual.");
        isAuctionActive = false;
        return;
    }

    *winner -= winningBid;
    property->changeOwner(winner);
    property->redeem();
    winner->addProperty(property);

    renderer->printInfo("Lelang selesai!");
    renderer->printInfo("Pemenang: " + winner->getUsername());
    renderer->printInfo("Harga akhir: M" + std::to_string(winningBid));
    renderer->printInfo("Properti " + property->getCode() + " kini dimiliki " + winner->getUsername());

    logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " + winner->getUsername() + " | LELANG | Menang lelang " + property->getCode() + " seharga M" + std::to_string(winningBid));

    isAuctionActive = false;
}

void Game::logAuctionEvent(const std::string& action, const std::string& detail) {
    logger->addLog("[AUCTION] " + action + ": " + detail);
}

// Mulai lelang (private punya Game)
void Game::startAuctionForProperty(PropertyTile& tile) {
    std::vector<Player*> active = getActivePlayers();
    Player* trigger = getCurrentPlayer();

    isAuctionActive = true;

    auctionManager->startAuction(&tile, trigger, active);

    renderer->printInfo("Properti " + tile.getName() + " (" + tile.getCode() + ") akan dilelang!");
    renderer->printInfo("Urutan lelang dimulai dari pemain setelah " + trigger->getUsername() + ".");
}

// Mulai lelang (public)
void Game::triggerAuction(PropertyTile& property) {
    startAuctionForProperty(property);
}

// ===== SKILLCARD (KARTU KEMAMPUAN) =====
void Game::useSkillCard(int cardIndex) {
    Player* player = getCurrentPlayer();
    if (!player) return;

    if (player->hasUsedSkillThisTurn()) {
        renderer->printError("Kamu sudah menggunakan kartu kemampuan pada giliran ini!");
        renderer->printInfo("Penggunaan kartu dibatasi maksimal 1 kali dalam 1 giliran.");
        return;
    }

    auto cardNames = player->getSkillCardNames();
    if (cardIndex < 0 || cardIndex >= (int)cardNames.size()) {
        renderer->printError("Indeks kartu tidak valid.");
        return;
    }

    renderer->printInfo("Menggunakan " + cardNames[cardIndex] + "...");

    try {
        player->useSkillCard(cardIndex, *this);
        logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " + player->getUsername() + " | KARTU_KEMAMPUAN | Pakai " + cardNames[cardIndex]);
    } catch (const NimonspoliException& e) {
        renderer->printError(e.what());
    }
}

// ===== JAIL =====
void Game::payJailFine() {
    Player* player = getCurrentPlayer();
    if (!player) return;

    if (player->getStatus() != "JAILED") {
        renderer->printError("Kamu tidak sedang di penjara.");
        return;
    }

    if (player->getCash() < jailFine) {
        renderer->printError("Uang tidak cukup untuk membayar denda M" + std::to_string(jailFine));
        return;
    }

    *player -= jailFine;
    player->releaseFromJail();

    renderer->printInfo("Kamu membayar denda M" + std::to_string(jailFine) + " ke Bank.");
    renderer->printInfo("Kamu bebas dari penjara!");
    renderer->printInfo("Uang kamu sekarang: M" + std::to_string(player->getCash()));

    logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " + player->getUsername() + " | BAYAR_DENDA | M" + std::to_string(jailFine));
}

// ===== GUNAKAN KARTU BEBAS =====
void Game::useJailFreeCard() {
    Player* player = getCurrentPlayer();
    if (!player) return;

    if (player->getStatus() != "JAILED") {
        renderer->printError("Kamu tidak sedang di penjara.");
        return;
    }

    // Cari kartu bebas penjara di tangan
    auto cardNames = player->getSkillCardNames();
    int freeCardIndex = -1;
    for (int i = 0; i < (int)cardNames.size(); i++) {
        if (cardNames[i] == "ShieldCard") {
            freeCardIndex = i;
            break;
        }
    }

    if (freeCardIndex < 0) {
        renderer->printError("Kamu tidak punya kartu bebas penjara.");
        return;
    }

    player->removeCard(freeCardIndex);
    player->releaseFromJail();

    renderer->printInfo("Kartu bebas penjara digunakan!");
    renderer->printInfo("Kamu bebas dari penjara!");

    logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " + player->getUsername() + " | KARTU_BEBAS | Keluar penjara dengan kartu.");
}

// ===== PLAYER =====
// Ambil pemain saat ini
Player *Game::getCurrentPlayer() const
{
    if (players.empty()) {
        return nullptr;
    }
    return players[turnOrder[currentPlayerIndex]].get();
}

std::string Game::getCurrentPlayerName() const {
    Player* p = getCurrentPlayer();
    return p ? p->getUsername() : "???";
}

// Ngitung jumlah pemain aktif
int Game::countActivePlayers() const {
    int count = 0;
    for (const auto& p : players) {
        if (p && p->getStatus() != "BANKRUPT") count++;
    }
    return count;
}

// Ambil daftar pemain yang aktif
std::vector<Player*> Game::getActivePlayers() const {
    std::vector<Player*> result;
    for (const auto& p : players) {
        if (p && p->getStatus() != "BANKRUPT") {
            result.push_back(p.get());
        }
    }
    return result;
}

// Ngegerakin pemain
void Game::movePlayer(Player& player, int steps) {
    int startPos = player.getPosition();
    int boardSize = board->getTileCount();
    int newPos = (startPos + steps) % boardSize;

    if ((startPos + steps) >= boardSize) {
        player += goSalary;
        renderer->printInfo("Melewati GO! Dapat M" + std::to_string(goSalary) + ". Uang sekarang: M" + std::to_string(player.getCash()));
        logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " + player.getUsername() + " | GO | Melewati GO, dapat M" + std::to_string(goSalary));
    }

    player.setPosition(newPos);
    Tile* tile = board->getTile(newPos);
    std::string tileName = tile ? tile->getName() : "???";
    std::string tileCode = tile ? tile->getCode() : "???";
    logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " + player.getUsername() + " | GERAK | Mendarat di " + tileName);

    processTileLanding(player, newPos);
}

// Teleport ke tile tujuan (dipake sama SkillCard)
void Game::teleportPlayer(Player& player, int targetTileIndex) {
    if (!board->isValidPosition(targetTileIndex)) {
        renderer->printError("Posisi tidak valid.");
        return;
    }

    // Cek apakah melewati GO
    if (targetTileIndex < player.getPosition()) {
        player += goSalary;
        renderer->printInfo("Melewati GO! Dapat M" + std::to_string(goSalary));
    }

    player.setPosition(targetTileIndex);
    Tile* tile = board->getTile(targetTileIndex);
    renderer->printInfo("Bidak dipindahkan ke " + (tile ? tile->getName() : "???"));

    processTileLanding(player, targetTileIndex);
}

// ===== SAVE =====
void Game::saveGame(const std::string& filename) {
    // Cek apakah file sudah ada
    if (SaveLoadManager::fileExists(filename)) {
        std::cout << "File \"" << filename << "\" sudah ada. Timpa file lama? (y/n): ";
        std::string input;
        while (true) {
            std::getline(std::cin, input);
            if (input == "y" || input == "Y") {
                break;
            } else if (input == "n" || input == "N") {
                return;
            } else {
                renderer->printInfo("Masukkan y atau n: ");
            }
        }
    }

    SaveLoadManager slm;
    try {
        slm.saveGame(filename, turnsPlayed, maxTurn,  getActivePlayers(), turnOrder, currentPlayerIndex, *board, *skillCardDeck, *logger);
        renderer->printInfo("Permainan berhasil disimpan ke: " + filename);
        logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " + getCurrentPlayer()->getUsername() + " | SIMPAN | " + filename);
    } catch (const NimonspoliException& e) {
        renderer->printError(e.what());
    }
}

// ===== LOAD =====
void Game::loadGame(const std::string& filename) {
    SaveLoadManager slm;
    try {
        initialize();
        std::vector<Player*> rawPlayers;
        slm.loadGame(filename, turnsPlayed, maxTurn, rawPlayers, turnOrder, currentPlayerIndex, *board, *logger);

        // Pindahkan raw pointer ke unique_ptr
        players.clear();
        for (Player* p : rawPlayers) {
            players.push_back(std::unique_ptr<Player>(p));
        }

        renderer->printInfo("Permainan berhasil dimuat. Melanjutkan giliran " + getCurrentPlayer()->getUsername() + "...");
        logger->addLog("Permainan dimuat dari: " + filename);
    } catch (const NimonspoliException& e) {
        renderer->printError(e.what());
    }
}

// ===== DISPLAY =====
// Cetak papan
void Game::printBoard() {
    Player* current = getCurrentPlayer();
    int currentIdx;
    if (current) {
        currentIdx = turnOrder[currentPlayerIndex];
    } else {
        currentIdx = 0;
    }
    std::vector<Player*> rawPlayers = getActivePlayers();
    renderer->renderBoard(*board, rawPlayers, turnsPlayed, maxTurn, currentIdx);
}

// Cetak akta
void Game::printDeed() {
    renderer->printInfo("Masukkan kode petak: ");
    std::string tileCode;
    std::getline(std::cin, tileCode);
    
    for (int i = 0; i < board->getTileCount(); i++) {
        Tile* tile = board->getTile(i);
        if (tile && tile->getCode() == tileCode) {
            StreetTile* st = dynamic_cast<StreetTile*>(tile);
            RailroadTile* rr = dynamic_cast<RailroadTile*>(tile);
            UtilityTile* ut = dynamic_cast<UtilityTile*>(tile);

            if (st) {
                renderer->printDeed(*st);
                return;
            } else if (rr) {
                renderer->printDeed(*rr);
                return;
            } else if (ut) {
                renderer->printDeed(*ut);
                return;
            } else {
                renderer->printDeedNotFound(tileCode);
                return;
            }
        }
    }
    renderer->printDeedNotFound(tileCode);
}

// Cetak properti
void Game::printPropertyInventory() {
    Player* player = getCurrentPlayer();
    if (!player) return;
    renderer->printPropertyInventory(*player);
}

// Cetak status pemain
void Game::printPlayerStatus() {
    Player* player = getCurrentPlayer();
    if (!player) return;
    renderer->printPlayerStatus(*player, *board);
}

// Cetak log
void Game::printLog(int limit) {
    logger->printLog(limit);
}

void Game::printHelp() { 
    std::cout << "=================================================================\n";
    std::cout << "                 DAFTAR PERINTAH NIMONSPOLI                      \n";
    std::cout << "=================================================================\n\n";

    std::cout << "[AKSI GILIRAN]\n";
    std::cout << "  LEMPAR_DADU         : Melempar dadu untuk bergerak ke petak baru.\n";
    std::cout << "  ATUR_DADU <x> <y>   : [Cheat] Mengatur angka dadu secara manual untuk testing.\n";
    std::cout << "  AKHIRI_GILIRAN      : Mengakhiri giliranmu dan lanjut ke pemain berikutnya.\n\n";

    std::cout << "[PROPERTI & BANGUNAN]\n";
    std::cout << "  BANGUN <petak>     : Membangun rumah/hotel di properti milikmu.\n";

    std::cout << "[MANAJEMEN ASET & LELANG]\n";
    std::cout << "  GADAI <petak> : Menggadaikan properti untuk mendapatkan uang.\n";
    std::cout << "  TEBUS <petak> : Menebus properti yang sedang digadaikan.\n";
    std::cout << "  TAWAR <harga> : Mengajukan harga saat sesi lelang properti.\n";
    std::cout << "  LEPAS         : Mundur dari sesi lelang saat ini.\n";

    std::cout << "[PENJARA & KARTU]\n";
    std::cout << "  GUNAKAN_KARTU_BEBAS         : Menggunakan kartu khusus untuk bebas.\n";
    std::cout << "  GUNAKAN_KEMAMPUAN <index>   : Menggunakan kartu skill (kemampuan) yang dimiliki.\n\n";

    std::cout << "[INFORMASI]\n";
    std::cout << "  CETAK_STATUS   : Melihat info saldo, posisi, dan statusmu saat ini.\n";
    std::cout << "  CETAK_PAPAN    : Menampilkan kondisi papan Nimonspoli saat ini.\n";
    std::cout << "  CETAK_PROPERTI : Menampilkan daftar seluruh properti yang kamu miliki.\n";
    std::cout << "  CETAK_AKTA <p> : Melihat detail harga, biaya sewa, dan status sebuah properti.\n";
    std::cout << "  CETAK_LOG      : Melihat riwayat aksi yang sudah terjadi di dalam game.\n";
    std::cout << "  BANTUAN        : Menampilkan menu daftar perintah ini.\n\n";

    std::cout << "[SISTEM]\n";
    std::cout << "  SIMPAN <file>   : Menyimpan progres permainan (contoh: SIMPAN game1.txt).\n";
    std::cout << "  MUAT <file>     : Memuat progres permainan dari file penyimpanan.\n";
    std::cout << "  QUIT            : Keluar dari aplikasi Nimonspoli sepenuhnya.\n\n";
    
    std::cout << "=================================================================\n";
}

void Game::setRenderer(CLIRenderer* r) {
    renderer = r;
}

// ===== FLOW GILIRAN =====
// Giliran berakhir
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
    if (dice->isDouble() && player->getStatus() != "JAILED" && player->getConsecutiveDoublesDice() > 0) {
        renderer->printInfo("Kamu mendapat double! Giliran tambahan.");
        logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " + player->getUsername() + " | DOUBLE | Giliran tambahan.");
        return;
    }

    nextTurn();
}

// Giliran berikutnya
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
    while (getCurrentPlayer() && getCurrentPlayer()->getStatus() == "BANKRUPT" && attempts < (int)players.size()) {
        currentPlayerIndex = (currentPlayerIndex + 1) % turnOrder.size();
        attempts++;
    }

    // Bagi kartu skill di awal giliran
    Player* next = getCurrentPlayer();
    if (next) {
        drawSkillCard(*next);
        logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " + next->getUsername() + " | GILIRAN | Giliran dimulai.");
    }
}

// Kondisi game selesai
void Game::handleGameEnd() {
    renderer->printInfo("\nPermainan selesai! (Batas giliran tercapai)");

    // Kumpulkan pemain aktif
    std::vector<Player*> active = getActivePlayers();

    // Urutan menang: uang terbanyak, properti terbanyak, kartu terbanyak
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

    // Tentukan pemenang (bisa lebih dari satu kalau seri)
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

// Bangkrut
void Game::declareBankruptcy() {
    Player* player = getCurrentPlayer();
    if (!player) return;

    renderer->printInfo(player->getUsername() + " menyatakan bangkrut!");

    bankruptcyManager->handle(*player, nullptr, 0, *this,
        [this](const std::vector<LiquidationOption>& options) -> int {
            renderer->printInfo("\n=== Panel Likuidasi ===");
            for (int i = 0; i < (int)options.size(); i++) {
                renderer->printInfo(std::to_string(i + 1) + ". " + options[i].getDescription());
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

    logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " + player->getUsername() + " | BANGKRUT | Menyerah dari permainan.");

    if (countActivePlayers() <= 1) {
        std::vector<Player*> active = getActivePlayers();
        if (!active.empty()) {
            renderer->printInfo("\nPermainan selesai! (Semua pemain kecuali satu bangkrut)");
            renderer->printWinner(*active[0], turnsPlayed);
            logger->addLog("Permainan selesai. Pemenang: " + active[0]->getUsername());
        }
        gameActive = false;
        return;
    }

    nextTurn();
}

// ===== GETTER =====
// Dapetin papan
Board& Game::getBoard() const
{
    return *board;
}

// Dapetin log
TransactionLogger *Game::getLogger()
{
    return logger.get();
}

// ===== SEWA =====
// Ketika berdiri di tiap jenis tile
void Game::processTileLanding(Player& player, int tileIndex) {
    Tile* tile = board->getTile(tileIndex);
    if (!tile) return;

    StreetTile* street = dynamic_cast<StreetTile*>(tile);
    if (street) { handleStreetLanding(player, *street); return; }

    RailroadTile* railroad = dynamic_cast<RailroadTile*>(tile);
    if (railroad) { handleRailroadLanding(player, *railroad); return; }

    UtilityTile* utility = dynamic_cast<UtilityTile*>(tile);
    if (utility) { handleUtilityLanding(player, *utility); return; }

    // Sisanya udah (Tax, Festival, Card, Special) dari onLanded() masing-masing
    tile->onLanded(player, *this);
}

// processTileLanding versi public (kayak getter)
void Game::processTileLandingPublic(Player& player, int tileIndex) {
    processTileLanding(player, tileIndex);
}

// Street
void Game::handleStreetLanding(Player& player, StreetTile& tile) {
    PropertyStatus status = tile.getStatus();

    // Kalau statusnya BANK, tawarin pembelian
    if (status == PropertyStatus::BANK) {
        renderer->printInfo("Kamu mendarat di " + tile.getName() + " (" + tile.getCode() + ")!");
        renderer->printDeed(tile);
        renderer->printInfo("Uang kamu saat ini: M" + std::to_string(player.getCash()));
        std::cout << "Apakah kamu ingin membeli properti ini seharga M" << tile.getPrice() << "? (y/n): ";

        std::string input;
        while (true) {
            std::getline(std::cin, input);
            if (input == "y" || input == "Y" || input == "n" || input == "N") {
                break;
            } else {
                std::cout << "Input tidak valid. Masukkan y atau n: ";
            }
        }

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
            logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " + player.getUsername() + " | BELI | Beli " + tile.getName() + " (" + tile.getCode() + ") seharga M" + std::to_string(tile.getPrice()));
            renderer->printInfo(tile.getName() + " kini menjadi milikmu!");
            renderer->printInfo("Uang tersisa: M" + std::to_string(player.getCash()));
        } else {
            renderer->printInfo("Properti ini akan masuk ke sistem lelang...");
            startAuctionForProperty(tile);
        }
        return;
    }

    // Kalau milik sendiri (termasuk yang digadaikan), tidak ada aksi
    if (tile.getOwner() == &player) {
        if (status == PropertyStatus::MORTGAGED) {
            renderer->printInfo("Kamu mendarat di properti milikmu sendiri: " + tile.getName() + " (sedang digadaikan).");
        } else {
            renderer->printInfo("Kamu mendarat di properti milikmu sendiri: " + tile.getName());
        }
        return;
    }

    // Kalau digadaikan milik orang lain, tidak ada sewa
    if (status == PropertyStatus::MORTGAGED) {
        renderer->printInfo("Kamu mendarat di " + tile.getName() + " (" + tile.getCode() + "), milik " + tile.getOwner()->getUsername() + ".");
        renderer->printInfo("Properti ini sedang digadaikan [M]. Tidak ada sewa yang dikenakan.");
        return;
    }

    // Kalau punya pemain lain, bayar sewa
    if (status == PropertyStatus::OWNED && tile.getOwner() != &player) {
        applyRent(player, tile);
        return;
    }
}

// Railroad
void Game::handleRailroadLanding(Player& player, RailroadTile& tile) {
    // Status BANK: otomatis jadi milik player
    if (tile.getStatus() == PropertyStatus::BANK) {
        tile.changeOwner(&player);
        tile.redeem();
        player.addProperty(&tile);
        renderer->printInfo("Kamu mendarat di " + tile.getName() + "!");
        renderer->printInfo("Belum ada yang menginjaknya duluan, stasiun ini kini menjadi milikmu!");
        logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " + player.getUsername() + " | RAILROAD | " + tile.getCode() + " kini milik " + player.getUsername() + " (otomatis)");
        return;
    }

    // Milik sendiri (termasuk yang digadaikan): tidak ada aksi
    if (tile.getOwner() == &player) {
        if (tile.getStatus() == PropertyStatus::MORTGAGED) {
            renderer->printInfo("Kamu mendarat di stasiun milikmu sendiri: " + tile.getName() + " (sedang digadaikan).");
        } else {
            renderer->printInfo("Kamu mendarat di stasiun milikmu sendiri: " + tile.getName() + ".");
        }
        return;
    }

    // Digadaikan milik orang lain: tidak ada sewa
    if (tile.getStatus() == PropertyStatus::MORTGAGED) {
        renderer->printInfo("Kamu mendarat di " + tile.getName() + ", milik " + tile.getOwner()->getUsername() + ".");
        renderer->printInfo("Stasiun ini sedang digadaikan [M]. Tidak ada sewa yang dikenakan.");
        return;
    }

    // Dimiliki pemain lain: bayar sewa
    if (tile.getStatus() == PropertyStatus::OWNED) {
        applyRent(player, tile);
        return;
    }
}

// Utility
void Game::handleUtilityLanding(Player& player, UtilityTile& tile) {
    // Status BANK: otomatis jadi milik player
    if (tile.getStatus() == PropertyStatus::BANK) {
        tile.changeOwner(&player);
        tile.redeem();
        player.addProperty(&tile);
        renderer->printInfo("Kamu mendarat di " + tile.getName() + "!");
        renderer->printInfo("Belum ada yang menginjaknya duluan, " + tile.getName() + " kini menjadi milikmu!");
        logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " + player.getUsername() + " | UTILITY | " + tile.getCode() + " kini milik " + player.getUsername() + " (otomatis)");
        return;
    }

    // Milik sendiri (termasuk yang digadaikan): tidak ada aksi
    if (tile.getOwner() == &player) {
        if (tile.getStatus() == PropertyStatus::MORTGAGED) {
            renderer->printInfo("Kamu mendarat di " + tile.getName() + " milikmu sendiri (sedang digadaikan).");
        } else {
            renderer->printInfo("Kamu mendarat di " + tile.getName() + " milikmu sendiri.");
        }
        return;
    }

    // Digadaikan milik orang lain: tidak ada sewa
    if (tile.getStatus() == PropertyStatus::MORTGAGED) {
        renderer->printInfo("Kamu mendarat di " + tile.getName() + ", milik " + tile.getOwner()->getUsername() + ".");
        renderer->printInfo("Utilitas ini sedang digadaikan [M]. Tidak ada sewa yang dikenakan.");
        return;
    }

    // Dimiliki pemain lain: bayar sewa
    if (tile.getStatus() == PropertyStatus::OWNED) {
        applyRent(player, tile);
        return;
    }
}

// Helper buat trigger sewa
void Game::applyRent(Player& player, PropertyTile& tile) {
    if (player.isShieldActive()) {
        renderer->printInfo("[SHIELD ACTIVE] Kamu terlindungi dari sewa!");
        return;
    }

    int rent = tile.calculateRent(dice->getTotal());
    Player* owner = tile.getOwner();

    renderer->printInfo("Kamu mendarat di " + tile.getName() + " (" + tile.getCode() + "), milik " + owner->getUsername() + "!");
    renderer->printInfo("Sewa: M" + std::to_string(rent));

    try {
        player.payRent(rent, *owner);
        logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " + player.getUsername() + " | SEWA | Bayar M" + std::to_string(rent) + " ke " + owner->getUsername() + " (" + tile.getCode() + ")");
        renderer->printInfo("Uang kamu: M" + std::to_string(player.getCash()));
        renderer->printInfo("Uang " + owner->getUsername() + ": M" + std::to_string(owner->getCash()));
    } catch (const NimonspoliException&) {
        // Tidak cukup bayar — masuk alur bankruptcy
        bankruptcyManager->handle(player, owner, rent, *this,
            [this](const std::vector<LiquidationOption>& options) -> int {
                renderer->printInfo("\n=== Panel Likuidasi ===");
                for (int i = 0; i < (int)options.size(); i++) {
                    renderer->printInfo(std::to_string(i + 1) + ". " + options[i].getDescription());
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

// ===== HELPER =====
void Game::drawSkillCard(Player& player) {
    SkillCard* card = skillCardDeck->draw();
    if (!card) return;

    auto cardNames = player.getSkillCardNames();
    if ((int)cardNames.size() >= 3) {
        // Slot penuh, wajib buang satu
        player.addCard(card);
        renderer->printInfo("Kamu mendapatkan 1 kartu acak baru!");
        renderer->printInfo("Kartu yang didapat: " + card->getName());
        renderer->printInfo("PERINGATAN: Kamu sudah memiliki 3 kartu di tangan (Maksimal 3)!");
        renderer->printInfo("Kamu diwajibkan membuang 1 kartu.");

        auto names = player.getSkillCardNames();
        renderer->printInfo("Daftar Kartu Kemampuan Anda:");
        for (int i = 0; i < (int)names.size(); i++) {
            renderer->printInfo(std::to_string(i + 1) + ". " + names[i]);
        }

        int dropIndex = -1;
        while (dropIndex < 0 || dropIndex >= (int)names.size()) {
            std::cout << "Pilih nomor kartu yang ingin dibuang (1-" << names.size() << "): ";
            std::string input;
            std::getline(std::cin, input);
            try {
                dropIndex = std::stoi(input) - 1;
            } catch (...) {
                dropIndex = -1;
            }
        }

        SkillCard* discarded = player.getOwnedSkillCards()[dropIndex];
        skillCardDeck->discard(discarded);
        player.removeCard(dropIndex);

        renderer->printInfo(names[dropIndex] + " telah dibuang.");
        logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " + player.getUsername() + " | DROP_KARTU | " + names[dropIndex]);
    } else {
        player.addCard(card);
        renderer->printInfo("Kamu mendapatkan kartu: " + card->getName());
    }

    logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " + player.getUsername() + " | DAPAT_KARTU | " + card->getName());
}