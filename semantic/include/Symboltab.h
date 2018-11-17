#include<list>
#include<stack>
#include<vector>
#include<string>
using namespace std;

class varItem{
public:
    enum varType{
        INT, FLOAT
    };
    varItem(){}
    varItem(string &name, varType type, int ival):name(name), type(type), ival(ival){}
    varItem(string &name, varType type, float fval):name(name), type(type), fval(fval){}
    ~varItem(){}

    const string GetName(){return name;}
    const varType GetType(){return type;}
    const float GetFval(){return fval;}
    const int GetIval(){return ival;}
    void SetIval(int newVal){ival = newVal;}
    void SetFval(float newVal){fval = newVal;}
private:
    string name;
    varType type;
    union {
        int ival;
        float fval;
    };

};


class funItem{
public:
    enum retType{
        INT, FLOAT, VOID
    };
    enum ArgType{
        Int, Float
    };
    funItem(){}
    funItem(string &funname, retType retval):name(funname), retval(retval){}
    ~funItem(){}

    void pushArg(ArgType type){ArgList.push_back(type);}             //uncertain



private:
    string name;
    retType retval;
    vector<ArgType> ArgList;
    
};

class VarSymbolTab{
public:
    
};