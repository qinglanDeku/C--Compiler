#pragma once
#include<string>
#include<vector>
using std::string;
using std::vector;


class SemanticError{
public:
    static string errorType[20];        //initial in cpp, include 17 types
    SemanticError(){}
    SemanticError(const SemanticError& a):lineNo(a.lineNo), ObjectName(a.ObjectName),\
        errorNo(a.errorNo), errorInfo(a.errorInfo){}
    SemanticError(int lineNo, string ObjectName, int errorNo):\
        lineNo(lineNo), ObjectName(ObjectName), errorNo(errorNo){
            SpliceErrorInfo(this->lineNo, this->ObjectName, this->errorNo);
        }
    ~SemanticError(){}
    /*Splice private Member errorInfo*/
    
    friend class SemanticErrorList;
private:
    void SpliceErrorInfo(int lineNo, string ObjectName, int errorNo);
    int lineNo;
    string ObjectName;
    int errorNo;        //from 1 to 17
    string errorInfo;
};


/*error list, create a global variable to use*/
class SemanticErrorList{    
public:
    void PrintErrorList();
    void AddError(SemanticError &error){errorList.push_back(error);}
    int size(){return errorList.size();}
private:
    vector<SemanticError> errorList;
};

