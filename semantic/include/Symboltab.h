#pragma once
#include<list>
#include<vector>
#include<string>
using std::string;
using std::vector;
using std::list;

class Analyze;

class structItem;
enum TYPE
{
    INT,
    FLOAT,
    STRUCT,
    INTARRAY,
    FLOATARRAY,
    STRUCTARRAY,
    VOID
}; //all possible type used in c-- language

/*variable symbol*/
class varItem{
public:
    
    varItem(){}
    varItem(const varItem& a);      //all item class need copy construct function
    varItem(string name, TYPE type, int lineNo, int dimension);
    ~varItem() {}

    string GetName(){return name;}
    const TYPE GetType()const {return type;}
    const int GetLineNo(){return lineNo;}
    const int GetDimension(){return dimension;}
    const int getSize(){
        if(type == VOID){
            return 0;
        }
        else if(type == INT || type == FLOAT || type == STRUCT)
            return basetypeSize;
        else{
            int temp(basetypeSize);
            for (int i(0); i < dimension; i++)
            {
                temp *= dimensionSize[i];
            }
            return temp;
        }
    }
    const int getBaseTypeSize(){
        if(type == VOID){
            return 0;
        }
        else if(type == INT || type == FLOAT || type == STRUCT)
            return basetypeSize;
        else{
            int temp(basetypeSize);
            for (int i(0); i < dimension - 1; i++)
            {
                temp *= dimensionSize[i];
            }
            return temp;
        }
    }
    void setDimensionSize(int DimNo, int size) { 
        if(dimensionSize!= NULL)
            dimensionSize[DimNo - 1] = size; 
    }

    int getDimensionSize(int DimNo){        //get size of each element of given Dimension
        if(dimensionSize == NULL)
            return 0;
        int retval(this->basetypeSize);
        for (int i(0); i < DimNo-1; i++){
            retval *= dimensionSize[i];
        }
        return retval;
    }

    void setDimensionSize(const varItem &a);
    structItem *GetStructType() { return structType; }
    void SetStructType(structItem *type);//if Item type is struct
    void print();
    friend bool operator == (const varItem &a, const varItem &b) ;        //judge if they are the same type
    bool operator != (varItem &a) {return !(*this == a);}
    void setPlace(){
        place = 1;
    }
    void resetPlace(){
        place = 0;
    }
    int getPlace() { return place; }

  private:
    string name;
    TYPE type;
    structItem* structType;      //only the var type is STRUCT
    int dimension;    //only the variable is array
    int lineNo;     //where the variable first appears
    int basetypeSize;
    int *dimensionSize;
    int place; //    1 means in function args, 0 means in stmt;
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
    structItem *GetRetStruct(){return retStruct;}
    bool NotDef(){return DefLine == -1?true:false;}
    int GetDecLine(){return DecLine;}
    int GetArgListSize(){return DefArgList.size();}
    const vector<varItem>& GetArgList() {return DefArgList;}
    bool operator ==(funItem& a);//retval and Arglist are the same

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
    varItem* FindItem(const char*name);       //if can't find, return nullptr
    varItem* FindItem(const string& name);
    int getItemNum(string name);
    void PrintTab();
private:
    list<varItem> table;
};




/*function table*/
class FuncSymbolTab{
public:
friend class Analyze;
    FuncSymbolTab(){}
    ~FuncSymbolTab(){}
    void AddItem(funItem &item){table.push_back(item);}
    void DeleteItem(char* ItemName);
    funItem* FindItem(const char*name);
    funItem* FindItem(const string& name);
    int getItemNum(string name);
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
    varItem* GetMember(int No){return &MemberVar[No - 1];}
    varItem* GetMember(const string& name);
    void print();
    bool operator == (structItem &a);
    bool operator !=(structItem &a){
        return !(*this == a);
    }
    int getSize(){return this->size;}
    void calculateSz()
    {
        size = 0;
        for (int i(0); i < MemberVar.size(); i++)
        {
            size += MemberVar[i].getSize();
        }
    }
    int getOffset(string memberName);

  private:
    string name;
    int FstDecLine;      //first declare line
    int FstDefLine;      /*first define line ,
    sometimes both lines are the same. If FstDefLine = -1, it was undefined */
    vector<varItem> MemberVar;      //struct member push both in symbletab and membervar
    int size;

};

/*struct table*/
class structTab{
public:
friend class Analyze;
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