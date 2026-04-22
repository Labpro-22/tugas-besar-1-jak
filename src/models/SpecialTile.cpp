#include "SpecialTile.hpp"

// SpecialTile

SpecialTile::SpecialTile(int idx, std::string cd, std::string nm)
    : Tile(idx, cd, nm) {}

// GoTile

GoTile::GoTile(int idx, std::string cd, std::string nm, int slr)
    : SpecialTile(idx, cd, nm), salary(slr) {}

void GoTile::onLanded(Player &player, Game &game) {}

// JailTile

JailTile::JailTile(int idx, std::string cd, std::string nm, int fn)
    : SpecialTile(idx, cd, nm), fine(fn) {}

void JailTile::onLanded(Player &player, Game &game) {}

void JailTile::processJailTurn(Player &player, Game &game) {}

// GoToJailTile

GoToJailTile::GoToJailTile(int idx, std::string cd, std::string nm)
    : SpecialTile(idx, cd, nm) {}

void GoToJailTile::onLanded(Player &player, Game &game) {}

// FreeParkingTile

FreeParkingTile::FreeParkingTile(int idx, std::string cd, std::string nm)
    : SpecialTile(idx, cd, nm) {}

void FreeParkingTile::onLanded(Player &player, Game &game) {}