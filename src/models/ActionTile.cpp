#include "models/ActionTile.hpp"
#include "models/Player.hpp"
#include "core/Game.hpp"
#include <algorithm>

// ActionTile

ActionTile::ActionTile(int idx, std::string cd, std::string nm)
    : Tile(idx, cd, nm) {}

TileType ActionTile::getTileType() const {
    return TileType::ACTION;
}

// CardTile

CardTile::CardTile(int idx, std::string cd, std::string nm, DeckType dt) 
    : ActionTile(idx, cd, nm), deckType(dt) {}

DeckType CardTile::getDeckType() const {
    return deckType;
}

void CardTile::onLanded(Player& player, Game& game) {}
void CardTile::onLanded(Player& player, Game& game) {
    if (deckType == DeckType::CHANCE) {
        game.drawChanceCard(player);
    } else {
        game.drawCommunityChestCard(player);
    }
}
DeckType CardTile::getDeckType() const {
    return deckType;
}

void CardTile::onLanded(Player& player, Game& game) {}

// FestivalTile

FestivalTile::FestivalTile(int idx, std::string cd, std::string nm) 
    : ActionTile(idx, cd, nm) {}

void FestivalTile::onLanded(Player& player, Game& game) {
    // Implementasi memilih StreetTile untuk festival melalui cli
}

// TaxTile

void TaxTile::onLanded(Player& player, Game& game) {
    //
}

TaxType TaxTile::getTaxType() const {
    return taxType;
}