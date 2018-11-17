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
    type(type), lineNo(lineNo), dimension(dimension){}
    ~varItem(){}

    const string& GetName(){return name;}
    const TYPE& GetType(){return type;}
    const int& GetLineNo(){return lineNo;}
    const structItem* GetStructType(){return structType;}
    void SetStructType(structItem *type);//if Item type is struct
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
    funItem(string &funname, TYPE retval):name(funname), retval(retval){}
    ~funItem(){}

    void pushArg(TYPE type){ArgList.push_back(type);}             //uncertain
    const string& GetName(){return name;}
    const TYPE& GetRetType(){return retval;}
    const TYPE& GetArg(int No){return ArgList[No - 1];}         //flawed

private:
    string name;
    TYPE retval;        //return variable
    vector<TYPE> ArgList;       //arguments list
    
};




/*variable table*/
class VarSymbolTab{
public:
    VarSymbolTab(){}
    ~VarSymbolTab(){}
    void AddItem(varItem &item){table.push_back(item);}
    void DeleteItem(char* ItemName);
    const varItem* FindItem(const char*name);       //if can't find, return nullptr
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
    const funItem* FindItem(const char*name);
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
    
    structItem(string &name):name(name){}
    const string& GetName(){return name;}
    void AddMember(varItem member){MemberVar.push_back(member);}
    const varItem* GetMember(int No){return &MemberVar[No - 1];}
    const varItem* GetMember(const string& name);

private:
    string name;
    vector<varItem> MemberVar;      //struct member push both in symbletab and membervar
};  


/*struct table*/
class structTab{
    structTab(){}
    ~structTab(){}
    void AddItem(structItem &item){table.push_back(item);}
    void DeleteItem(char *ItemName);
    const structItem* FindItem(const char* name);
    
private:
    list<structItem> table;
};