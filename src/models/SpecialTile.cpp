#include "models/SpecialTile.hpp"
#include "models/Player.hpp"

// SpecialTile

SpecialTile::SpecialTile(int idx, std::string cd, std::string nm)
    : Tile(idx, cd, nm) {}

// GoTile

GoTile::GoTile(int idx, std::string cd, std::string nm, int slr)
    : SpecialTile(idx, cd, nm), salary(slr) {}

// JailTile

JailTile::JailTile(int idx, std::string cd, std::string nm, int fn)
    : SpecialTile(idx, cd, nm), fine(fn) {}

void JailTile::processJailTurn(Player& player, Game& game) {}

// GoToJailTile

GoToJailTile::GoToJailTile(int idx, std::string cd, std::string nm)
    : SpecialTile(idx, cd, nm) {}

// FreeParkingTile

FreeParkingTile::FreeParkingTile(int idx, std::string cd, std::string nm)
    : SpecialTile(idx, cd, nm) {}