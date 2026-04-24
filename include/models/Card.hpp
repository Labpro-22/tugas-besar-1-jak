#ifndef CARD_HPP
#define CARD_HPP
#include "core/Game.hpp"

#include <string>

class Card {
protected:
    std::string name;        // Nama dari kartu
    std::string description; // Deskripsi efek kartu

public:
    Card(std::string n, std::string d) : name(n), description(d) {}
    virtual ~Card() = default;

    std::string getName() const { return name; }               // Getter nama kartu
    std::string getDescription() const { return description; } // Getter deskripsi kartu
};

#endif