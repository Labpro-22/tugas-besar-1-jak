#include "../../include/utils/NimonspoliException.hpp"

NimonspoliException::NimonspoliException(std::string message) 
    : message(message) {}

const char* NimonspoliException::what() const noexcept {
    return message.c_str();
}

InsufficientFundsException::InsufficientFundsException() 
    : NimonspoliException("Error: Uang tidak cukup!") {}

InvalidActionException::InvalidActionException() 
    : NimonspoliException("Error: Aksi tidak valid!") {}

CardSlotFullException::CardSlotFullException() 
    : NimonspoliException("Error: Slot kartu penuh!") {}

PropertyNotOwnedException::PropertyNotOwnedException() 
    : NimonspoliException("Error: Properti tidak dimiliki!") {}

FileNotFoundException::FileNotFoundException() 
    : NimonspoliException("Error: File tidak ditemukan!") {}

InvalidInputException::InvalidInputException() 
    : NimonspoliException("Error: Input tidak valid!") {}