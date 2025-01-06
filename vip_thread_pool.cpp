#include "vip_thread_pool.hpp"


Vip_Thread_Pool::Vip_Thread_Pool(){

}

Vip_Thread_Pool::~Vip_Thread_Pool(){

    pthread_mutex_destroy(&vip_command_list_lock);
    pthread_cond_destroy(&vip_empty_or_end);
}


Vip_Function Vip_Thread_Pool::get_Next_Vip_Command(){

    pthread_mutex_lock(&vip_command_list_lock);
    while( vip_command_list.empty() && End_Vip_Threads == 0 ){

        pthread_cond_wait(&vip_empty_or_end, &vip_command_list_lock);
    }
    if(End_Vip_Threads == 1){
        Vip_Function null_func("-1",-1,"-1");
        return null_func;
    }
    
    Vip_Function vip_command = (*(vip_command_list.begin()));
    vip_command_list.erase(vip_command_list.begin());
    vip_command_number --;
    pthread_mutex_unlock(&vip_command_list_lock);
    return vip_command;

}


 void Vip_Thread_Pool::Insert_Vip_Command(std::string command_name,int vip_number,std::string atm_id ){
    
    pthread_mutex_lock(&vip_command_list_lock);
    vip_command_list.emplace(command_name,vip_number,atm_id);
    vip_command_number ++;
    pthread_cond_signal(&vip_empty_or_end);
    pthread_mutex_unlock(&vip_command_list_lock);
 }


 void Vip_Thread_Pool::Set_End_Vip_Threads(){

    End_Vip_Threads = 1;
    pthread_cond_broadcast(&vip_empty_or_end);
 }