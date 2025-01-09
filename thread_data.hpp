#ifndef THREAD_DATA_HPP
#define THREAD_DATA_HPP

#include "atm.hpp"
#include "bank.hpp"

class Bank;

class Thread_Data{

    private:
    ATM* atm;
    Bank* bank;

    public:
    Thread_Data(ATM* atm,Bank* bank);

    ATM* get_atm();
    Bank* get_Bank();
};

#endif