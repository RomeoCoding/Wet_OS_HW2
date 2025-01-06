#ifndef VIP_THREAD_POOL_HPP
#define VIP_THREAD_POOL_HPP

#include "vip_function.hpp"
#include <set>

#include <pthread.h>

struct Compare_by_VipNumber {
    bool operator()(const Vip_Function& a, const Vip_Function& b) const {
        return a.get_vip_number() > b.get_vip_number();  // Sort by vip_number from biggest to smallest
    }
};

class Vip_Thread_Pool{

    private:
    std::multiset<Vip_Function, Compare_by_VipNumber> vip_command_list;
    pthread_mutex_t vip_command_list_lock = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t vip_empty_or_end = PTHREAD_COND_INITIALIZER;
    int vip_command_number = 0;
    int End_Vip_Threads = 0;

    public:
    Vip_Thread_Pool();
    ~Vip_Thread_Pool();
    Vip_Function get_Next_Vip_Command();
    void Insert_Vip_Command(std::string command_name,int vip_number, std::string atm_id);
    void Set_End_Vip_Threads();

};


#endif 