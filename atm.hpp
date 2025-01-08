#ifndef ATM_HPP
#define ATM_HPP

#include <string>
#include <pthread.h>

class ATM {
private:
    std::string atm_id;       //Unique ATM ID
    std::string input_file;   //Path to input file
    bool end_atm = false;         //true to close the ATM
    pthread_mutex_t end_atm_lock = PTHREAD_MUTEX_INITIALIZER;

public:
    ATM(const std::string& id, const std::string& file_path);
    ~ATM(){pthread_mutex_destroy(&end_atm_lock);}
    //Accessors
    const std::string& get_id() const { return atm_id; }
    const std::string& get_input_file() const { return input_file; }
    
    bool End_ATM();
    
};

#endif // ATM_HPP
