#include<list>
#include<stack>
#include<vector>
#include<string>
using namespace std;
class structItem;
enum TYPE{
    INT, FLOAT, STRUCT, INTARRAY, FLOATARRAY, STRUCTARRAY
};      //all possible type used in c-- language 
class varItem{
public:

    varItem(){}
    varItem(const varItem& a);      //all item class need copy construct function
    varItem(string &name, TYPE type, int lineNo):name(name), type(type), lineNo(lineNo){}
    ~varItem(){}

    const string& GetName(){return name;}
    const TYPE& GetType(){return type;}
    const int& GetLineNo(){return lineNo;}
    const structItem* GetStructType(){return structType;}
private:
    string name;
    TYPE type;
    structItem* structType;      //only the var type is STRUCT
    int dimension;    //only the variable is array
    int lineNo;     //where the variable first appears

};


class funItem{      //when first define or first declare a function, then create a funItem
public:
    funItem(){}
    funItem(const funItem& a);
    funItem(string &funname, TYPE retval):name(funname), retval(retval){}
    ~funItem(){}

    void pushArg(TYPE type){ArgList.push_back(type);}             //uncertain
    const string& GetName(){return name;}
    const TYPE& GetRetType(){return retval;}
    const TYPE& GetArg(int No){return ArgList[No - 1];}

private:
    string name;
    TYPE retval;        //return variable
    vector<TYPE> ArgList;       //arguments list
    
};

class VarSymbolTab{
public:
    void AddItem(varItem item){table.push_back(item);}
    void DeleteItem(char* ItemName);
    const varItem* FindItem(const string& name);       //if can't find, return nullptr
private:
    list<varItem> table;
};



class funSymbalTab{
public:
    void AddItem(funItem item){table.push_back(item);}
    void DeleteItem(char* ItemName);
    const varItem* FindItem(const string& name);
private:
    list<funItem> table;
};


/*it means struct type but not struct var*/
class structItem{
public:
    structItem(){}
    structItem(const structItem& a);
    ~structItem(){}
    
    structItem(string &name):name(name){}
    const string& Getname(){return name;}
    void AddMember(varItem member){MemberVar.push_back(member);}
    const varItem* GetMember(int No){return &MemberVar[No - 1];}

private:
    string name;
    vector<varItem> MemberVar;      //struct member push both in symbletab and membervar
};  