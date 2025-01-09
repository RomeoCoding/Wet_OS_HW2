#include "atm.hpp"

ATM::ATM(const std::string& id, const std::string& file_path)
    : atm_id(id), input_file(file_path) {}

 bool ATM::End_ATM(){
    pthread_mutex_lock(&end_atm_lock);
    if(end_atm == false)
    {
        end_atm = true;
         pthread_mutex_unlock(&end_atm_lock);
        return true;
    }
     pthread_mutex_unlock(&end_atm_lock);
    return false;
 }