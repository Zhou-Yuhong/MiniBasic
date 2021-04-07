#ifndef EVALSTATE_H
#define EVALSTATE_H
#include <string>
#include <map>
using namespace std;
class EvalState {
public:
    EvalState() {};

    ~EvalState() {};
    void setValue(string var, int value);
    int getValue(string var);
    bool isExist(string var);
    void clear();

private:
    map<string, int> valtable;
};

#endif // EVALSTATE_H

