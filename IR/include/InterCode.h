#pragma once
#include<string>
#include<list>
using std::string;
using std::list;

class Operand{
public:
    enum OPType {VARIABLE, TEMPVARIABLE, CONSTANT, ADRESS, TEMPADRESS} ;      
    Operand(OPType type, int No):type(type), No(No){}
    Operand(){}
    ~Operand(){}
    const OPType getType(){return type;}
    const string getName();
    void setValue(int Ival){value.IVal = Ival;}
    void setValue(float Fval){value.FVal = Fval;}
    

private:
    OPType type;
    union{
        int IVal;       //int val of a variable/constant
        float FVal;     //float val of a variable/constant
        int adress;     //adress number of an adress
        
    }value;
    int No;         //number of varibale or temporary varaible

};

class InterCode{
public:
    enum IRType{ASSIGN, PLUS, SUB, MUL, DIV, MINUS, FUNC, LABEL, 
    ASSIGNLOC,      // code like "a = &b;"
    ASSIGNFROMLOC,  //code like "a = *b;"
    ASSIGNTOLOC, //code like "*a = b;"
    GOTO, IFGOTO, DEC, //this for definition/declaration of an array or struct
    ARG, CALL, PARAM, READ, WRITE};


private:
    IRType type;
    union{
        struct{Operand left, right;} assign;
        struct{Operand result, op1, op2;} binop;
        string funcName;
        int LabelNum;       //number of label
    }stuff;
};