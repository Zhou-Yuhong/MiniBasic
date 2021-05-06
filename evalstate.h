#ifndef EVALSTATE_H
#define EVALSTATE_H
#include <string>
#include <map>
#include "error.h"
using namespace std;
struct ValueUnit{
    bool type;//true表示int,false表示string
    int valInt;
    string valString;
    string getvalString(){
        return this->valString;
    }
    int getvalInt(){
        return this->valInt;
    }

    ValueUnit(string value){
        this->valString=value;
        this->type=false;
    }
    ValueUnit(int value){
        this->valInt=value;
        this->type=true;
    }
    ValueUnit(){

    }
    //进行算术运算的前提是两个ValueUnit都存储的是int
    ValueUnit operator +(ValueUnit &b){
        if(!(this->type&&b.type)){
            string err="Error:Strings cannot be computed";
            throw myException(err);
        }
        int result= this->getvalInt()+b.getvalInt();
        return ValueUnit(result);
    }
    ValueUnit operator -(ValueUnit &b){
        if(!(this->type&&b.type)){
            string err="Error:Strings cannot be computed";
            throw myException(err);
        }
        int result= this->getvalInt()-b.getvalInt();
        return ValueUnit(result);
    }
    ValueUnit operator *(ValueUnit &b){
        if(!(this->type&&b.type)){
            string err="Error:Strings cannot be computed";
            throw myException(err);
        }
        int result=this->getvalInt()*b.getvalInt();
        return ValueUnit(result);
    }
    ValueUnit operator /(ValueUnit &b){
        if(!(this->type&&b.type)){
            string err="Error:Strings cannot be computed";
            throw myException(err);
        }
        int result=this->getvalInt()/b.getvalInt();
        return ValueUnit(result);
    }
    ValueUnit operator >(ValueUnit &b){
        if(!(this->type&&b.type)){
            string err="Error:Strings cannot be computed";
            throw myException(err);
        }
        int result= this->getvalInt()>b.getvalInt()? 1:0;
         return ValueUnit(result);
    }
    ValueUnit operator <(ValueUnit &b){
        if(!(this->type&&b.type)){
            string err="Error:Strings cannot be computed";
            throw myException(err);
        }
       int result= this->getvalInt()<b.getvalInt()? 1:0;
       return ValueUnit(result);
    }

};

class EvalState {
public:
    EvalState() {};

    ~EvalState() {};
    void setValue(string var, int value);
    void setValue(string var, string value);
    void setValue(string var,ValueUnit value);
    ValueUnit getValue(string var);
    bool isExist(string var);
    void clear();

private:
    map<string, ValueUnit> valtable;
};

#endif // EVALSTATE_H

