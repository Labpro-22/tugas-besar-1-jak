#include "models/ActionTile.hpp"

// ActionTile

ActionTile::ActionTile(int idx, std::string cd, std::string nm)
    : Tile(idx, cd, nm) {}

// CardTile

CardTile::CardTile(int idx, std::string cd, std::string nm, DeckType dt) 
    : ActionTile(idx, cd, nm), deckType(dt) {}

DeckType CardTile::getDeckType() const {
    return deckType;
}

void CardTile::onLanded(Player& player, Game& game) {}

// FestivalTile

FestivalTile::FestivalTile(int idx, std::string cd, std::string nm) 
    : ActionTile(idx, cd, nm) {}

void FestivalTile::onLanded(Player& player, Game& game) {}

// TaxTile

TaxTile::TaxTile(int idx, std::string cd, std::string nm, TaxType txt, int fm, double pct) 
    : ActionTile(idx, cd, nm), taxType(txt), flatAmount(fm), percentage(pct) {}

TaxType TaxTile::getTaxType() const {
    return taxType;
}