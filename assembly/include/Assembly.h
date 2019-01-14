#pragma once
#include<vector>
#include<list>
#include<deque>
#include<string>
#include"../../IR/include/InterCode.h"
#include"BasicBlock.h"
#include"Register.h"
using std::deque;
using std::list;
using std::string;
using std::vector;

class Variable{
public:
  Variable();
  Variable(int addr, int reg, VariableOP*var);
  Variable(const Variable &a);
  ~Variable();
  int getAddr() { return addr; }
  string getName() { return origin->getName(); }
  Operand::OPType getVarType() { return origin->getType(); }
  int getVarSize() { return origin->getSymbolTabItem()->getSize(); }
  int getReg() { return reg; }

private:
    VariableOP *origin;
    int addr; //the offset from bp;
    int reg;    //有的变量可能放在寄存器里，比如参数，其余变量也可能在溢出前放在寄存器中，溢出后该值为0
};

class VariableList{
public:
    VariableList();
    ~VariableList();
    void addVar(Variable &newVar) { VarList.push_back(newVar); }
    bool ifVarInList(Variable var){
        for(int i(0); i< VarList.size(); i++){
            if(VarList[i].getName() == var.getName())
                return true;
        }
        return false;
    }
    bool ifVarInList(const string &varName ){
        for(int i(0); i< VarList.size(); i++){
            if(VarList[i].getName() == varName)
                return true;
        }
        return false;
    }
    Variable& getVar(const string &name);
private:
    vector<Variable> VarList;
};


class AsmOperand{
public:
    enum AsmOPType
    {
        IMMEDIATE, REGISTER, ADDRESS, LABEL
    };
    AsmOperand(AsmOPType type, int part1, int part2);//只有ADDRESS类才由两个部分组成：第一部分是偏移量，第二部分是寄存器编号
    AsmOperand(const AsmOperand &a);
    ~AsmOperand();
    //将part1(和part2)转换成字符串用以合成字符串
    int getPart1() { return part1; }//不同的类型part1的意义不同。
    int getPart2() { return part2; }//非ADDRESS类型调用该函数返回的是无效的寄存器编号-1；
    string getStrOperand() { return AsmOPStr; }
    AsmOPType getType() { return type; }

private:
    AsmOPType type;
    int part1, part2;
    string AsmOPStr;
    string produceStrOperand();  
};


class AsmCode{
public:
  AsmCode(string AsmOperator);
  AsmCode(const AsmCode &a);
  ~AsmCode();
  string getStrAsmCode() {
      if(strCode == "")
        strCode = produceStrCode();
      return strCode;
  }
  void addOperand(AsmOperand newOP) { OperandList.push_back(newOP); }
  string getOperator() { return AsmOperator; }

private:
  string AsmOperator;
  vector<AsmOperand> OperandList;
  string strCode;
  string produceStrCode();      //label语句产生的ASM代码注意区分！！！
};

class Assembly
{
  public:
    Assembly();
    Assembly(list<InterCode *> IRCodeList, list<VariableOP *> variableList);
    ~Assembly();
    void produceAssembly();
    void printAssembly();
    void outputAssembly(const string &filename);
    void spillReg();
    static ConstantOP* getConstOPptr(Operand * OP_ptr){
        ConstantOP *retval = (ConstantOP *)OP_ptr;
        return retval;
    }
    static VariableOP* getVariaOPptr(Operand *OP_ptr){
        VariableOP *retval = (VariableOP *)OP_ptr;
        return retval;
    }

  private:
    list<InterCode *> IRCodeList;
    list<VariableOP *> variableList;
    BlockList blockList;
    MipsRegisterList Mips32;
    VariableList asmVarList;
    vector<AsmCode> AssemblyCodeList;
    void translateOneBlock(const BasicBlock &BB);
    int translateOneLine(list<InterCode *>::iterator it, int &offset); //这里的offset是函数体内局部变量的存储地址在栈中的偏移量
    int translateParam(list<InterCode *>::iterator it, int offset);    //这里的offset是参数的offset
};
