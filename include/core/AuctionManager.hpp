#pragma once

#include <vector>
#include <string>

class Player;
class PropertyTile;
class IGameAction;

class AuctionManager {
private:
    PropertyTile* targetProperty;
    std::vector<Player*> activeParticipants;
    Player* currentHighBidder;
    int currentHighBid;
    bool hasAnyBid;
    int consecutivePasses;
    bool finished;
    size_t currentIndex;
    IGameAction* gameAction;

    void advanceToNextParticipant();
    void endAuction();
    void logEvent(const std::string& action, const std::string& detail);

public:
    AuctionManager(IGameAction* gameAction);
    void startAuction(PropertyTile* property, Player* triggerPlayer, const std::vector<Player*>& eligiblePlayers);
    void processBid(Player* bidder, int amount);
    void processPass(Player* passer);
    bool isFinished() const;
    Player* getWinner() const;
    int getWinningBid() const;
    PropertyTile* getTargetProperty() const;
};