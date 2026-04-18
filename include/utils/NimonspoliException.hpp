#ifndef NIMONSPOLI_EXCEPTION_HPP
#define NIMONSPOLI_EXCEPTION_HPP

#include <exception>
#include <string>

class NimonspoliException : public std::exception {
protected:
    std::string message;

public:
    NimonspoliException(std::string message);
    virtual const char* what() const noexcept override;
};


class InsufficientFundsException : public NimonspoliException {
public:
    InsufficientFundsException();
};

class InvalidActionException : public NimonspoliException {
public:
    InvalidActionException();
};

class CardSlotFullException : public NimonspoliException {
public:
    CardSlotFullException();
};

class PropertyNotOwnedException : public NimonspoliException {
public:
    PropertyNotOwnedException();
};

class FileNotFoundException : public NimonspoliException {
public:
    FileNotFoundException();
};

class InvalidInputException : public NimonspoliException {
public:
    InvalidInputException();
};

#endif