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

    // Cek giliran ke-4 setelah masuk penjara
    if (player->isJailed() && player->getJailTurns() == 0) {
        renderer->printError("Kamu wajib keluar penjara! Gunakan BAYAR_DENDA atau GUNAKAN_KARTU_BEBAS.");
        return;
    }

    int result = dice->roll();
    int d1 = dice->getDice1();
    int d2 = dice->getDice2();

    player->setRolledDiceThisTurn(true);

    renderer->printInfo("Mengocok dadu...");
    renderer->printInfo("Hasil: " + std::to_string(d1) + " + " + std::to_string(d2) + " = " + std::to_string(result));
    renderer->printInfo("Memajukan bidak " + player->getUsername() + " sebanyak " + std::to_string(result) + " petak...");

    // Handle jail turn
    if (player->isJailed()) {
        if (dice->isDouble()) {
            player->releaseFromJail();
            renderer->printInfo("Kamu mendapat double! Keluar dari penjara.");
            movePlayer(*player, result);
        } else {
            renderer->printInfo("Kamu tidak mendapat double. Tetap di penjara.");
            player->setRolledDiceThisTurn(true);
        }
        return;
    }


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

    if (player->isJailed()) {
        dice->setDice(x, y);
        int result = dice->getTotal();
        player->setRolledDiceThisTurn(true);

        if (dice->isDouble()) {
            player->releaseFromJail();
            renderer->printInfo("Kamu mendapat double! Keluar dari penjara.");
            movePlayer(*player, result);
        } else {
            renderer->printInfo("Kamu tidak mendapat double. Tetap di penjara.");
            // Tidak bergerak, hanya menghabiskan giliran
        }
        return;
    }

    dice->setDice(x, y);
    int result = dice->getTotal();
    player->setRolledDiceThisTurn(true);

    logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " + player->getUsername() + " | DADU | Diatur manual: " + std::to_string(x) + "+" + std::to_string(y) + "=" + std::to_string(result));

    if (dice->isDouble()) {
        player->setConsecutiveDoublesDice(player->getConsecutiveDoublesDice() + 1);
        if (player->getConsecutiveDoublesDice() >= 3) {
            player->setConsecutiveDoublesDice(0);
            player->goToJail();
            renderer->printInfo("Double 3x! Masuk penjara!");
            return;
        }
        player->setRolledDiceThisTurn(false);
        renderer->printInfo("Kamu mendapat DOUBLE! Kamu berhak melempar dadu lagi.");
    } else {
        player->setConsecutiveDoublesDice(0);
    }

    movePlayer(*player, result);
}

// ===== PROPERTY =====
// Jual semua bangunan (syarat sebelum gadai)
void Game::sellAllBuildingsInGroup(const std::string& colorGroup) {
    Player* player = getCurrentPlayer();
    if (!player) return;

    auto group = board->getPropertiesByColor(colorGroup);
    for (auto* prop : group) {
        StreetTile* st = dynamic_cast<StreetTile*>(prop);
        if (st && st->getBuildingLevel() > 0) {
            int saleVal = st->getBuildingSaleValue();
            st->resetBuildings();
            *player += saleVal;
            renderer->printInfo("Bangunan " + st->getName() + " terjual. Kamu menerima M" + std::to_string(saleVal) + ".");
        }
    }
    renderer->printInfo("Uang kamu sekarang: M" + std::to_string(player->getCash()));
}

// Gadai properti
void Game::mortgageProperty(const std::string& tileCode) {
    Player* player = getCurrentPlayer();
    if (!player) return;

    PropertyTile* target = nullptr;
    for (auto* prop : player->getOwnedProperties()) {
        if (prop->getCode() == tileCode) { target = prop; break; }
    }
    if (!target) return;

    int mortgageVal = target->getMortgageValue();
    target->mortgage(); // Ubah status jadi digadai
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
        if (prop->getCode() == tileCode) { target = prop; break; }
    }
    if (!target) return;

    int redeemPrice = target->getPrice();
    *player -= redeemPrice;
    target->redeem(); // Ubah status jadi owned

    renderer->printInfo(target->getName() + " berhasil ditebus!");
    renderer->printInfo("Kamu membayar M" + std::to_string(redeemPrice) + " ke Bank.");
    renderer->printInfo("Uang kamu sekarang: M" + std::to_string(player->getCash()));

    logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " + player->getUsername() + " | TEBUS | " + tileCode + " ditebus seharga M" + std::to_string(redeemPrice));
}

