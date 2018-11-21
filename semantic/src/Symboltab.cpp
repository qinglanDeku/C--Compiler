#include"Symboltab.h"
#include<iostream>
using std::cout;
using std::endl;
/*class varItem*/
string StrTYPE[7] = {
    "int", "float", "struct", "intArray", "floatArray", "structArray", "void"
};
varItem::varItem(const varItem& a){
    this->name = a.name;
    this->type = a.type;
    this->structType = a.structType;
    this->dimension = a.dimension;
    /*if(this->type == STRUCT)
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
    else;*/
    this->lineNo = a.lineNo;
}

void varItem::SetStructType(structItem* type){
    this->structType = type;
}

void varItem::print(){
    cout<<"var name:"<<name<<"| "<<"var type:"<<StrTYPE[type]<<"| ";
    if(type == STRUCT || type == STRUCTARRAY){
        cout<<"struct name:"<<structType->GetName()<<"| ";
    }
    cout<<"dimension:"<<dimension<<"| "<<"define line:"<<lineNo<<endl;
}

/*************************************************
class funItem*/
funItem::funItem(const funItem& a){
    this->name = a.name;
    this->retval = a.retval;
    this->DefArgList.assign(a.DefArgList.begin(), a.DefArgList.end());
    this->DecArgList.assign(a.DecArgList.begin(), a.DecArgList.end());
    this->DecLine = a.DecLine;
    this->DefLine = a.DefLine;
}

void funItem::print(){
    cout<<"func name:"<<name<<" |  "<<"retval type:"<<StrTYPE[retval]<<" |  ";
    if(retval == STRUCT)
        cout<<"struct name:"<<retStruct->GetName()<<" |  ";
    for(int i(0); i < DefArgList.size(); i++){
        cout<<"arg "<<i + 1<<"type :";
        if(DecArgList[i] == STRUCT || DecArgList[i] == STRUCTARRAY){
            cout<<"struct "<<DefArgList[i].GetStructType()->GetName();
        }
        else{
            cout<<StrTYPE[DefArgList[i].GetType()];
        }
        for(int j(0); j<DefArgList[i].GetDimension(); j++)
            cout<<"[]";
        cout<<"   ";
    }
    cout<<" |  ";
    cout<<"DefLine:"<<DefLine<<endl;
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

const varItem* VarSymbolTab::FindItem(const string &name){
    list<varItem>::iterator it = table.begin();
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

void VarSymbolTab::PrintTab(){
    list<varItem>::iterator it(table.begin());
    while(it != table.end()){
        it->print();
        cout<<"-------------"<<endl;
        it++;
    }
    cout<<"==================================="<<endl;
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

void FuncSymbolTab::PrintTab(){
    list<funItem>::iterator it(table.begin());
    while(it != table.end()){
        it->print();
        cout<<"-----------------------"<<endl;
        it++;
    }
    cout<<"============================================="<<endl;
}



/****************************************************
 class structItem*/
 structItem::structItem(const structItem& a){
     this->name = a.name;
     this->FstDecLine = a.FstDecLine;
     this->FstDefLine = a.FstDefLine;
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

 void structItem::print(){
     cout<<"struct name:"<<name<<" |  " ;
     for(int i(0); i < MemberVar.size();i++){
        cout<<"struct Member"<<i+1<<" :";
        MemberVar[i].print();
    }
    cout<<"|  "<<"struct DefLine:"<<FstDefLine<<endl;
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

structItem* structTab::FindItem(const char* name){
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

structItem* structTab::FindItem(const string& name){
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

void structTab::PrintList(){
    list<structItem>::iterator it(table.begin());
    while(it != table.end()){
        it->print();
        cout<<"------------------------"<<endl;
        it++;
    }
    cout<<"============================================="<<endl;
}





