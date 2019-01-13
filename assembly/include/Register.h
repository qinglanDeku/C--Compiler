#pragma once
#include<vector>
#include<deque>
#include<string>
#include"../../IR/include/InterCode.h"
using std::deque;
using std::string;
using std::vector;
class Variable;
class Register
{
  public:
    Register();
    Register(string &name);
    ~Register();
    bool getState() { return state; }
    bool setState(bool newState){
        bool old = state;
        state = newState;
        return old;
    }
    void setVar(Variable &var);
    string setVarName(string varName)
    {
        string oldName = variableName;
        variableName = varName;
        return oldName;
    }
    unsigned int setAddress(unsigned newAddr){
        unsigned int old = address;
        address = newAddr;
        return old;
    }
    int setValue(int newVal){
        int old(value);
        value = newVal;
        return old;
    }

  private:
    bool state;           //0 means spare, 1 means occupied;
    string name;          //  name of register
    unsigned int address; // address of the value which is a variable in reg;
    string variableName;  // name of variable stored
    int value;            // value of variable stroed; if  variable is a array, then the value may be void
};

class MipsRegisterList{
public:
  MipsRegisterList();
  ~MipsRegisterList();
  Register &getReg(int i);
  Register &getReg(const string &name);
  static string Mips32Regs[32];
private:
  vector<Register> regs;
  deque<Register*> emptyRegList;
  deque<Register*> occupiedRegList;
};