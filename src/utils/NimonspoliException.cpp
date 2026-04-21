#include "../../include/utils/NimonspoliException.hpp"

NimonspoliException::NimonspoliException(const std::string& message) 
    : message(message) {}

const char* NimonspoliException::what() const noexcept {
    return message.c_str();
}

InsufficientFundsException::InsufficientFundsException(const std::string& message) 
    : NimonspoliException(message) {}

InvalidActionException::InvalidActionException(const std::string& message) 
    : NimonspoliException(message) {}

CardSlotFullException::CardSlotFullException(const std::string& message) 
    : NimonspoliException(message) {}

PropertyNotOwnedException::PropertyNotOwnedException(const std::string& message) 
    : NimonspoliException(message) {}

FileNotFoundException::FileNotFoundException(const std::string& message) 
    : NimonspoliException(message) {}

InvalidInputException::InvalidInputException(const std::string& message) 
    : NimonspoliException(message) {}