// Bangun rumah atau upgrade ke hotel
void Game::buildOnProperty(const std::string& tileCode) {
    Player* player = getCurrentPlayer();
    if (!player) return;

    PropertyTile* target = nullptr;
    for (auto* prop : player->getOwnedProperties()) {
        if (prop->getCode() == tileCode) { target = prop; break; }
    }

    StreetTile* chosen = dynamic_cast<StreetTile*>(target);
    if (!chosen) return;

    int cost = (chosen->getBuildingLevel() == 4) ? chosen->getHotelCost() : chosen->getHouseCost();

    *player -= cost;
    chosen->build();

    std::string result = chosen->getBuildingLevel() == 5 ? "Hotel" : std::to_string(chosen->getBuildingLevel()) + " rumah";
    
    if (chosen->getBuildingLevel() == 5) {
        renderer->printSuccess(chosen->getName() + " di-upgrade ke Hotel!");
    } else {
        renderer->printSuccess("Kamu membangun 1 rumah di " + chosen->getName() + ". Biaya: M" + std::to_string(cost));
    }
    
    renderer->printInfo("Uang tersisa: M" + std::to_string(player->getCash()));
    logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " + player->getUsername() + " | BANGUN | " + chosen->getCode() + " -> " + result);
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
        renderer->printInfo("Penawaran tertinggi: M" + std::to_string(auctionManager->getCurrentHighBid()) + " (" + player->getUsername() + ")");

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

    if (player->hasRolledDiceThisTurn()) {
        renderer->printError("Kartu kemampuan hanya bisa digunakan sebelum melempar dadu.");
        return;
    }

    if (player->hasUsedSkillThisTurn()) {
        renderer->printError("Kamu sudah menggunakan kartu kemampuan pada giliran ini!");
        return;
    }

    auto& cards = player->getSkillCards();
    if (cardIndex < 0 || cardIndex >= (int)cards.size()) {
        renderer->printError("Indeks kartu tidak valid.");
        return;
    }

    SkillCard* card = cards[cardIndex];
    std::string name = card->getName();

    if (name == "MoveCard") {
        applyMoveCard(dynamic_cast<MoveCard*>(card)->getValue());
    } else if (name == "DiscountCard") {
        applyDiscountCard(dynamic_cast<DiscountCard*>(card)->getValue());
    } else if (name == "ShieldCard") {
        applyShieldCard();
    } else if (name == "TeleportCard") {
        applyTeleportCard();
    } else if (name == "LassoCard") {
        applyLassoCard();
    } else if (name == "DemolitionCard") {
        applyDemolitionCard();
    }

    logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " + player->getUsername() + " | KARTU_KEMAMPUAN | Pakai " + name);
    player->removeCard(cardIndex);
    player->setUsedSkillThisTurn(true);
}

// Implementasi masing-masing jenis
void Game::applyMoveCard(int steps) {
    Player* player = getCurrentPlayer();
    if (!player) return;
    movePlayer(*player, steps);
}

void Game::applyDiscountCard(int percent) {
    Player* player = getCurrentPlayer();
    if (!player) return;
    player->activateDiscount(percent);
    renderer->printInfo("DiscountCard aktif! Diskon " + std::to_string(percent) + "% selama 1 giliran.");
}

void Game::applyShieldCard() {
    Player* player = getCurrentPlayer();
    if (!player) return;
    player->activateShield();
    renderer->printInfo("ShieldCard aktif! Kamu kebal tagihan selama 1 giliran.");
}

