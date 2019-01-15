#pragma once
#include<vector>
#include<deque>
#include<string>
#include"../../IR/include/InterCode.h"
using std::deque;
using std::string;
using std::vector;
class Variable;
class Assembly;
class Register
{
  public:
    enum AsmVarType
  {
      V, ARRAV, T, LOCT, NONE     //分别表示：变量，表示数组的变量，临时变量，表示地址的临时变量，LOCT是为了更好的处理数组的翻译,VOID表示为空
  };
    Register();
    Register(string &name);
    ~Register();
    bool getState() { return state; }
    bool setState(bool newState){
        bool old = state;
        state = newState;
        return old;
    }
    string getRegName()const{
        return name;
    }
    int getVarAddress() const { return address; }
    void setVar(Variable &var);
    string setVarName(string varName)
    {
        string oldName = variableName;
        variableName = varName;
        return oldName;
    }
    string getVarName() { return variableName; }
    int setAddress(int newAddr){
        int old = address;
        address = newAddr;
        return old;
    }
    int setValue(int newVal){
        int old(value);
        value = newVal;
        return old;
    }
    int getValue() { return value; }
    AsmVarType setAsmType(AsmVarType newType){
      AsmVarType old = asmType;
      asmType = newType;
      return old;
    }

    AsmVarType getAsmType() { return asmType; }
  private:
    bool state;           //0 means spare, 1 means occupied;
    string name;          //  name of register
    int address; // address of the value which is a variable in reg;
    string variableName;  // name of variable stored
    int value;            // value of variable stroed; if  variable is a array, then the value may be void
    AsmVarType asmType; //只有在计算过程中会用到的寄存器才会有这个值，也就是那个16个寄存器
};

class MipsRegisterList{
public:
  MipsRegisterList();
  ~MipsRegisterList();
  Register *getReg(int i);
  Register *getReg(const string &name);
  Register& allocateReg(Assembly& assembly_process);
  void retractReg(int i, Assembly &asm_process);
  void retractReg(const string &name, Assembly &asm_process);
  int getRegNumber(const Register& reg){       //通过寄存器的名字得到其编号
      for (int i(0); i < 32; i++){
          if(Mips32Regs[i] == reg.getRegName())
              return i;
      }
      return 0;
  }
  void spillOneReg(Register &r, Assembly &Asm_process);     //溢出单个寄存器

  static string Mips32Regs[32];

private:
  vector<Register> regs;
  deque<Register*> emptyRegList;
  deque<Register*> occupiedRegList;
};