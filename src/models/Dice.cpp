#include "models/Dice.hpp"
#include <cstdlib>
#include <ctime>

Dice::Dice() : dice1(1), dice2(1) {
    // Inisialisasi random seed sekali ketika object dibuat
    std::srand(std::time(nullptr));
}

int Dice::roll() {
    dice1 = (std::rand() % 6) + 1;
    dice2 = (std::rand() % 6) + 1;
    return getTotal();
}

void Dice::setDice(int x, int y) {
    dice1 = x;
    dice2 = y;
}

int Dice::getDice1() const {
    return dice1;
}

int Dice::getDice2() const {
    return dice2;
}

int Dice::getTotal() const {
    return dice1 + dice2;
}

bool Dice::isDouble() const {
    return dice1 == dice2;
}