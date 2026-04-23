#ifndef CARDDECK_HPP
#define CARDDECK_HPP

#include <queue>
#include <vector>
#include <string>
#include <algorithm>
#include <random>

template <typename T>
class CardDeck {
private:
    std::queue<T*> cards;             // Antrean kartu yang siap ditarik
    std::vector<T*> discardPile;      // Tumpukan kartu yang sudah terpakai (dijadikan vector untuk mempermudah shuffle ulang)

public:
    CardDeck() = default;
    
    // Destructor untuk memastikan manajemen memori yang aman
    ~CardDeck() {
        while (!cards.empty()) {
            delete cards.front();
            cards.pop();
        }
        for (T* card : discardPile) {
            delete card;
        }
    }

    // Mengambil kartu teratas. Jika habis, kocok ulang dari discardPile
    T* draw() {
        if (cards.empty()) {
            if (discardPile.empty()) {
                return nullptr; // Kondisi deck sepenuhnya kosong
            }
            
            // Reshuffle dari discardPile menjadi cards baru
            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(discardPile.begin(), discardPile.end(), g);
            
            for (T* card : discardPile) {
                cards.push(card);
            }
            discardPile.clear();
        }
        
        T* drawnCard = cards.front();
        cards.pop();
        return drawnCard;
    }

    // Membuang kartu yang sudah dipakai
    void discard(T* card) {
        if (card != nullptr) {
            discardPile.push_back(card);
        }
    }

    // Format informasi deck untuk fungsi Save
    std::string getStateInfo() const {
        std::string state = std::to_string(cards.size() + discardPile.size()) + "\n";
        
        // Asumsi struktur format simpan diatur dalam string ini
        // Implementasi mendetail parsing queue ke string akan bergantung format SaveLoadManager
        
        return state;
    }

    // Fungsi utilitas untuk inisialisasi awal program (Membaca dari file / manual seed)
    void addCardToDeck(T* card) {
        if (card != nullptr) {
            cards.push(card);
        }
    }
};

#endif