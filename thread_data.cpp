#include "thread_data.hpp"

Thread_Data::Thread_Data(ATM* atm,Bank* bank)
:atm(atm), bank(bank){}

ATM* Thread_Data::get_atm(){
    return atm;
}

Bank* Thread_Data::get_Bank(){
    return bank;
}