#include "models/ActionCard.hpp"
#include "models/Player.hpp"
#include "models/Board.hpp"
#include "models/PropertyTile.hpp"
#include "core/Game.hpp"
#include "utils/NimonspoliException.hpp"
#include <vector>
#include "models/Player.hpp"
#include "models/Board.hpp"
#include "models/PropertyTile.hpp"
#include "core/Game.hpp"
#include "utils/NimonspoliException.hpp"
#include <vector>

GoToNearestStationCard::GoToNearestStationCard() 
    : ActionCard("Pergi ke Stasiun Terdekat", "Pergi ke stasiun terdekat dari posisimu saat ini.", CardType::CHANCE) {}

void GoToNearestStationCard::execute(Player& player, Game& game) {
    // Kode stasiun sesuai spesifikasi: GBR(6), STB(16), TUG(26), GUB(36)
    std::vector<int> stationIndices = {5, 15, 25, 35}; // 0-based
    int pos = player.getPosition();

    int nearest = stationIndices[0];
    int minDist = -1;
    for (int idx : stationIndices) {
        int dist = (idx - pos + 40) % 40;
        if (minDist < 0 || dist < minDist) {
            minDist = dist;
            nearest = idx;
        }
    }

    // Gerak ke stasiun terdekat — lewat GO kalau perlu
    if (nearest < pos) {
        auto special = game.getBoard().getTile(0);
        // Sudah dihandle di movePlayer via goSalary
    }

    player.setPosition(nearest);
    game.processTileLandingPublic(player, nearest);
}

MoveBackCard::MoveBackCard()
    : ActionCard("MoveBack", "Mundur 3 petak.", CardType::CHANCE) {}

void MoveBackCard::execute(Player& player, Game& game) {
    int boardSize = game.getBoard().getTileCount();
    int newPos = (player.getPosition() - 3 + boardSize) % boardSize;
    player.setPosition(newPos);
    game.processTileLandingPublic(player, newPos);
}

GoToJailCard::GoToJailCard()
    : ActionCard("GoToJail", "Masuk Penjara.", CardType::CHANCE) {}

void GoToJailCard::execute(Player& player, Game& game) {
    game.sendPlayerToJail(player, "Mendapatkan chance card pergi ke penjara");
}

BirthdayCard::BirthdayCard()
    : ActionCard("Birthday", "Ini adalah hari ulang tahun Anda. Dapatkan M100 dari setiap pemain.", CardType::COMMUNITY_CHEST),
      amount(100) {}

void BirthdayCard::execute(Player& player, Game& game) {
    auto activePlayers = game.getActivePlayers();
    for (Player* other : activePlayers) {
        if (other == &player) continue;
        try {
            *other -= amount;
            player += amount;
        } catch (const NimonspoliException&) {
            // Kalau tidak bisa bayar, bayar sebisanya
            int available = other->getCash();
            *other -= available;
            player += available;
        }
    }
}

DoctorFeeCard::DoctorFeeCard()
    : ActionCard("DoctorFee", "Biaya dokter. Bayar M700.", CardType::COMMUNITY_CHEST),
      fee(700) {}

void DoctorFeeCard::execute(Player& player, Game& game) {
    player -= fee;
}

// ===== NyalegCard =====
NyalegCard::NyalegCard()
    : ActionCard("Nyaleg", "Anda mau nyaleg. Bayar M200 kepada setiap pemain.", CardType::COMMUNITY_CHEST),
      amount(200) {}

void NyalegCard::execute(Player& player, Game& game) {
    auto activePlayers = game.getActivePlayers();
    for (Player* other : activePlayers) {
        if (other == &player) continue;
        try {
            player -= amount;
            *other += amount;
        } catch (const NimonspoliException&) {
            int available = player.getCash();
            player -= available;
            *other += available;
            break;
        }
    }
}