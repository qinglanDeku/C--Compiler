#include<string>
#include<vector>
using std::string;
using std::vector;


class SemanticError{
public:
    static string errorType[19];        //initial in cpp, include 17 types
    SemanticError(){}
    SemanticError(const SemanticError& a):lineNo(a.lineNo), ObjectName(a.ObjectName),\
        errorNo(a.errorNo), errorInfo(a.errorInfo){}
    SemanticError(int lineNo, string &ObjectName, int errorNo):\
        lineNo(lineNo), ObjectName(ObjectName), errorNo(errorNo){}
    ~SemanticError(){}
    /*Splice private Member errorInfo*/
    void SpliceErrorInfo(int lineNo, string ObjectName, int errorNo);
    friend class SemanticErrorList;
private:
    int lineNo;
    string ObjectName;
    int errorNo;        //from 1 to 17
    string errorInfo;
};


/*error list, create a global variable to use*/
class SemanticErrorList{    
    void PrintErrorList();
    void AddError(SemanticError &error){errorList.push_back(error);}
private:
    vector<SemanticError> errorList;
};

