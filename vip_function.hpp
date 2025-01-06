#ifndef VIP_FUNCTION_HPP
#define VIP_FUNCTION_HPP

#include <string>
class Vip_Function{

    private:
    std::string function_name;
    int vip_number;
    std::string atm_id;
   
    
    
    public:
    Vip_Function(const std::string& function_name, int vip_number,std::string atm_id);

    std::string get_function_name() const;
    std::string get_atm_id() const;
    int get_vip_number() const;
 
};

#endif 