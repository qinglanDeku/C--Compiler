#pragma once
#include<string>
#include<list>
using std::string;
using std::list;

class Operand{
public:

    enum OPType {VARIABLE, TEMPVARIABLE, ICONSTANT, FCONSTANT, ADRESS, TEMPADRESS} ;      
    Operand(OPType type, int No):type(type){
        this->No = No;
        name = produceName();}
    Operand(){}
    ~Operand(){}
    const OPType getType(){return type;}
    const string getName(){return name;}
    void setValue(int Ival){value.IVal = Ival;}
    void setValue(float Fval){value.FVal = Fval;}
    

private:
    static int tempVarNum;
    OPType type;
    union{
        int IVal;       //int val of a variable/constant
        float FVal;     //float val of a variable/constant
        int adress;     //adress number of an adress
        
    }value;
    int No;         //number of varibale

    string name;
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
    

protected:
    IRtype type;
    string code;
};

class AssignCode:public InterCode{
public:
    AssignCode(IRtype type, Operand *left, Operand *right);
    const Operand getLeftOp(){return left;}
    const Operand getRightOp(){return right;}
private:
    Operand left, right; 
    string produceCode();
};

class BinopCode:public InterCode{
public:
    BinopCode(IRtype type, Operand *result, Operand *op1, Operand *op2);
    const Operand getResult(){return result;}
    const Operand getOp1(){return op1;}
    const Operand getOp2(){return op2;}

private:
    Operand result, op1, op2;
    string produceCode();
};

class FuncDecCode:public InterCode{
public:
    FuncDecCode(IRtype type, string *funcName);
    const string getFuncName(){return funcName;}

private:
    string funcName;
    string produceCode();
};

class LabelCode:public InterCode{
public:
    LabelCode(IRtype type, int labelNo);
    const int getLabelNo(){return labelNo;}

private:
    int labelNo;
    string produceCode();
};

class GotoCode:public InterCode{
public:
    GotoCode(IRtype type, int dstLabel);
    const int getDst(){return dstLabel;}

private:
    int dstLabel;
    string produceCode();
};

class CondCode:public InterCode{
public:
    enum Relop{
        L, LE, G, GE, EQ, NE
    };
    CondCode(IRtype type, Operand *op1, Operand *op2, Relop relop, int dstLabel);
    Operand getOp1(){return op1;}
    Operand getOp2(){return op2;}
    Relop getRelop(){return relop;}
    int getDst(){return dstLabel;}
private:
    Operand op1, op2;
    Relop relop;
    int dstLabel;
    string produceCode();
};

class RetCode:public InterCode{
public:
    RetCode(IRtype type, Operand* retVal);
    const Operand getRetVal(){return retVal;}
private:
    Operand retVal;
    string produceCode();
};

class DecCode:public InterCode{
public:
    DecCode(IRtype type, Operand* OP, int size);
    const Operand getOp(){return OP;}
    const int getSize(){return size;}
private:
    Operand OP;
    int size;
    string produceCode();
};

class ArgCode:public InterCode{
public:
    ArgCode(IRtype type, Operand* arg);
    const Operand getArg(){return arg;}
private:
    Operand arg;
    string produceCode();
};


class CallCode:public InterCode{
public:
    CallCode(IRtype type, Operand *retVal, string funcName);
    const Operand getRetVal(){return retVal;}
    const string getFuncName(){return funcName;}

private:
    Operand retVal;
    string funcName;
    string produceCode();
};


class ParamCode:public InterCode{
public:
    ParamCode(IRtype type, Operand *param);
    const Operand getParam(){return param;}

private:
    Operand param;
    string produceCode();
};


class ReadCode:public InterCode{
public:
    ReadCode(IRtype type, Operand *OP);
    const Operand getReadVal(){return val;}

private:
    Operand val;
    string produceCode();
};


class WriteCode:public InterCode{
public:
    WriteCode(IRtype type, Operand *val);
    const Operand getWriteVal(){return val;}

private:
    Operand val;
    string produceCode();
};


