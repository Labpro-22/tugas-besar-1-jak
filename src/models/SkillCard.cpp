#include "models/SkillCard.hpp"


MoveCard::MoveCard(int s) 
    : SkillCard("MoveCard", "Maju " + std::to_string(s) + " petak."), steps(s) {}

void MoveCard::use(Player& player, Game& game) {
    // Memicu pergerakan pemain sebanyak `steps`
    // game.movePlayer(player, steps);
}

DiscountCard::DiscountCard(int p, int dur) 
    : SkillCard("DiscountCard", "Diskon " + std::to_string(p) + "% selama " + std::to_string(dur) + " giliran."), percent(p), duration(dur) {}

void DiscountCard::use(Player& player, Game& game) {
    // Memicu diskon aktif pada pemain
    // player.activateDiscount(percent, duration);
}

ShieldCard::ShieldCard(int dur) 
    : SkillCard("ShieldCard", "Kebal tagihan/sanksi selama " + std::to_string(dur) + " giliran."), duration(dur) {}

void ShieldCard::use(Player& player, Game& game) {
    // Membuat pemain kebal
    // player.activateShield(duration);
}

TeleportCard::TeleportCard() 
    : SkillCard("TeleportCard", "Berpindah ke petak manapun di atas papan.") {}

void TeleportCard::use(Player& player, Game& game) {
    // Meminta prompt tujuan dan memindahkan pemain
}

LassoCard::LassoCard() 
    : SkillCard("LassoCard", "Menarik satu pemain lawan di depan ke petak Anda.") {}

void LassoCard::use(Player& player, Game& game) {
    // Menarik pemain di depan ke posisi saat ini
}

DemolitionCard::DemolitionCard() 
    : SkillCard("DemolitionCard", "Menghancurkan satu properti milik pemain lawan.") {}

void DemolitionCard::use(Player& player, Game& game) {
    // Menghancurkan 1 properti lawan yang dipilih
}