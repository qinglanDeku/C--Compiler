#pragma once
#include<string>
#include<list>
#include"../../semantic/include/Symboltab.h"
using std::string;
using std::list;
class varItem;

class Operand{
public:
    //using TEMP_ARRAY_XXXXXX while IR involved array or struct
    enum OPType {VARIABLE, ARRAY_ADRESS, ARRAY_FIRST_ELEMENT,\
    TEMP_VARIABLE, TEMP_ARRAY_ADRESS, TEMP_ARRAY_FIRST_ELEMENT,\
    ICONSTANT, FCONSTANT, } ;      
    Operand(OPType type, int No):type(type),No(No){}
        //name = produceName();}
    Operand(){}
    ~Operand(){}
    const OPType getType(){return type;}
    const string getName(){return name;}
    void setValue(int Ival){value.IVal = Ival;}
    void setValue(float Fval){value.FVal = Fval;}
    void setType(OPType newtype){if(type == TEMP_VARIABLE)
            type = newtype;
    }
    void operator= (const Operand&a){
        type = a.type;
        value = a.value;
        No = a.No;
        name = a.name;
    }

  protected:
    OPType type;
    union{
        int IVal;       //int val of a variable/constant
        float FVal;     //float val of a variable/constant
        //int adress;     //adress number of an adress
        
    }value;
    int No;         //the number of varibale

    string name;
    virtual string produceName() = 0;

};

class VariableOP:public Operand{
public:
    VariableOP(OPType type, int No, varItem* symbolTabP):Operand(type, No),\
    counterPart(symbolTabP)
    {
        name = produceName();
    }

    VariableOP(const VariableOP &a);

    varItem * getSymbolTabItem() { return counterPart; }

  private:
    varItem *counterPart;
    string produceName();
};

class TemporaryOP:public Operand{
public:
    TemporaryOP(OPType type, int No):Operand(type, No){
        name = produceName();
    }

private:
  string produceName();
};

class ConstantOP:public Operand{
public:
    ConstantOP(OPType type, int ival):Operand(type, -1){
        value.IVal = ival;
        name = produceName();
    }

    ConstantOP(OPType type, float fval):Operand(type, -1){
        value.FVal = fval;
        name = produceName();
    }
    int getIntVal() { return value.IVal; }

  private:
    string produceName();
};

class InterCode{
public:
    enum IRtype{ASSIGN, PLUS, SUB, MUL, DIV, FUNC, LABEL, 
    ASSIGNLOC,      // code like "a = &b;"
    ASSIGNFROMLOC,  //code like "a = *b;"
    ASSIGNTOLOC, //code like "*a = b;"
    GOTO, COND, RETURN,
    DEC, //this for definition/declaration of an array or struct
    ARG, CALL, PARAM, READ, WRITE};
    InterCode(IRtype type):type(type){}
    const IRtype getType(){
        return type;
    }
    const string getCode(){return code;}
    virtual Operand* getOperand(int number) = 0; //get the operand at number position
    IRtype getOperator() { return getType(); }

  protected:
    IRtype type;
    string code;
};

class AssignCode:public InterCode{
public:
    AssignCode(IRtype type, Operand *left, Operand *right);
    const Operand* getLeftOp(){return left;}
    const Operand* getRightOp(){return right;}
    Operand* getOperand(int number){
        if(number == 1)
            return left;
        else if (number == 2)
            return right;
        else
            return nullptr;
    }

  private:
    Operand *left, *right; 
    string produceCode();
};

class BinopCode:public InterCode{
public:
    BinopCode(IRtype type, Operand *result, Operand *op1, Operand *op2);
    const Operand* getResult(){return result;}
    const Operand* getOp1(){return op1;}
    const Operand* getOp2(){return op2;}
    Operand* getOperand(int number){
        if(number == 1)
            return result;
        else if(number == 2)
            return op1;
        else if(number == 3)
            return op2;
        else
            return nullptr;
    }


  private:
    Operand *result, *op1, *op2;
    string produceCode();
};

class FuncDecCode:public InterCode{
public:
    FuncDecCode(IRtype type, string *funcName);
    const string getFuncName(){return funcName;}
    Operand* getOperand(int number){
        return nullptr;
    }


  private:
    string funcName;
    string produceCode();
};

class LabelCode:public InterCode{
public:
    LabelCode(IRtype type, int labelNo);
    const int getLabelNo(){return labelNo;}
    Operand *getOperand(int number) { return nullptr; }

  private:
    int labelNo;
    string produceCode();
};

class GotoCode:public InterCode{
public:
    GotoCode(IRtype type, int dstLabel);
    const int getDst(){return dstLabel;}
    Operand *getOperand(int number) { return nullptr; }

  private:
    int dstLabel;
    string produceCode();
};

class CondCode:public InterCode{
public:
    enum Relop{
        L, LE, G, GE, EQ, NE
    };
    CondCode(IRtype type, Operand *op1, Operand *op2, string relop, int dstLabel);
    Operand *getOp1(){return op1;}
    Operand *getOp2(){return op2;}
    string getRelop(){return relop;}
    int getDst(){return dstLabel;}
    Operand* getOperand(int number){
        if(number == 1)
            return op1;
        else if(number == 2)
            return op2;
        else
            return nullptr;
    }

  private:
    Operand *op1, *op2;
    string relop;
    int dstLabel;
    string produceCode();
};

class RetCode:public InterCode{
public:
    RetCode(IRtype type, Operand* retVal);
    const Operand *getRetVal(){return retVal;}
    Operand* getOperand(int number){
        if(number == 1)
            return retVal;
        else
            return nullptr;
    }

  private:
    Operand *retVal;
    string produceCode();
};

class DecCode:public InterCode{
public:
    DecCode(IRtype type, Operand* OP, int size);
    const Operand *getOp(){return OP;}
    const int getSize(){return size;}
    Operand* getOperand(int number){
        if(number == 1)
            return OP;
        else
            return nullptr;
    }

  private:
    Operand *OP;
    int size;
    string produceCode();
};

class ArgCode:public InterCode{
public:
    ArgCode(IRtype type, Operand *arg);
    //const Operand *getArg(){return arg;}
    Operand* getOperand(int number){
        if(number == 1)
            return arg;
        else
            return nullptr;
    }

  private:
    Operand* arg;
    string produceCode();
};


class CallCode:public InterCode{
public:
    CallCode(IRtype type, Operand *retVal, string funcName);
    const Operand *getRetVal(){return retVal;}
    const string getFuncName(){return funcName;}
    Operand* getOperand(int number){
        if (number == 1)
            return retVal;
        else
            return nullptr;
    }

  private:
    Operand *retVal;
    string funcName;
    string produceCode();
};


class ParamCode:public InterCode{
public:
    ParamCode(IRtype type, Operand *param);
    const Operand* getParam(){return param;}
    Operand* getOperand(int number){
        if(number == 1)
            return param;
        else
            return nullptr;
    }

  private:
    Operand* param;
    string produceCode();
};


class ReadCode:public InterCode{
public:
    ReadCode(IRtype type, Operand *OP);
    const Operand* getReadVal(){return val;}
    Operand* getOperand(int number){
        if (number == 1)
            return val;
        else
            return nullptr;
    }

  private:
    Operand *val;
    string produceCode();
};


class WriteCode:public InterCode{
public:
    WriteCode(IRtype type, Operand *val);
    //const Operand* getWriteVal(){return val;}
    Operand* getOperand(int number){
        if(number == 1)
            return val;
        else
            return nullptr;
    }

  private:
    Operand *val;
    string produceCode();
};


