#include "core/AuctionManager.hpp"
#include "core/IGameAction.hpp"
#include "models/PropertyTile.hpp"
#include "models/Player.hpp"
#include "utils/NimonspoliException.hpp"
#include <algorithm>
#include <stdexcept>

AuctionManager::AuctionManager(IGameAction* gameAction):
    targetProperty(nullptr),
    currentHighBidder(nullptr),
    currentHighBid(0),
    hasAnyBid(false),
    finished(false),
    currentIndex(0),
    gameAction(gameAction)
{
    if (!gameAction) throw std::invalid_argument("gameAction cannot be null");
}

void AuctionManager::startAuction(PropertyTile* property, Player* triggerPlayer, const std::vector<Player*>& eligiblePlayers) {
    if (!property) throw std::invalid_argument("Property cannot be null");
    if (eligiblePlayers.empty()) throw std::invalid_argument("No eligible players");

    targetProperty = property;
    finished = false;
    hasAnyBid = false;
    currentHighBid = 0;
    currentHighBidder = nullptr;
    activeParticipants.clear();

    auto it = std::find(eligiblePlayers.begin(), eligiblePlayers.end(), triggerPlayer);
    if (it == eligiblePlayers.end()) {
        activeParticipants = eligiblePlayers;
    } else {
        size_t startIdx = (it - eligiblePlayers.begin() + 1) % eligiblePlayers.size();
        for (size_t i = 0; i < eligiblePlayers.size(); ++i) {
            activeParticipants.push_back(eligiblePlayers[(startIdx + i) % eligiblePlayers.size()]);
        }
    }

    currentIndex = 0;
    logEvent("LELANG_START", "Properti " + property->getCode() + " (" + property->getName() + ") dilelang. Giliran pertama: " + activeParticipants[currentIndex]->getUsername());
}

void AuctionManager::processBid(Player* bidder, int amount) {
    if (finished) throw InvalidActionException("Lelang sudah selesai.");
    if (bidder != activeParticipants[currentIndex]) throw InvalidActionException("Bukan giliran Anda.");

    if (bidder->getCash() <= currentHighBid) {
        processPass(bidder);
        return;
    }

    if (amount <= currentHighBid) throw InvalidActionException("Tawaran harus lebih tinggi dari " + std::to_string(currentHighBid));
    if (amount > bidder->getCash()) throw InsufficientFundsException("Uang tidak cukup. Anda hanya memiliki " + std::to_string(bidder->getCash()));

    currentHighBid = amount;
    currentHighBidder = bidder;
    hasAnyBid = true;
    logEvent("BID", bidder->getUsername() + " menawar " + std::to_string(amount));

    advanceToNextParticipant();
}

void AuctionManager::processPass(Player* passer) {
    if (finished) throw InvalidActionException("Lelang sudah selesai.");
    if (passer != activeParticipants[currentIndex]) throw InvalidActionException("Bukan giliran Anda.");

    if (std::find(activeParticipants.begin(), activeParticipants.end(), passer) == activeParticipants.end()) {
        throw InvalidActionException("Player tidak ikut lelang.");
    }

    logEvent("PASS", passer->getUsername() + " melewatkan giliran.");

    activeParticipants.erase(activeParticipants.begin() + currentIndex);
    
    if (activeParticipants.empty()) {
        finished = true;
        logEvent("LELANG_BATAL", "Tidak ada peserta yang tersisa. Lelang dibatalkan.");
        gameAction->finalizeAuction(nullptr, targetProperty, 0);
        return;
    }

    if (activeParticipants.size() == 1) {
        if (hasAnyBid) {
            endAuction();
        } else {
            Player* lastPlayer = activeParticipants[0];
            currentHighBidder = lastPlayer;
            currentHighBid = 0;
            hasAnyBid = true;
            logEvent("BID_PAKSA", lastPlayer->getUsername() + " dipaksa membeli dengan harga 0.");
            endAuction();
        }
        return;
    }

    if (currentIndex >= activeParticipants.size()) {
        currentIndex = 0;
    }

    logEvent("GILIRAN", "Giliran sekarang: " + activeParticipants[currentIndex]->getUsername());
}

void AuctionManager::advanceToNextParticipant() {
    if (activeParticipants.empty()) return;
    currentIndex = (currentIndex + 1) % activeParticipants.size();
    logEvent("GILIRAN", "Giliran sekarang: " + activeParticipants[currentIndex]->getUsername());
}

void AuctionManager::endAuction() {
    finished = true;
    if (!currentHighBidder) {
        logEvent("LELANG_GAGAL", "Tidak ada pemenang. Properti " + targetProperty->getCode() + " dikembalikan ke Bank.");
        gameAction->finalizeAuction(nullptr, targetProperty, 0);
        return;
    }
    logEvent("LELANG_SELESAI", "Pemenang: " + currentHighBidder->getUsername() + " dengan harga " + std::to_string(currentHighBid));
    gameAction->finalizeAuction(currentHighBidder, targetProperty, currentHighBid);
}

bool AuctionManager::isFinished() const {
    return finished;
}

Player* AuctionManager::getWinner() const {
    return finished ? currentHighBidder : nullptr;
}

int AuctionManager::getWinningBid() const {
    return finished ? currentHighBid : 0;
}

PropertyTile* AuctionManager::getTargetProperty() const {
    return targetProperty;
}

void AuctionManager::logEvent(const std::string& action, const std::string& detail) {
    if (gameAction) {
        gameAction->logAuctionEvent(action, detail);
    }
}