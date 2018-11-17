#include"Symboltab.h"

/*class varItem*/
varItem::varItem(const varItem& a){
    this->name = a.name;
    this->type = a.type;
    this->structType = NULL;
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

void varItem::SetStructType(structItem* type){
    this->structType = type;
}

/*class funItem*/
funItem::funItem(const funItem& a){
    this->name = a.name;
    this->retval = a.retval;
    this->ArgList.assign(a.ArgList.begin(), a.ArgList.end());
}





/*class VarSymbolTab*/
void VarSymbolTab::DeleteItem(char* Itemname){
    string tempname;
    tempname.assign(Itemname);
    

}


