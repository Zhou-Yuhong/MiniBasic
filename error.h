#ifndef ERROR_H
#define ERROR_H
using namespace std;
#include <exception>
#include <iostream>
#include <string>
class myException:public exception {
public:
    myException(std::string information)
    {

        error_inf=information;
    }
    //返回错误信息
    std::string report(){

        return error_inf;
    }
private:
    std::string error_inf;

};


#endif // ERROR_H