void Game::applyTeleportCard() {
    Player* player = getCurrentPlayer();
    if (!player) return;

    std::string kode = renderer->promptTileCode();

    Board& b = getBoard();
    for (int i = 0; i < b.getTileCount(); i++) {
        if (b.getTile(i) && b.getTile(i)->getCode() == kode) {
            teleportPlayer(*player, i);
            return;
        }
    }
    renderer->printError("Kode petak tidak ditemukan.");
}

void Game::applyLassoCard() {
    Player* player = getCurrentPlayer();
    if (!player) return;
    int myPos = player->getPosition();
    // Cari pemain di depan (posisi lebih besar, atau sudah wrap around)
    Player* target = nullptr;
    int minDist = 41;
    for (Player* other : getActivePlayers()) {
        if (other == player) continue;
        int dist = (other->getPosition() - myPos + 40) % 40;
        if (dist > 0 && dist < minDist) {
            minDist = dist;
            target = other;
        }
    }
    if (!target) {
        renderer->printError("Tidak ada pemain di depan.");
        return;
    }
    target->setPosition(myPos);
    renderer->printInfo(target->getUsername() + " ditarik ke posisi kamu (" + std::to_string(myPos) + ").");
}

void Game::applyDemolitionCard() {
    Player* player = getCurrentPlayer();
    if (!player) return;
    // Tampilkan properti lawan yang punya bangunan
    std::vector<StreetTile*> targets;
    for (Player* other : getActivePlayers()) {
        if (other == player) continue;
        for (auto* prop : other->getOwnedProperties()) {
            StreetTile* st = dynamic_cast<StreetTile*>(prop);
            if (st && st->getBuildingLevel() > 0) {
                targets.push_back(st);
            }
        }
    }
    if (targets.empty()) {
        renderer->printError("Tidak ada properti lawan yang memiliki bangunan.");
        return;
    }
    renderer->printInfo("Pilih properti yang ingin dihancurkan:");
    for (int i = 0; i < (int)targets.size(); i++) {
        renderer->printInfo(std::to_string(i+1) + ". " + targets[i]->getName() + " (level " + std::to_string(targets[i]->getBuildingLevel()) + ")");
    }
    int choice;
    std::string input;
    std::getline(std::cin, input);
    try {
        choice = std::stoi(input);
    } catch (...) {
        renderer->printError("Input tidak valid.");
        return;
    }
    if (choice < 1 || choice > (int)targets.size()) {
        renderer->printError("Pilihan tidak valid.");
        return;
    }
    StreetTile* chosen = targets[choice - 1];
    chosen->setBuildingLevel(chosen->getBuildingLevel() - 1);
    renderer->printInfo("Bangunan di " + chosen->getName() + " berhasil dihancurkan!");
    logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " + player->getUsername() + " | DEMOLITION | Hancurkan bangunan di " + chosen->getName());
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
    // Cuman bisa di awal
    if (getCurrentPlayer()->hasRolledDiceThisTurn()) {
        renderer->printError("SIMPAN hanya bisa dilakukan di awal giliran sebelum melempar dadu.");
        return;
    }

    // Cek apakah file sudah ada
    if (SaveLoadManager::fileExists("saves/" + filename)) {
        if (!renderer->promptYesNo("File \"saves/" + filename + "\" sudah ada. Timpa file lama?")) return;
    }

    SaveLoadManager slm;
    try {
        std::string fullPath = "saves/" + filename;
        slm.saveGame(fullPath, turnsPlayed, maxTurn,  getActivePlayers(), turnOrder, currentPlayerIndex, *board, *skillCardDeck, *logger);
        renderer->printInfo("Permainan berhasil disimpan ke: " + fullPath);
        logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " + getCurrentPlayer()->getUsername() + " | SIMPAN | " + filename);
    } catch (const NimonspoliException& e) {
        renderer->printError(e.what());
    }
}

