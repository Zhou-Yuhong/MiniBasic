#include "evalstate.h"
void EvalState::setValue(string var, int value)
{
    if (!isExist(var)) {
        valtable.insert(pair<string, int>(var, value));
        return;
    }
    valtable[var] = value;
}

int EvalState::getValue(string var)
{
    map<string, int>::iterator  it= valtable.find(var);  //如果查找失败，返回map::end()
    return it->second;
}

bool EvalState::isExist(string var)
{
    map<string, int>::iterator it = valtable.find(var);
    if (it != valtable.end()) return true;
    return false;
}

void EvalState::clear()
{
    valtable.clear();
}


