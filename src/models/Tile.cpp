#include "Tile.hpp"

Tile::Tile(int idx, std::string cd, std::string nm)
    : index(idx), code(cd), name(nm) {}

std::string Tile::getDisplayColor()
{
    return "WHITE";
}

std::string Tile::getCode() const
{
    return code;
}

int Tile::getIndex() const
{
    return index;
}