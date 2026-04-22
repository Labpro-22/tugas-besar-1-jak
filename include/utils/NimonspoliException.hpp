#ifndef NIMONSPOLI_EXCEPTION_HPP
#define NIMONSPOLI_EXCEPTION_HPP

#include <exception>
#include <string>

class NimonspoliException : public std::exception
{
protected:
    std::string message;

public:
    NimonspoliException(const std::string &message);
    virtual const char *what() const noexcept override;
};

class InsufficientFundsException : public NimonspoliException
{
public:
    InsufficientFundsException(const std::string &message);
};

class InvalidActionException : public NimonspoliException
{
public:
    InvalidActionException(const std::string &message);
};

class CardSlotFullException : public NimonspoliException
{
public:
    CardSlotFullException(const std::string &message);
};

class PropertyNotOwnedException : public NimonspoliException
{
public:
    PropertyNotOwnedException(const std::string &message);
};

class FileNotFoundException : public NimonspoliException
{
public:
    FileNotFoundException(const std::string &message);
};

class InvalidInputException : public NimonspoliException
{
public:
    InvalidInputException(const std::string &message);
};

#endif