#include"Symboltab.h"
varItem::varItem(const varItem& a){
    this->name = a.name;
    this->type = a.type;
    this->structType = nullptr;
    this->dimension = 0;
    if(this->type == STRUCT)
        this->structType = a.structType;
        
    else if(this->type == INTARRAY){
        this->dimension = a.dimension;
    }
    else if(this->type == FLOATARRAY){
        this->dimension = a.dimension;
    }
    else if(this->type == STRUCTARRAY){
        this->structType = a.structType;
        this->dimension = a.dimension;
    }
    else;
    this->lineNo = a.lineNo;
}

varItem::varItem(string &name, TYPE type, int lineNo):name(name), type(type), lineNo(lineNo){
    
}