// ===== LOAD =====
void Game::loadGame(const std::string& filename) {
    std::string fullPath = "saves/" + filename;
    SaveLoadManager slm;
    std::vector<Player*> rawPlayers;
    slm.loadGame(fullPath, turnsPlayed, maxTurn, rawPlayers, turnOrder, currentPlayerIndex, *board, *skillCardDeck, *logger);
    
    // Pindahkan raw pointer ke unique_ptr
    players.clear();
    for (Player* p : rawPlayers) {
        players.push_back(std::unique_ptr<Player>(p));
    }

    logger->addLog("Permainan dimuat dari: " + filename);
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
void Game::printDeed(const std::string& tileCode) {
    for (int i = 0; i < board->getTileCount(); i++) {
        Tile* tile = board->getTile(i);
        if (tile && tile->getCode() == tileCode) {
            StreetTile* st = dynamic_cast<StreetTile*>(tile);
            RailroadTile* rr = dynamic_cast<RailroadTile*>(tile);
            UtilityTile* ut = dynamic_cast<UtilityTile*>(tile);

            if (st) { renderer->printDeed(*st); return; }
            if (rr) { renderer->printDeed(*rr); return; }
            if (ut) { renderer->printDeed(*ut); return; }

            renderer->printDeedNotFound(tileCode);
            return;
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

// Menu bantuan
void Game::printHelp() {
    renderer->printHelp();
}

// Set renderer
void Game::setRenderer(CLIRenderer* r) {
    renderer = r;
}

// ===== FLOW GILIRAN =====
// Giliran berakhir
void Game::endTurn() {
    Player* player = getCurrentPlayer();
    if (!player) return;

    if (!player->hasRolledDiceThisTurn()) {
        renderer->printError("Kamu harus melempar dadu dulu sebelum mengakhiri giliran!");
        return;
    }

    // Reset flag skill per turn
    player->resetTurnFlags();
    player->setShieldActive(false);
    player->setDiscountPercent(0);

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

    // Serve jail turn buat pemain berikutnya
    Player* next = getCurrentPlayer();
    if (next && next->isJailed()) {
        next->serveJailTurn();
    }

    // Bagi kartu skill di awal giliran
    if (next) {
        std::cout << "\n";
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
            renderer->printInfo("Pilih aksi: ");
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

    renderer->printInfo("Bidak mendarat di: " + tile->getName() + " (" + tile->getCode() + ").");

    StreetTile* street = dynamic_cast<StreetTile*>(tile);
    if (street) { handleStreetLanding(player, *street); return; }

    RailroadTile* railroad = dynamic_cast<RailroadTile*>(tile);
    if (railroad) { handleRailroadLanding(player, *railroad); return; }

    UtilityTile* utility = dynamic_cast<UtilityTile*>(tile);
    if (utility) { handleUtilityLanding(player, *utility); return; }

    TaxTile* tax = dynamic_cast<TaxTile*>(tile);
    if (tax) { handleTaxLanding(player, *tax); return; }

    FestivalTile* festival = dynamic_cast<FestivalTile*>(tile);
    if (festival) { handleFestivalLanding(player); return; }

    CardTile* cardTile = dynamic_cast<CardTile*>(tile);
    if (cardTile) { handleCardLanding(player, *cardTile); return; }

    GoToJailTile* goToJail = dynamic_cast<GoToJailTile*>(tile);
    if (goToJail) { handleGoToJailLanding(player); return; }

    // Sisanya
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
        renderer->printInfo("Apakah kamu ingin membeli properti ini seharga M" + std::to_string(tile.getPrice()) + "? (y/n): ");

        std::string input;
        while (true) {
            std::getline(std::cin, input);
            if (input == "y" || input == "Y" || input == "n" || input == "N") {
                break;
            } else {
                renderer->printInfo("Input tidak valid. Masukkan y atau n: ");
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

// Tax
void Game::handleTaxLanding(Player& player, TaxTile& tile) {
    if (player.isShieldActive()) {
        renderer->printInfo("[SHIELD ACTIVE] Kamu terlindungi dari pajak!");
        return;
    }

    if (tile.getTaxType() == TaxType::PBM) {
        renderer->printInfo("Kamu mendarat di Pajak Barang Mewah (PBM)!");
        renderer->printInfo("Pajak sebesar M" + std::to_string(pbmFlat) + " langsung dipotong.");
        try {
            player -= pbmFlat;
            renderer->printInfo("Uang kamu: M" + std::to_string(player.getCash()));
            logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " + player.getUsername() + " | PAJAK | Bayar PBM M" + std::to_string(pbmFlat));
        } catch (const NimonspoliException&) {
            bankruptcyManager->handle(player, nullptr, pbmFlat, *this,
                [this](const std::vector<LiquidationOption>& options) -> int {
                    renderer->printInfo("\n=== Panel Likuidasi ===");
                    for (int i = 0; i < (int)options.size(); i++) {
                        renderer->printInfo(std::to_string(i + 1) + ". " + options[i].getDescription());
                    }
                    renderer->printInfo("0. Selesai");
                    renderer->printInfo("Pilih aksi: ");
                    std::string input;
                    std::getline(std::cin, input);
                    try { return std::stoi(input) - 1; } catch (...) { return -1; }
                });
        }
    } else { // PPH
        renderer->printInfo("Kamu mendarat di Pajak Penghasilan (PPH)!");
        renderer->printInfo("Pilih opsi pembayaran pajak:");
        renderer->printInfo("1. Bayar flat M" + std::to_string(pphFlat));
        renderer->printInfo("2. Bayar " + std::to_string(pphPersen) + "% dari total kekayaan");
        renderer->printInfo("(Pilih sebelum menghitung kekayaan!)");

        int choice = 0;
        while (choice != 1 && choice != 2) {
            std::cout << "Pilihan (1/2): ";
            std::string input;
            std::getline(std::cin, input);
            try { choice = std::stoi(input); } catch (...) {}
        }

        int amount = 0;
        if (choice == 1) {
            amount = pphFlat;
        } else {
            int wealth = player.getTotalWealth();
            amount = wealth * pphPersen / 100;
            renderer->printInfo("Total kekayaan kamu: M" + std::to_string(wealth));
            renderer->printInfo("Pajak " + std::to_string(pphPersen) + "%: M" + std::to_string(amount));
        }

        try {
            player -= amount;
            renderer->printInfo("Uang kamu: M" + std::to_string(player.getCash()));
            logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " + player.getUsername() + " | PAJAK | Bayar PPH M" + std::to_string(amount));
        } catch (const NimonspoliException&) {
            bankruptcyManager->handle(player, nullptr, amount, *this,
                [this](const std::vector<LiquidationOption>& options) -> int {
                    renderer->printInfo("\n=== Panel Likuidasi ===");
                    for (int i = 0; i < (int)options.size(); i++) {
                        renderer->printInfo(std::to_string(i + 1) + ". " + options[i].getDescription());
                    }
                    renderer->printInfo("0. Selesai");
                    renderer->printInfo("Pilih aksi: ");
                    std::string input;
                    std::getline(std::cin, input);
                    try { return std::stoi(input) - 1; } catch (...) { return -1; }
                });
        }
    }
}

// Festival
void Game::handleFestivalLanding(Player& player) {
    renderer->printInfo("Kamu mendarat di petak Festival!");
    
    auto props = player.getOwnedProperties();
    if (props.empty()) {
        renderer->printInfo("Kamu tidak memiliki properti. Efek festival tidak berlaku.");
        return;
    }

    renderer->printInfo("Daftar properti milikmu:");
    for (auto* prop : props) {
        StreetTile* st = dynamic_cast<StreetTile*>(prop);
        if (st) renderer->printInfo("- " + prop->getCode() + " (" + prop->getName() + ")");
    }

    renderer->printInfo("Masukkan kode properti: ");
    std::string kode;
    std::getline(std::cin, kode);
    for (char& c : kode) c = std::toupper(c);

    StreetTile* chosen = nullptr;
    for (auto* prop : props) {
        StreetTile* st = dynamic_cast<StreetTile*>(prop);
        if (st && prop->getCode() == kode) {
            chosen = st;
            break;
        }
    }

    if (!chosen) {
        renderer->printError("Kode properti tidak valid atau bukan Street tile.");
        return;
    }

    int sewaBefore = chosen->calculateRent(0);
    chosen->applyFestival();
    int sewaAfter = chosen->calculateRent(0);

    if (sewaBefore == sewaAfter) {
        renderer->printInfo("Efek sudah maksimum (harga sewa sudah digandakan tiga kali).");
        renderer->printInfo("Durasi di-reset menjadi: 3 giliran.");
    } else if (sewaBefore * 2 == sewaAfter) {
        renderer->printInfo("Efek festival aktif!");
        renderer->printInfo("Sewa awal: M" + std::to_string(sewaBefore));
        renderer->printInfo("Sewa sekarang: M" + std::to_string(sewaAfter));
        renderer->printInfo("Durasi: 3 giliran.");
    } else {
        renderer->printInfo("Efek diperkuat!");
        renderer->printInfo("Sewa sebelumnya: M" + std::to_string(sewaBefore));
        renderer->printInfo("Sewa sekarang: M" + std::to_string(sewaAfter));
        renderer->printInfo("Durasi di-reset menjadi: 3 giliran.");
    }

    logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " + player.getUsername() + " | FESTIVAL | " + chosen->getCode() + " sewa M" + std::to_string(sewaBefore) + " -> M" + std::to_string(sewaAfter));
}

// Card
void Game::handleCardLanding(Player& player, CardTile& tile) {
    renderer->printInfo("Kamu mendarat di Petak " + tile.getName() + "!");
    renderer->printInfo("Mengambil kartu...");

    ActionCard* card = nullptr;
    if (tile.getDeckType() == DeckType::CHANCE) {
        card = chanceDeck->draw();
    } else {
        card = generalFundsDeck->draw();
    }

    if (!card) {
        renderer->printInfo("Deck kosong.");
        return;
    }

    renderer->printInfo("Kartu: \"" + card->getDescription() + "\"");
    card->execute(player, *this);

    if (tile.getDeckType() == DeckType::CHANCE) {
        chanceDeck->discard(card);
    } else {
        generalFundsDeck->discard(card);
    }

    logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " + player.getUsername() + " | KARTU | " + card->getDescription());
}

// Go tile
void Game::handleGoLanding(Player& player) {
    player += goSalary;
    renderer->printInfo("Kamu mendarat tepat di GO! Dapat M" + std::to_string(goSalary) + ".");
    renderer->printInfo("Uang kamu sekarang: M" + std::to_string(player.getCash()));
    logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " + player.getUsername() + " | GO | Mendarat di GO, dapat M" + std::to_string(goSalary));
}

// Go to jail tile
void Game::handleGoToJailLanding(Player& player) {
    if (player.isShieldActive()) {
        renderer->printInfo("[SHIELD ACTIVE] Kamu terlindungi dari masuk penjara!");
        return;
    }
    player.goToJail();
    renderer->printInfo("Kamu mendarat di Petak Pergi ke Penjara!");
    renderer->printInfo("Bidak dipindahkan ke Penjara. Kamu ditahan!");
    logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " + player.getUsername() + " | PENJARA | Masuk penjara via PPJ");
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
                return renderer->promptChoice();
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
        int dropIndex = renderer->promptDropCard(names);
        std::string droppedName = names[dropIndex];
        player.removeCard(dropIndex);
        renderer->printInfo(droppedName  + " telah dibuang.");
        logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " + player.getUsername() + " | DROP_KARTU | " + names[dropIndex]);
    } else {
        player.addCard(card);
        renderer->printInfo("Pemain " + player.getUsername() + "! Kamu mendapatkan kartu: " + card->getName());
    }

    logger->addLog("[Turn " + std::to_string(turnsPlayed) + "] " + player.getUsername() + " | DAPAT_KARTU | " + card->getName());
}