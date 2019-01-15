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
  Variable(int addr, int reg, Operand *var);
  Variable(const Variable &a);
  ~Variable();
  int getAddr() { return addr; }
  int setAddr(int newaddr) { int ret = addr;
      addr = newaddr;
      return ret;
  }
  string getName() { return name; }
  Operand::OPType getVarType() { return type; }
  int getVarSize() { return size; }
  int getReg() { return reg; }
  int setReg(int newval){
      int retval = reg;
      reg = newval;
      return reg;
  }
  bool isArray(){
    if(type == Operand::ARRAY_ADRESS || type == Operand::ARRAY_FIRST_ELEMENT)
        return true;
    else
        return false;
  }

private:
    int addr; //the offset from bp;对于数组来说，这个相对地址存放的是指针
    int reg;    //有的变量可能放在寄存器里，比如参数，其余变量也可能在溢出前放在寄存器中，溢出后该值为0
    string name;
    Operand::OPType type;
    int size;
    int baseSize;
};

class VariableList{
public:
  VariableList();
  ~VariableList();
  void addVar(Variable &newVar) { VarList.push_back(newVar); }
  void setVarReg(const string &varName, int regNumber)
  {
      for (int i(0); i < VarList.size(); i++)
      {
          if (VarList[i].getName() == varName)
          {
              VarList[i].setReg(regNumber);
          }
      }
    }
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
    void spillReg() { Mips32.spillAllReg(*this); }
    //下面这个函数用于添加汇编代码代代码表
    void addAsmCode(const AsmCode &newCode) { AssemblyCodeList.push_back(newCode); } 
    /*下面两个函数用于指针强制转化*/
    static ConstantOP* getConstOPptr(Operand * OP_ptr){
        ConstantOP *retval = (ConstantOP *)OP_ptr;
        return retval;
    }
    static VariableOP* getVariaOPptr(Operand *OP_ptr){
        VariableOP *retval = (VariableOP *)OP_ptr;
        return retval;
    }

    static TemporaryOP* getTempOPptr(Operand *OP_ptr){
        TemporaryOP *retval = (TemporaryOP *)OP_ptr;
        return retval;
    }
    VariableList asmVarList;
    static void clearDynamicVar(void* p){
        delete p;
        p = nullptr;
    }
    void subSp(int subVal){
        AsmCode code0("addi ");
        code0.addOperand(AsmOperand(AsmOperand::REGISTER, 29, -1));
        code0.addOperand(AsmOperand(AsmOperand::REGISTER, 29, -1));
        code0.addOperand(AsmOperand(AsmOperand::IMMEDIATE, -subVal, -1));
        addAsmCode(code0);
    }
    void addSp(int addVal){
        AsmCode code0("addi ");
        code0.addOperand(AsmOperand(AsmOperand::REGISTER, 29, -1));
        code0.addOperand(AsmOperand(AsmOperand::REGISTER, 29, -1));
        code0.addOperand(AsmOperand(AsmOperand::IMMEDIATE, addVal, -1));
        addAsmCode(code0);
    }
    void saveRa(){
        subSp(4);
        AsmCode *code = new AsmCode("sw ");
        code->addOperand(AsmOperand(AsmOperand::REGISTER, 31, -1));
        code->addOperand(AsmOperand(AsmOperand::ADDRESS, 0, 29));
        addAsmCode(*code);
        clearDynamicVar(code);
    }

    void retRa(){
        AsmCode *code = new AsmCode("lw ");
        code->addOperand(AsmOperand(AsmOperand::REGISTER, 31, -1));
        code->addOperand(AsmOperand(AsmOperand::ADDRESS, 0, 29));
        addAsmCode(*code);
        clearDynamicVar(code);
        addSp(4);
    }

//这3个move都是针对相对地址（也就是从相对帧指针的位置搬出）
    void moveFromMemtoReg(int regNo, int addr);

    void moveFromMemtoReg(int dst_regNo, int addr, int src_regNo);  //地址的表示形式为 addr($dst_regNo)

    void moveFromRegtoMem(int regNo, int addr);

    void moveFromRegtoMem(int dst_regNo, int addr, int src_regNo);

    void moveFromRegtoReg(int dst_regNo, int src_regNo);

    void loadConsttoReg(int constVar, int retNo);

    void Jump(string labelName, string jmpType);

    void setLabel(int labelNo);

    void setCond(string relop, int reg1, int reg2, int dst);

    void setCondsubFunc(string relopAsm, int reg1, int reg2, int dst);

    void createSpace(Operand *var, int &bp_offset);

    static string turnIntToStr(int val);

    static int turnStrToInt(string str);

    static void errorRoutine(string errorInfo);

  private:
    list<InterCode *> IRCodeList;
    list<VariableOP *> variableList;
    static string AsmHead[22];
    BlockList blockList;
    MipsRegisterList Mips32;
    vector<AsmCode> AssemblyCodeList;
    void translateOneBlock(const BasicBlock &BB);
    int translateOneLine(list<InterCode *>::iterator it, int &offset); //这里的offset是函数体内局部变量的存储地址在栈中的偏移量
    int translateParam(list<InterCode *>::iterator it, int offset);    //这里的offset是参数的offset
};
