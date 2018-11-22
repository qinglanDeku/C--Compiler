#pragma once
#include<list>
#include<vector>
#include<string>
using std::string;
using std::vector;
using std::list;

class structItem;
enum TYPE{
    INT, FLOAT, STRUCT, INTARRAY, FLOATARRAY, STRUCTARRAY, VOID
};      //all possible type used in c-- language 



/*variable symbol*/
class varItem{
public:
    
    varItem(){}
    varItem(const varItem& a);      //all item class need copy construct function
    varItem(string &name, TYPE type, int lineNo, int dimension):name(name), \
    type(type), lineNo(lineNo), dimension(dimension), structType(NULL){}
    ~varItem(){}

    const string& GetName(){return name;}
    const TYPE& GetType(){return type;}
    const int& GetLineNo(){return lineNo;}
    const int GetDimension(){return dimension;}
    structItem* GetStructType(){return structType;}
    void SetStructType(structItem *type);//if Item type is struct
    void print();
    bool operator == (varItem &a) const;        //judge if they are the same type
    bool operator != (varItem &a) const{return !(*this == a);}
    
private:
    string name;
    TYPE type;
    structItem* structType;      //only the var type is STRUCT
    int dimension;    //only the variable is array
    int lineNo;     //where the variable first appears

};


/*function symbol*/
class funItem{      //when first define or first declare a function, then create a funItem
public:
    funItem(){}
    funItem(const funItem& a);
    funItem(string &funname, TYPE retval, int DecLine):name(funname), retval(retval),\
        DecLine(DecLine), DefLine(-1){}
    funItem(string &funname, TYPE retval, int DecLine, int DefLine):\
        name(funname), retval(retval), DecLine(DecLine), DefLine(DefLine){}
    ~funItem(){}

    //void pushDecArg(TYPE type){DecArgList.push_back(type);}             //uncertain
    void pushDefArg(varItem &arg){DefArgList.push_back(arg);}
    void SetRetStruct(structItem* structType){retStruct = structType;}
    const string& GetName(){return name;}
    const TYPE& GetRetType(){return retval;}
    //const TYPE& GetDecArg(int No){return DecArgList[No - 1];}         //flawed
    varItem* GetDefArg(int No){return &DefArgList[No -1];}
    void setDefLine(int line){DefLine = line;}      //sometimes DefLine = DecLine;
    void print();
    void CopyDefArgList(funItem& a){
        this->DefArgList.clear();
        this->DefArgList.assign(a.DefArgList.begin(), a.DefArgList.end());
    }
    bool NotDef(){return DefLine == -1?true:false;}
    bool operator ==(funItem& a)const;//retval and Arglist are the same

private:
    string name;
    TYPE retval;        //return variable
    structItem* retStruct;  //if return var is a struct
    //vector<TYPE> DecArgList;       //arguments list,using in declareation
    vector<varItem> DefArgList;
    int DecLine;        //where declaration of this function shows
    int DefLine;        //where define of the function shows
    
};




/*variable table*/
class VarSymbolTab{
public:
    VarSymbolTab(){}
    ~VarSymbolTab(){}
    void AddItem(varItem &item){table.push_back(item);}
    void DeleteItem(char* ItemName);
    const varItem* FindItem(const char*name);       //if can't find, return nullptr
    const varItem* FindItem(const string& name);   
    void PrintTab();
private:
    list<varItem> table;
};




/*function table*/
class FuncSymbolTab{
public:
    FuncSymbolTab(){}
    ~FuncSymbolTab(){}
    void AddItem(funItem &item){table.push_back(item);}
    void DeleteItem(char* ItemName);
    funItem* FindItem(const char*name);
    funItem* FindItem(const string& name);
    void PrintTab();
private:
    list<funItem> table;
};





/* struct symbol
it means struct type but not struct variable */
class structItem{
public:
    structItem(){}
    structItem(const structItem& a);
    ~structItem(){}
    
    structItem(string &name, int FstDecLine, int FstDefLine):\
        name(name), FstDecLine(FstDecLine), FstDefLine(FstDefLine){}
    const string& GetName(){return name;}
    void AddMember(varItem member){MemberVar.push_back(member);}
    void SetFstDefLine(int FstDefLine){this->FstDefLine = FstDefLine;}
    int GetFstDefLine() const {return this->FstDefLine;}
    int MemberNum() const {return MemberVar.size();}
    const varItem* GetMember(int No){return &MemberVar[No - 1];}
    const varItem* GetMember(const string& name);
    void print();
    bool operator == (structItem &a)const;
    bool operator !=(structItem &a)const{
        return !(*this == a);
    }

private:
    string name;
    int FstDecLine;      //first declare line
    int FstDefLine;      /*first define line ,
    sometimes both lines are the same. If FstDefLine = -1, it was undefined */
    vector<varItem> MemberVar;      //struct member push both in symbletab and membervar
};  


/*struct table*/
class structTab{
public:
    structTab(){}
    ~structTab(){}
    void AddItem(structItem &item){table.push_back(item);}
    void DeleteItem(char *ItemName);
    int TabSize(){return table.size();}
    void PrintList();
    structItem* GetBack(){
        return &(table.back());
    }
    structItem* FindItem(const char* name);
    structItem* FindItem(const string& name);
    
private:
    list<structItem> table;
};