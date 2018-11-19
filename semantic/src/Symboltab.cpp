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

/*************************************************
class funItem*/
funItem::funItem(const funItem& a){
    this->name = a.name;
    this->retval = a.retval;
    this->ArgList.assign(a.ArgList.begin(), a.ArgList.end());
}





/* ***********************************************
class VarSymbolTab*/
void VarSymbolTab::DeleteItem(char* Itemname){
    string tempname;
    tempname.assign(Itemname);
    list<varItem>::iterator it = table.begin();
    while(it != table.end()){
        if(it->GetName() == tempname)
            break;
        it++;
    }
    if(it == table.end())
        return;
    table.erase(it);
}

const varItem* VarSymbolTab::FindItem(const char*name){
    string tempname;
    tempname.assign(name);
    list<varItem>::iterator it = table.begin();
    while(it != table.end()){
        if(it->GetName() == tempname)
            break;
        it++;
    }
    if(it == table.end())
        return NULL;
    else
        return &(*it);
}



/* *************************************************
class FuncSymbolTab*/
void FuncSymbolTab::DeleteItem(char* ItemName){
    string tempname;
    tempname.assign(ItemName);
    list<funItem>::iterator it = table.begin();
    while(it != table.end()){
        if(it->GetName() == tempname)
            break;
        it++;
    }
    if(it == table.end())
        return;
    table.erase(it);
}

const funItem* FuncSymbolTab::FindItem(const char*name){
    string tempname;
    tempname.assign(name);
    list<funItem>::iterator it = table.begin();
    while(it != table.end()){
        if(it->GetName() == tempname)
            break;
        it++;
    }
    if(it == table.end())
        return NULL;
    else
        return &(*it);
}



/****************************************************
 class structItem*/
 structItem::structItem(const structItem& a){
     this->name = a.name;
     this->lineNo = a.lineNo;
     this->MemberVar.assign(a.MemberVar.begin(), a.MemberVar.end());
 }

 const varItem* structItem::GetMember(const string& name){
    varItem* p = NULL;
    for(int i(0); i<MemberVar.size(); i++){
        if(MemberVar[i].GetName() == name){
            p = &(MemberVar[i]);
            break;
        }
    }
    return p;
 }




 /***************************************************
  class structTab*/
void structTab::DeleteItem(char *ItemName){
    string tempname;
    tempname.assign(ItemName);
    list<structItem>::iterator it = table.begin();
    while(it != table.end()){
        if(it->GetName() == tempname)
            break;
        it++;
    }
    if(it == table.end())
        return;
    table.erase(it);
}

const structItem* structTab::FindItem(const char* name){
    string tempname;
    tempname.assign(name);
    list<structItem>::iterator it = table.begin();
    while(it != table.end()){
        if(it->GetName() == tempname)
            break;
        it++;
    }
    if(it == table.end())
        return NULL;
    else
        return &(*it);
}

const structItem* structTab::FindItem(const string& name){
    list<structItem>::iterator it = table.begin();
    while(it != table.end()){
        if(it->GetName() == name)
            break;
        it++;
    }
    if(it == table.end())
        return NULL;
    else
        return &(*it);
}





