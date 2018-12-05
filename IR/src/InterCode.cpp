#include"InterCode.h"
#include<iostream>
#include<sstream>
using std::cout;
using std::cin;
using std::endl;
using std::stringstream;
/**********************class Operand****************************/
int Operand::tempVarNum = 0;

string Operand::produceName(){
    string temp;
    if(type == VARIABLE || type == ADRESS){
        temp.append("v");
        stringstream *ss = new stringstream;
        string *strNo = new string;
        *ss<<this->No;
        *ss>>*strNo;
        temp.append(*strNo);
        delete ss;
        delete strNo;
    }
    else if(type == TEMPVARIABLE || type == TEMPADRESS){
        temp.append("t");
        this->tempVarNum+= 1;
        stringstream *ss;
        string *strNo = new string;
        *ss <<this->tempVarNum;
        *ss >>*strNo;
        temp.append(*strNo);
        delete ss;
        delete strNo;
    }
    else{
        temp.append("#");
        string *strVal = new string;
        stringstream* ss = new stringstream;
        if(type == ICONSTANT){
            *ss << value.IVal;
            *ss >> *strVal;
        }
        else{
            *ss <<value.FVal;
            *ss >>*strVal;
        }
        temp.append(*strVal);
        delete strVal;
        delete ss;
    }
    return temp;
}



/***********************class AssignCode*******************************/

AssignCode::AssignCode(IRtype type, Operand* left, Operand *right):InterCode(type),\
left(*left), right(*right){
    this->code = produceCode();
}

string AssignCode::produceCode(){
    string tempCode;
    if(this->type == ASSIGN){
        tempCode.append(left.getName());
        tempCode.append(" := ");
        tempCode.append(right.getName());
    }
    else if(this->type == ASSIGNLOC){
        tempCode.append(left.getName());
        tempCode.append(" := &");
        tempCode.append(right.getName());
    }
    else if(this->type == ASSIGNFROMLOC){
        tempCode.append(left.getName());
        tempCode.append(" := *");
        tempCode.append(right.getName());
    }
    else{
        tempCode.append("*");
        tempCode.append(left.getName());
        tempCode.append(" := ");
        tempCode.append(right.getName());
    }
    return tempCode;
}



/**********************class BinopCode**************/
BinopCode::BinopCode(IRtype type, Operand* result, Operand *op1, Operand *op2):\
InterCode(type), result(*result), op1(*op1), op2(*op2){
    this->code = produceCode();    
}

string BinopCode::produceCode(){
    string temp;
    temp.append(this->result.getName());
    temp.append(" := ");
    temp.append(this->op1.getName());
    if(this->type == PLUS)
        temp.append(" + ");
    else if(this->type == SUB)
        temp.append(" - ");
    else if(this->type == MUL)
        temp.append(" * ");
    else
        temp.append(" / ");
    temp.append(this->op2.getName());
    return temp;
}



/*************************class FuncDecCode***************************/
FuncDecCode::FuncDecCode(IRtype type, string *funcName):\
InterCode(type), funcName(*funcName){
    this->code = produceCode();
}

string FuncDecCode::produceCode(){
    string temp;
    temp.append("FUNCTION ");
    temp.append(funcName);
    temp.append(" :");
}




/****************************class LabelCode*****************************/
LabelCode::LabelCode(IRtype type, int labelNo):\
InterCode(type), labelNo(labelNo){
    this->code = produceCode();
}

string LabelCode::produceCode(){
    string temp;
    stringstream* ss = new stringstream;
    string strNo;
    *ss << labelNo;
    *ss >> strNo;
    temp.append("LABEL ");
    temp.append(strNo);
    temp.append(" :");
    delete ss;
    return temp;
}



/*****************************class GotoCode**********************************/
GotoCode::GotoCode(IRtype type, int dstLabel):\
InterCode(type), dstLabel(dstLabel){
    this->code = produceCode();
}

string GotoCode::produceCode(){
    string temp;
    stringstream *ss = new stringstream;
    string strLabel;
    temp.append("GOTO ");
    *ss << this->dstLabel;
    *ss>>strLabel;
    temp.append(strLabel);
    delete ss;
    return temp;
}

/***************************class CondCode****************************/
CondCode::CondCode(IRtype type, Operand *op1, Operand *op2, Relop relop, int dstLabel):\
InterCode(type), op1(*op1), op2(*op2), relop(relop), dstLabel(dstLabel)
{
    this->code = produceCode();
}

string CondCode::produceCode(){
    string temp;
    string *strLabel = new string; 
    stringstream *ss = new stringstream;

    temp.append("IF ");
    temp.append(op1.getName);
    temp.append(" ");
    if(relop == GE)
        temp.append(" >= ");
    else if(relop == G)
        temp.append(" > ");
    else if(relop == LE)
        temp.append(" <= ");
    else if(relop == L)
        temp.append(" < ");
    else if(relop == EQ)
        temp.append(" = ");
    else
        temp.append(" != ");
    temp.append(op2.getName());
    temp.append(" GOTO ");
    *ss<<this->dstLabel;
    *ss >>*strLabel;
    temp.append(*strLabel);

    delete strLabel;
    delete ss;

    return temp;
}


