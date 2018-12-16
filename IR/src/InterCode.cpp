#include"InterCode.h"
#include<iostream>
#include<sstream>
using std::cout;
using std::cin;
using std::endl;
using std::stringstream;

/***********************class VariableOP*****************************/
string VariableOP::produceName(){
    string temp;
    
    temp.append("v");
    stringstream *ss = new stringstream;
    string *strNo = new string;
    *ss<<this->No;
    *ss>>*strNo;
    temp.append(*strNo);
    delete ss;
    delete strNo;

    return temp;
}


string TemporaryOP::produceName(){
    string temp;
    temp.append("t");
    stringstream *ss = new stringstream;
    string *strNo = new string;
    *ss <<this->No;
    *ss >>*strNo;
    temp.append(*strNo);
    delete ss;
    delete strNo;
    return temp;
}

string ConstantOP::produceName(){
    string temp;
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
    return temp;
}

/***********************class AssignCode*******************************/

AssignCode::AssignCode(IRtype type, Operand* left, Operand *right):InterCode(type),\
left(left), right(right){
    this->code = produceCode();
}

string AssignCode::produceCode(){
    string tempCode;
    if(this->type == ASSIGN){
        tempCode.append(left->getName());
        tempCode.append(" := ");
        tempCode.append(right->getName());
    }
    else if(this->type == ASSIGNLOC){
        tempCode.append(left->getName());
        tempCode.append(" := &");
        tempCode.append(right->getName());
    }
    else if(this->type == ASSIGNFROMLOC){
        tempCode.append(left->getName());
        tempCode.append(" := *");
        tempCode.append(right->getName());
    }
    else{
        tempCode.append("*");
        tempCode.append(left->getName());
        tempCode.append(" := ");
        tempCode.append(right->getName());
    }
    return tempCode;
}

/***************class RetCode**************/
RetCode::RetCode(IRtype type, Operand* retVal):InterCode(type){
    this->retVal = retVal;
    this->code = produceCode();
}

string RetCode::produceCode(){
    string temp;
    temp.append("RETURN ");
    temp.append(retVal->getName());
    return temp;
}

/**********************class BinopCode**************/
BinopCode::BinopCode(IRtype type, Operand* result, Operand *op1, Operand *op2):\
InterCode(type), result(result), op1(op1), op2(op2){
    this->code = produceCode();    
}

string BinopCode::produceCode(){
    string temp;
    temp.append(this->result->getName());
    temp.append(" := ");
    temp.append(this->op1->getName());
    if(this->type == PLUS)
        temp.append(" + ");
    else if(this->type == SUB)
        temp.append(" - ");
    else if(this->type == MUL)
        temp.append(" * ");
    else
        temp.append(" / ");
    temp.append(this->op2->getName());
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
    return temp;
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
    temp.append("LABEL label");
    temp.append(strNo);
    temp.append(" :");
    delete ss;
    return temp;
}

/*************************DecCode**********************/
DecCode::DecCode(IRtype type, Operand* OP, int size):InterCode(type){
    this->OP = OP;
    this->size = size;
    this->code = produceCode();
}

string DecCode::produceCode(){
    string temp;
    temp.append("DEC ");
    temp.append(OP->getName());
    temp.append(" ");
    stringstream ss;
    string intStr;
    ss << this->size;
    ss >> intStr;
    temp.append(intStr);
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
    temp.append("label");
    *ss << this->dstLabel;
    *ss>>strLabel;
    temp.append(strLabel);
    delete ss;
    return temp;
}

/***************************class CondCode****************************/
CondCode::CondCode(IRtype type, Operand *op1, Operand *op2, string relop, int dstLabel):\
InterCode(type), op1(op1), op2(op2), relop(relop), dstLabel(dstLabel)
{
    this->code = produceCode();
}

string CondCode::produceCode(){
    string temp;
    string *strLabel = new string; 
    stringstream *ss = new stringstream;

    temp.append("IF ");
    temp.append(op1->getName());
    temp.append(" ");
    temp.append(relop);
    temp.append(" ");
    temp.append(op2->getName());
    temp.append(" GOTO label");
    *ss<<this->dstLabel;
    *ss >>*strLabel;
    temp.append(*strLabel);

    delete strLabel;
    delete ss;

    return temp;
}

/******************************class readCode***************************/
ReadCode::ReadCode(IRtype type, Operand *op):InterCode(type){
    val = op;
    this->code = produceCode();
}

string ReadCode::produceCode(){
    string temp;
    temp.append("READ ");
    if(val != NULL)
        temp.append(val->getName());
    return temp;
}


/*****************************class CallCode******************/
CallCode::CallCode(IRtype type, Operand *retVal, string funcName):InterCode(type){
    this->retVal = retVal;
    this->funcName = funcName;
    this->code = produceCode();
}

string CallCode::produceCode(){
    string temp;
    if(retVal == NULL){
        temp.append("nomean");
        temp.append(" := CALL ");
        temp.append(this->funcName);
        return temp;
    }
    if(retVal->getType() == Operand::TEMP_ARRAY_ADRESS)
        temp.append("*");
    temp.append(retVal->getName());
    temp.append(" := CALL ");
    temp.append(this->funcName);
    return temp;
}

/************************************writeCode*************************************/
WriteCode::WriteCode(IRtype type, Operand * val):InterCode(type){
    this->val = val;
    this->code = produceCode();
}

string WriteCode::produceCode(){
    string temp;
    temp.append("WRITE ");
    temp.append(val->getName());
    return temp;
}


/**************argCode*********************/
ArgCode::ArgCode(IRtype type, Operand* arg):InterCode(type){
    this->arg = arg;
    this->code = produceCode();
}

string ArgCode::produceCode(){
    string temp;
    temp.append("ARG ");
    if(arg->getType() == Operand::ARRAY_FIRST_ELEMENT)
        temp.append("&");
    temp.append(arg->getName());
    return temp;
}

/**********paramCode**************/
ParamCode::ParamCode(IRtype type, Operand *param):InterCode(type){
    this->param = param;
    this ->code = produceCode();
}
string ParamCode::produceCode(){
    string temp;
    temp.append("PARAM ");
    temp.append(param->getName());
    return temp;
}
