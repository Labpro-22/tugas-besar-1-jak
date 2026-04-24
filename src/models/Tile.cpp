#include "models/Tile.hpp"

Tile::Tile(int idx, std::string cd, std::string nm)
    : index(idx), code(cd), name(nm) {}

std::string Tile::getDisplayColor() {
    return "WHITE";
}

std::string Tile::getCode() const {
    return code;
}

int Tile::getIndex() const {
    return index;
}

std::string Tile::getName() const {
    return name;
}

TileType Tile::getTileType() const {
    return TileType::UNDEFINED;
}