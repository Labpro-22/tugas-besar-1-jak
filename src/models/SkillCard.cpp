#include "models/SkillCard.hpp"


MoveCard::MoveCard(int s) 
    : SkillCard("MoveCard", "Maju " + std::to_string(s) + " petak."), steps(s) {}

DiscountCard::DiscountCard(int p, int dur) 
    : SkillCard("DiscountCard", "Diskon " + std::to_string(p) + "% selama " + std::to_string(dur) + " giliran."), percent(p), duration(dur) {}

ShieldCard::ShieldCard(int dur) 
    : SkillCard("ShieldCard", "Kebal tagihan/sanksi selama " + std::to_string(dur) + " giliran."), duration(dur) {}

TeleportCard::TeleportCard() 
    : SkillCard("TeleportCard", "Berpindah ke petak manapun di atas papan.") {}

LassoCard::LassoCard() 
    : SkillCard("LassoCard", "Menarik satu pemain lawan di depan ke petak Anda.") {}

DemolitionCard::DemolitionCard() 
    : SkillCard("DemolitionCard", "Menghancurkan satu properti milik pemain lawan.") {}