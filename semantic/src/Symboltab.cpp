#include"Symboltab.h"
#include<iostream>
#include<assert.h>
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
    this->basetypeSize = a.basetypeSize;
    if(a.dimension != 0){
        this->dimensionSize = new int[a.dimension];
        for (int i(0); i < a.dimension; i++)
        {
            this->dimensionSize[i] = a.dimensionSize[i];
        }
    }
    else
        this->dimensionSize = NULL;
}

varItem::varItem(string name, TYPE type, int lineNo, int dimension):name(name), \
type(type), lineNo(lineNo), dimension(dimension), structType(NULL)
{
    if(type == STRUCT || type == STRUCTARRAY){
        ;
    }
    else if(type == VOID){
        basetypeSize = 0;
    }
    else
        basetypeSize = 4;
    
    if(dimension == 0)
        this->dimensionSize = NULL;
    else{
        this->dimensionSize = new int[dimension];
    }
}


void varItem::SetStructType(structItem* type){
    this->structType = type;
    if(type != NULL)
        basetypeSize = structType->getSize();
}

void varItem::print(){
    cout<<"var name:"<<name<<"| "<<"var type:"<<StrTYPE[type]<<"| ";
    if(type == STRUCT || type == STRUCTARRAY){
        cout<<"struct name:"<<structType->GetName()<<"| ";
    }
    cout<<"dimension:"<<dimension<<" | ";
    if(dimension > 0){
        for (int i(0); i < dimension; i++)
            cout << "Dimen" << i + 1 << ":" << dimensionSize[i] << " ;";
        cout << endl;
        cout << "variable size: " << getSize();
    }
    else{
        cout << "variable size: " << getSize()<<"|";
        
    }
    cout << "basetype size: " << this->basetypeSize;
    cout<<endl<<"define line:"<<lineNo<<endl;
}


bool operator== (const varItem &a, const varItem &b) {
    if(a.type != STRUCT && a.type != STRUCTARRAY && a.type == b.type){
        if(a.dimension == b.dimension)
            return true;
    }
    else if(a.type == STRUCT && a.type == b.type){
        if(a.structType == b.structType)
            return true;
        else{
            if(*(a.structType) == *(b.structType))
                return true;
        } 
    }
    else if(a.type == STRUCTARRAY &&a.type == b.type){
        if(a.structType == b.structType && a.dimension == b.dimension)
            return true;
        else{
            if(*(a.structType) == *(b.structType) && a.dimension == b.dimension)
                return true;
        }
    }
    return false;
}

/*************************************************
class funItem*/
funItem::funItem(const funItem& a){
    this->name = a.name;
    this->retval = a.retval;
    this->retStruct = a.retStruct;
    this->DefArgList.assign(a.DefArgList.begin(), a.DefArgList.end());
   // this->DecArgList.assign(a.DecArgList.begin(), a.DecArgList.end());
    this->DecLine = a.DecLine;
    this->DefLine = a.DefLine;
}

void funItem::print(){
    cout<<"func name:"<<name<<"  |";
    cout<<"retval type :";
    if(retval == INT){
        cout<<"int  |";
    }
    else if(retval == FLOAT){
        cout<<"float  |";
    }
    else{
        cout<<"struct  |";
    }
    if(retval == STRUCT){
        cout<<"struct name:"<<retStruct->GetName()<<" |  ";
    }
    for(int i(0); i < DefArgList.size(); i++){
        cout<<"arg"<<i + 1<<" type :";
        if(DefArgList[i].GetType() == STRUCT || DefArgList[i].GetType() == STRUCTARRAY){
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

bool funItem::operator==(funItem& a){
    if(this->retval != a.retval)
        return false;
    else if(this->retval == STRUCT)
        if(this->retStruct != a.retStruct){
            if(*(this->retStruct) != *(a.retStruct))
                return false;
        }
    
    if(this->DefArgList.size() != a.DefArgList.size())
        return false;
    
    for(int i(0); i < this->DefArgList.size(); i++ ){
        if(this->DefArgList[i] != a.DefArgList[i])
            return false;
    }

    return true;
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

varItem* VarSymbolTab::FindItem(const char*name){
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

varItem* VarSymbolTab::FindItem(const string &name){
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

int VarSymbolTab::getItemNum (string name){
    list<varItem>::iterator it(table.begin());
    int num(1);
    while (it != table.end()){
        if(it->GetName() == name){
            return num;
        }
        num += 1;
        it++;
    }
    return -1;//not find
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

funItem* FuncSymbolTab::FindItem(const char*name){
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

funItem* FuncSymbolTab::FindItem(const string& name){
    list<funItem>::iterator it = table.begin();
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

int FuncSymbolTab::getItemNum(string name){
    list<funItem>::iterator it(table.begin());
    int num(1);
    while(it != table.end()){
        if(it->GetName() == name){
            return num;
        }
        num += 1;
        it++;
    }
    return -1;
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
     this->size = a.size;
 }

 varItem *structItem::GetMember(const string &name)
 {
     varItem *p = NULL;
     for (int i(0); i < MemberVar.size(); i++)
     {
         if (MemberVar[i].GetName() == name)
         {
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
    cout << "struct size: " << getSize();
    cout << endl<< "struct DefLine:" << FstDefLine << endl;
 }

 bool structItem::operator ==(structItem &a){
     if((this->name != a.name) || (this->name == a.name && this->name == "")){      
        if(this->MemberVar.size() != a.MemberVar.size())
            return false;
        else{
            for(int i(0); i< this->MemberVar.size(); i++){
                if(this->MemberVar[i] != a.MemberVar[i])
                    return false;
            }
        }
     }
     
     return true;
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





