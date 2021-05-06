#include "evalstate.h"
void EvalState::setValue(string var, int value)
{   ValueUnit tmp(value);
    if (!isExist(var)) {

        valtable.insert(pair<string, ValueUnit>(var, tmp));
        return;
    }
    valtable[var] = tmp;
}
void EvalState::setValue(string var, string value){
    ValueUnit tmp(value);
    if(!isExist(var)){
        valtable.insert(pair<string,ValueUnit>(var,tmp));
        return;
    }
    valtable[var]=tmp;
}
void EvalState::setValue(string var, ValueUnit value){
    if(!isExist(var)){
        valtable.insert(pair<string,ValueUnit>(var,value));
        return;
    }
    valtable[var]=value;
}

ValueUnit EvalState::getValue(string var)
{
    map<string, ValueUnit>::iterator  it= valtable.find(var);  //如果查找失败，返回map::end()
    return it->second;
}

bool EvalState::isExist(string var)
{
    map<string,ValueUnit>::iterator it = valtable.find(var);
    if (it != valtable.end()) return true;
    return false;
}

void EvalState::clear()
{
    valtable.clear();
}


