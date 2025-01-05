#include "vip_function.hpp"


 Vip_Function::Vip_Function(const std::string& function_name, int vip_number,std::string atm_id)
 : function_name(function_name), vip_number(vip_number), atm_id(atm_id) {}

std::string Vip_Function::get_function_name() const{
return function_name;
}

int Vip_Function::get_vip_number() const{
    return vip_number;
}

std::string Vip_Function::get_atm_id() const{
    return atm_id;
}