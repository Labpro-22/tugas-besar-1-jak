#include "models/ActionCard.hpp"
// #include "Player.hpp"  // Diasumsikan tersedia
// #include "Game.hpp"    // Diasumsikan tersedia

GoToNearestStationCard::GoToNearestStationCard() 
    : ActionCard("Pergi ke Stasiun Terdekat", "Pergi ke stasiun terdekat dari posisimu saat ini.", CardType::CHANCE) {}

void GoToNearestStationCard::execute(Player& player, Game& game) {
    // Asumsi: Game memiliki fungsi utilitas untuk mencari stasiun terdekat dan memicu pendaratan
    // game.moveToNearestStation(player);
}

MoveBackCard::MoveBackCard() 
    : ActionCard("Mundur 3 Petak", "Mundur 3 petak dari posisimu saat ini.", CardType::CHANCE) {}

void MoveBackCard::execute(Player& player, Game& game) {
    // Asumsi: Game memiliki fungsi memindahkan mundur posisi pemain
    // game.movePlayer(player, -3); 
}

GoToJailCard::GoToJailCard() 
    : ActionCard("Masuk Penjara", "Langsung masuk penjara tanpa melewati GO.", CardType::CHANCE) {}

void GoToJailCard::execute(Player& player, Game& game) {
    // Mengubah status menjadi JAILED dan memindahkan ke Penjara
    // game.handleJail(player); 
}

BirthdayCard::BirthdayCard() 
    : ActionCard("Ulang Tahun", "Ini adalah hari ulang tahun Anda. Dapatkan M100 dari setiap pemain.", CardType::COMMUNITY_CHEST), amount(100) {}

void BirthdayCard::execute(Player& player, Game& game) {
    // Asumsi: Game memiliki vector players. Mengurangi M100 dari pemain lain dan menambah saldo player.
    // game.executeBirthdayGift(player, amount);
}

DoctorFeeCard::DoctorFeeCard() 
    : ActionCard("Biaya Dokter", "Biaya dokter. Bayar M700.", CardType::COMMUNITY_CHEST), fee(700) {}

void DoctorFeeCard::execute(Player& player, Game& game) {
    // Memotong uang pemain, jika tidak cukup masuk ke handleBankruptcy
    /*
    if (player.getTotalWealth() < fee) { ... }
    player -= fee; 
    */
}

NyalegCard::NyalegCard() 
    : ActionCard("Nyaleg", "Anda mau nyaleg. Bayar M200 kepada setiap pemain.", CardType::COMMUNITY_CHEST), amount(200) {}

void NyalegCard::execute(Player& player, Game& game) {
    // Memotong saldo pemain M200 x (Jumlah pemain - 1) untuk diberikan ke pemain lain
    // game.executeNyaleg(player, amount);
}