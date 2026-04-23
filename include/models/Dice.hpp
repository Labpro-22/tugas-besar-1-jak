#pragma once

class Dice {
    private:
    int dice1;
    int dice2;

    public:
    Dice();

    // Acak nilai dadu secara random dan return totalnya
    int roll();

    // Atur hasil lemparan dadu secara manual
    void setDice(int x, int y);

    // Getter
    int getDice1() const;
    int getDice2() const;

    // Return total kedua dadu
    int getTotal() const;

    // Cek kedua dadu sama atau ngga
    bool isDouble() const;
};